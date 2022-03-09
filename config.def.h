#include <linux/limits.h>


/* Default env values */
const char default_crealtime_bin_path[] = "/tmp/crealtime";

/* path_max * 2 + command length itself */
const int max_compiler_command_size = PATH_MAX*2 + 22;

/* 1 %s - source path
 * 2 %s - output path
 */
const char compiler_command_template[] = "clang -x c \"%s\" -o \"%s\"";

