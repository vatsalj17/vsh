#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <stdbool.h>
#include <sys/types.h>

void handle_redirections(char *input_file, char *output_file, char *append_file);
bool found_pipe(char **command);
void exec_pipe(char ***new_cmds, int i, int n);
pid_t execute_command(char **command, double *time);

#endif
