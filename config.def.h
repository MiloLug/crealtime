#include <linux/limits.h>

/* if undef this - all the defaults will be used */
#define USE_ENV_VARS

/* if undef this - the program won't try to `mkdir -p` the path for bins
 * and you'll have to create it on your own.
 * 
 * It'll remove a big part of the app however :)
 * */
#define MKDIR_CREALTIME_BIN_PATH

/* Default env values */
const char default_crealtime_bin_path[] = "/tmp/crealtime";

/* path_max * 2 + command length itself */
const int max_compiler_command_size = PATH_MAX*2 + 22;

/* 1 %s - source path
 * 2 %s - output path
 */
const char compiler_command_template[] = "clang -x c \"%s\" -o \"%s\"";

