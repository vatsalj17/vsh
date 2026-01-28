#include "shell.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/history.h>
#include <readline/readline.h>

#include "colors.h"
#include "parser.h"
#include "utils.h"

static void format_time(double ms, double* value, const char** unit) {
	if (ms < 1000.0) {
		*value = ms;
		*unit = "ms";
	} else if (ms < 60 * 1000.0) {
		*value = ms / 1000.0;
		*unit = "s";
	} else if (ms < 60 * 60 * 1000.0) {
		*value = ms / (60 * 1000.0);
		*unit = "min";
	} else {
		*value = ms / (60 * 60 * 1000.0);
		*unit = "h";
	}
}

void print_shell(char* shell, size_t shell_size, char* username, char* hostname, char* path, double time_ms, int status) {
	char time_buf[64] = "";
	char status_buf[32] = "";

	if (time_ms > 0.0) {
		double tval;
		const char* tunit;
		format_time(time_ms, &tval, &tunit);
		snprintf(time_buf, sizeof(time_buf), " " YEL "took %.2f%s", tval, tunit);
	}

	if (status)
		snprintf(status_buf, sizeof(status_buf), RED " %d", status);
	else
		snprintf(status_buf, sizeof(status_buf), GRN);

	snprintf(shell, shell_size, BHGRN "\n%s" BHRED "@%s" reset " - " CYN
									  "%s"
									  "%s"
									  "%s"
									  "\n⊱ " reset,
			 username, hostname, path, time_buf, status_buf);
}

char** read_line(pid_t status, double time) {
	char** command;
	char* input;
	char* username = vsh_get_username();
	char* home = vsh_get_homedir();
	char hostname[256];
	gethostname(hostname, sizeof(hostname));
	char* path = vsh_get_path(home);
	if (path == NULL) {
		perror("path");
		free(path);
		return NULL;
	}
	size_t shell_size = strlen(username) + strlen(hostname) + strlen(path) + 100;
	char* shell = (char*)malloc(shell_size);
	print_shell(shell, shell_size, username, hostname, path, time, status);
	input = readline(shell);
	if (input == NULL) {
		printf("Exiting shell...\n");
		free(path);
		free(shell);
		free(input);
		exit(0);
	} else if (input && *input) {
		add_history(input);
		append_history(1, vsh_history_path());
	}
	command = tokenize_input(input);
	free(input);
	if (command[0] == NULL) {
		free(path);
		free(shell);
		free(command);
		return NULL;
	}
	free(path);
	free(shell);
	return command;
}
