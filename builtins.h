#ifndef BUILTINS_H
#define BUILTINS_H

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#define PERM 0644
#define MAX_ARGS 32
#define MAX_ARGS_LENGTH 50

void my_exit();
int my_cp(char **argv);
int my_ls(char **argv);
int my_chmod(char **argv);

#endif

