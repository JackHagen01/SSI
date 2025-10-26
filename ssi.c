#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

// Singly linked list to store background jobs
typedef struct background_job {
	pid_t pid;
	char *command;
	struct background_job *next;
} background_job;

background_job *bg_head = NULL;

// Global pid variable to track the current foreground process
pid_t fg_pid = -1;

// print_prompt function, no input or return values
// Prints the prompt for the user to input commands
void print_prompt() {
	char hostname[256];
	char cwd[1024];

	// Current user's login name
	char *username = getlogin();

	// Name of machine
	gethostname(hostname, sizeof(hostname));

	// Current working directory
	getcwd(cwd, sizeof(cwd));

	// Formats and prints shell prompt
	printf("%s@%s: %s > ", username, hostname, cwd);
}

// parse_input function
// Takes string input and returns tokenized string
char **parse_input(char *input) {
	char **args = malloc(MAX_ARGS * sizeof(char *));
	int i = 0;

	// Splits input string into tokens
	char *token = strtok(input, " \t\n");

	// Each token stored in an array
	while (token != NULL && i < MAX_ARGS - 1) {
		args[i++] = token;
		token = strtok(NULL, " \t\n");
	}

	// End of token array
	args[i] = NULL;
	return args;
}

// execute_command function, no return
// Executes command inputted by user
// Takes tokenized input string as argument
void execute_command(char **args) {
	// Creates new process
	pid_t pid = fork();
	if (pid == 0) {
		// Runs command in child process
		execvp(args[0], args);

		// Prints error message if comman fails
		perror("execvp failed");
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		// Sets the current foreground pid
		fg_pid = pid;

		// Parent process waits for child to finish
		wait(NULL);
		
		// Resets current foreground pid
		fg_pid = -1;
	} else {
		perror("fork failed");
	}
}

// add_job function, takes pid and command as input
// Adds a new background job to the list
void add_job(pid_t pid, char *command) {
	background_job *new_job = malloc(sizeof(background_job));
	new_job->pid = pid;
	new_job->command = strdup(command);
	new_job->next = bg_head;
	bg_head = new_job;
}

// remove_job function, takes pid as input
// Removes a background job from the list
void remove_job(pid_t pid) {
	background_job **cur = &bg_head;
	while (*cur) {
		if ((*cur)->pid == pid) {
			background_job *temp = *cur;
			*cur = temp->next;
			printf("%d: %s has terminated.\n",temp->pid,temp->command);
			free(temp->command);
			free(temp);
			return;
		}
		cur = &((*cur)->next);
	}
}

// check_jobs function, no input
// Checks the status of jobs, removes terminated jobs
void check_jobs() {
	int status;
	pid_t pid;
	while ((pid = waitpid(-1,&status,WNOHANG)) > 0) {
		remove_job(pid);
	}
}

// Handles SIGINT sent by CTRL-C input, no return value
// Kills current foreground process or prints new prompt
void handle_sigint() {
	// Kills current foreground process if it exists
	if (fg_pid > 0) {
		kill(fg_pid, SIGINT);
	} else {
		// Prints a new line and prompt if no foreground process is running
		printf("\n");
		print_prompt();
		// Flushes prompt output
		fflush(stdout);
	}
	return;
}

int main() {
	// Registers the handler function for SIGINT input
	signal(SIGINT, handle_sigint);

	char input[MAX_INPUT];
	
	// Infinite loop until user exits shell
	while (1) {
		// Checks status of jobs iteration
		check_jobs();

		// Prints shell prompt
		print_prompt();

		// Reads line of input from user, CTRL-D is read as NULL
		if (fgets(input, sizeof(input), stdin) == NULL) {
			printf("\n");
			break;
		}

		// Ignores empty input line
		if (input[0] == '\n') {
			continue;
		}

		input[strcspn(input, "\n")] = '\0';
		
		// Input is parsed
		char **args = parse_input(input);
		
		// cd command is treated as a special case since it cannot be passed to exec
		if (strcmp(args[0], "cd") == 0) {
			char *target = args[1];

			// No argument or ~ send the user to their home directory
			if (target == NULL || strcmp(target, "~") == 0) {
				// Gets the users's home directory
				chdir(getenv("HOME"));
				continue;
			} else {
				if (strncmp(target, "~/",2) == 0) {
					char full_path[MAX_ARGS];
					snprintf(full_path, sizeof(full_path), "%s/%s", getenv("HOME"),target + 2);
					target = full_path;
				}
			}

			// Attempts to change directory
			if (chdir(target) != 0) {
				perror("cd failed");
			}
		} else if (strcmp(args[0], "bg") == 0) {
			// Adds a new background job based on user input
			if (args[1] == NULL) {
				fprintf(stderr, "bg: missing command\n");
			} else {
				pid_t pid = fork();
				if (pid == 0) {
					execvp(args[1], &args[1]);
					perror("exec failed");
					exit(1);
				} else {
					char full_cmd[1024] = {0};
					for (int i = 1; args[i]; ++i) {
						strcat(full_cmd, args[i]);
						strcat(full_cmd, " ");
					}
					add_job(pid, full_cmd);
				}
			}
		} else if (strcmp(args[0], "bglist") == 0) {
			// Removes terminated jobs before printing the list
			check_jobs();
			
			// Lists background jobs
			int count = 0;
			for (background_job *cur = bg_head; cur; cur = cur->next) {
				printf("%d: %s\n", cur->pid, cur->command);
				count++;
			}
			printf("Total Background jobs: %d\n",count);
		} else {
			// Executes command
			execute_command(args);
		}

		// Allocated memory is freed
		free(args);
	}
	return 0;
}
