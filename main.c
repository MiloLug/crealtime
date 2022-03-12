#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <linux/limits.h>  // PATH_MAX
#include <libgen.h>
#include <unistd.h>
#include <stdbool.h>

#include "config.h"


inline char *strdup(const char *src)
{
    char *str;
    char *p;
    int len = 0;

    while (src[len])
        len++;
    
    p = str = malloc(len + 1);
    
    while (*src)
        *p++ = *src++;
    *p = '\0';

    return str;
}

void join_strings(const char * const *strs, char *result)
{
    for(;*strs != NULL; strs++){
        strcpy(result, *strs);
        result += strlen(*strs);
    }
}

#ifdef MKDIR_CREALTIME_BIN_PATH
void mkdir_p(const char *dir)
{
    struct stat check_stat;
    int err = stat(dir, &check_stat);
    if (err != -1)
        return;
    else if (errno != ENOENT) {
        perror("Error [mkdir_p] stat");
        exit(errno);
    }

    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len = strlen(dir);

    strcpy(tmp, dir);

    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;

    for (p = tmp + 1; *p; p++)
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    mkdir(tmp, S_IRWXU);
}
#endif

bool try_compile(const char* file_path, const char* bin_path, bool force)
{
    struct stat file_stat;
    struct stat bin_stat;
    int err;

    err = stat(file_path, &file_stat);
    if (err != 0) {
        perror("Error [try_compile] stat");
        exit(errno);
    }

    char command[max_compiler_command_size];
    command[0] = 0;

    err = stat(bin_path, &bin_stat);
    if (err != 0 || file_stat.st_mtime > bin_stat.st_mtime || force) {
        sprintf(command, compiler_command_template, file_path, bin_path);
        err = system(command);
        if (err == -1) {
            return false;
        }
    }

    return true;
}


int main(int argc, char *argv[])
{
    /* help */
    if (argc < 2) {
        printf(
            "\n"
            "Usage: crealtime [flags] <file-path> [{ arg1 arg2 ...}]\n"
            "Example: crealtime -f /bin/join_strings { , hello world }\n\n"
            "Parameters:\n"
            "    <file-path> - string; event-source path\n\n"
            "Flags:\n"
            "    -f - force recompilation\n"
            "    -c - compile the source but not run it\n\n"
#ifdef USE_ENV_VARS
            "Env variables:\n"
            "    CREALTIME_BIN_PATH - string; a path, where temporary binaries will reside\n\n"
#endif
        );
        return 0;
    }
    
    /* env initialization */
#ifdef USE_ENV_VARS
    const char *crealtime_bin_path = getenv("CREALTIME_BIN_PATH");
    if (!crealtime_bin_path)
        crealtime_bin_path = default_crealtime_bin_path;
#else
    const char *crealtime_bin_path = default_crealtime_bin_path;
#endif
    /* args initialization */
    const char *file_path = argv[argc - 1];
    char **sub_args_start = NULL;  // arguments for the compiled application
    char **sub_args_end = NULL;
    int own_argc = argc;  // own args, without sub args
    
    bool compile_only = false;
    bool force_recompile = false;
    
    /* finding sub args start and changing own args count */
    if (*file_path == '}') {
        char **tmp_args;
        tmp_args = sub_args_start = &argv[argc - 2];
        sub_args_end = &argv[argc-1];
        
        /* searching for the first bracket */
        while (true) {
            while (**tmp_args != '{' && tmp_args != argv) tmp_args--;
            if (tmp_args == argv)
                break;
            
            sub_args_start = tmp_args;
            tmp_args--;
        }
        
        /* here sub_args_start is at the first '{' position */
        file_path = *(sub_args_start - 1);
        own_argc = (sub_args_start++) - argv;
    }

    /* checking options */
    if (own_argc > 2) {
        char *cur_arg;
        
        for (int i = 1; i < own_argc; i++) {
            cur_arg = argv[i];
            
            if (cur_arg[0] == '-') switch (cur_arg[1]) {
                case 'c':
                    compile_only = true;
                    break;
                case 'f':
                    force_recompile = true;
                    break;
            }
        }
    }
    
    /* paths gen */
    char file_full_path[PATH_MAX];
    char bin_full_path[PATH_MAX];
    char *res = realpath(file_path, file_full_path);
    if (!res) {
        perror("Error [main] realpath");
        exit(errno);
    }
    
    char *tmp_bin_full_path = bin_full_path;
    const char *tmp_file_full_path = file_full_path;
    const char *tmp_crealtime_bin_path = crealtime_bin_path;
    while (*tmp_crealtime_bin_path) {
        *(tmp_bin_full_path++) = *(tmp_crealtime_bin_path++);
    }
    *(tmp_bin_full_path++) = *(tmp_file_full_path++);
    while (*tmp_file_full_path) {
        *(tmp_bin_full_path++) = *tmp_file_full_path == '/' ? '%' : *tmp_file_full_path;
        tmp_file_full_path++;
    }
    *tmp_bin_full_path = 0;

    /* compilation + running */
#ifdef MKDIR_CREALTIME_BIN_PATH
    mkdir_p(crealtime_bin_path);
#endif
    if (try_compile(file_full_path, bin_full_path, force_recompile)) {
        char **sub_args_list;
        char **tmp_sub_args_list;
        char **tmp_sub_args_start = sub_args_start;

        if (sub_args_start == NULL) {
            sub_args_list = malloc(sizeof(char*) * 2);
            sub_args_list[0] = bin_full_path;
            sub_args_list[1] = NULL;
        } else {
            /* array len
             * = argc - own_argc - two_brackets + bin_path + NULL_ending
             * = argc - own_argc
             * */
            tmp_sub_args_list = sub_args_list = (char**)malloc(sizeof(char*) * (argc - own_argc));

            *(tmp_sub_args_list++) = bin_full_path;
            while (tmp_sub_args_start != sub_args_end) {
                *(tmp_sub_args_list++) = *(tmp_sub_args_start++);
            }
            *tmp_sub_args_list = NULL;
        }
        execvp(bin_full_path, sub_args_list);
    } else {
        printf("Compilation error");
        exit(1);
    }

    return 0;
}
