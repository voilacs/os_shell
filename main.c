#include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h> 
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <signal.h>
    #include <time.h>
    #define MAX_INPUT_SIZE 1024
    #define HISTORY_SIZE 100  
struct CommandHistory {
    char commands[HISTORY_SIZE][MAX_INPUT_SIZE];
    int count;
    pid_t pids[HISTORY_SIZE];
    time_t start_times[HISTORY_SIZE];
    double durations[HISTORY_SIZE];
};
struct BackgroundProcess {
    pid_t pid;
    char command[MAX_INPUT_SIZE];
};

struct BackgroundProcess background_processes[100];   
int backg_procs = 0;
struct CommandHistory history;


void add_to_history(char *command, pid_t pid, time_t start_time, double duration) {
    if (history.count < HISTORY_SIZE) {
        strcpy(history.commands[history.count], command);
        history.pids[history.count] = pid;
        history.start_times[history.count] = start_time;
        history.durations[history.count] = duration;
        history.count++;
    } else {
        for (int i = 0; i < HISTORY_SIZE - 1; i++) {
            strcpy(history.commands[i], history.commands[i + 1]);
            history.pids[i] = history.pids[i + 1];
            history.start_times[i] = history.start_times[i + 1];
            history.durations[i] = history.durations[i + 1];
        }
        strcpy(history.commands[HISTORY_SIZE - 1], command);
        history.pids[HISTORY_SIZE - 1] = pid;
        history.start_times[HISTORY_SIZE - 1] = start_time;
        history.durations[HISTORY_SIZE - 1] = duration;
    }
}


void display_history() {
    printf("Command History:\n");
    for (int i = 0; i < history.count; i++) {
        printf("%d: %s\n", i + 1, history.commands[i]);
    }
}
    void sigint_handler(int signum) {   
        static int counter = 0;
        if(signum == SIGINT) {
        char buff1[23] = "\nCaught SIGINT signal\n";
        write(STDOUT_FILENO, buff1, 23);
            if(counter++==1) {
        char buff2[20] = "Cannot handle more\n";
        write(STDOUT_FILENO, buff2, 20);
        exit(0);
        }
        } else if (signum == SIGCHLD) {
        char buff1[23] = "Caught SIGCHLD signal\n";
        write(STDOUT_FILENO, buff1, 23);
        }
    }

   #include <time.h>

int launch(char *command) {
    pid_t pid;
    int status;
    int background = 0;

    if (command[strlen(command) - 1] == '&') {
        background = 1;
        command[strlen(command) - 1] = '\0'; // Remove the '&' from the command
    }

    pid = fork();

    if (pid == 0) {
        // In the child process
        char *args[MAX_INPUT_SIZE];
        int i = 0;

        // Tokenize the command
        char *token = strtok(command, " \t\n");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " \t\n");
        }
        args[i] = NULL;

        if (execvp(args[0], args) == -1) {
            perror("error in shell");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        // Forking error
        perror("error in shell");
    } else {
        // In the parent process
        if (!background) {
            do {
                waitpid(pid, &status, WUNTRACED);
                // if (waitpid(pid, &status, WUNTRACED) == -1) {
                //     perror("waitpid");
                //     exit(EXIT_FAILURE);
                // }

            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }

    return 1;
}


    void display_background_processes() {
    printf("Background Processes:\n");
    for (int i = 0; i < backg_procs; i++) {
        printf("PID: %d - Command: %s\n", background_processes[i].pid, background_processes[i].command);
    }
}
void execute_script(const char *filename) {
    FILE *script_file = fopen(filename, "r");
    if (script_file == NULL) {
        perror("error in shell");
        return;
    }

    char line[MAX_INPUT_SIZE];
    while (fgets(line, MAX_INPUT_SIZE, script_file) != NULL) {
        // Remove newline character
        line[strlen(line) - 1] = '\0';

        if (strlen(line) > 0) {
            printf("Executing: %s\n", line);
            // Dummy values for script commands: -1 for PID, -1 for start time, and -1.0 for duration
            add_to_history(line, -1, -1, -1.0);
            launch(line);
        }
    }

    fclose(script_file);
}


void shell_exit() {
    printf("Exiting shell...\n");

    // Display execution details for commands in the history
    printf("Execution Details:\n");
    for (int i = 0; i < history.count; i++) {
        printf("Command: %s\n", history.commands[i]);
        printf("PID: %d\n", history.pids[i]);
        printf("Start Time: %s", ctime(&history.start_times[i]));
        printf("Duration: %.2f seconds\n", history.durations[i]);
        printf("\n");
    }
}
// Function to execute a command with piping
int launch_with_pipes(char *command) {
    pid_t pid;
    int status;

    // Split the command by pipe characters '|'
    char *commands[10]; // Adjust the size as needed
    int num_commands = 0;
    char *token = strtok(command, "|");
    while (token != NULL) {
        commands[num_commands] = token;
        num_commands++;
        token = strtok(NULL, "|");
    }

    // Create an array to store file descriptors for pipes
    int pipe_fds[10][2]; // Adjust the size as needed

    // Create the pipes
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipe_fds[i]) == -1) {
            perror("error implementing pipes");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_commands; i++) {
        pid = fork();

        if (pid == 0) {
            // In the child process

            // Close the appropriate pipe file descriptors
            if (i > 0) {
                close(pipe_fds[i - 1][1]); // Close write end of previous pipe
                dup2(pipe_fds[i - 1][0], STDIN_FILENO); // Redirect stdin from the previous pipe
                close(pipe_fds[i - 1][0]); // Close read end of previous pipe
            }
            if (i < num_commands - 1) {
                close(pipe_fds[i][0]); // Close read end of current pipe
                dup2(pipe_fds[i][1], STDOUT_FILENO); // Redirect stdout to the current pipe
                close(pipe_fds[i][1]); // Close write end of current pipe
            }

            // Tokenize and execute the command
            char *args[MAX_INPUT_SIZE];
            int j = 0;
            char *token = strtok(commands[i], " \t\n");
            while (token != NULL) {
                args[j++] = token;
                token = strtok(NULL, " \t\n");
            }
            args[j] = NULL;

            // Execute the command using execvp
            if (execvp(args[0], args) == -1) {
                perror("error in shell");
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {
            // Forking error
            perror("error in shell");
            exit(EXIT_FAILURE);
        }
    }

    // Close all pipe file descriptors in the parent
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipe_fds[i][0]);
        close(pipe_fds[i][1]);
    }

    // Wait for the last child process
    do {
        waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    return 1;
}

void shell_loop() {
    char input[MAX_INPUT_SIZE];

    while (1) {
        int background = 0;  // Initialize background here

        printf("shell> ");
        fgets(input, MAX_INPUT_SIZE, stdin);

        // Handle Ctrl+C (SIGINT) signal
        signal(SIGINT, sigint_handler);

        // Remove newline character
        input[strlen(input) - 1] = '\0';

        // Exit
        // Exit the shell if the user types 'exit'
        if (strcmp(input, "exit") == 0) {
            printf("Exiting shell...\n");
            break;
        } else if (strcmp(input, "history") == 0) {
            // Handle the 'history' command
            display_history();
        } else if (strcmp(input, "jobs") == 0) {
            // Handle the 'jobs' command to display background processes
            display_background_processes();
        } else {
            // Check if the command is meant to run in the background
            if (input[strlen(input) - 1] == '&') {
                background = 1;
                input[strlen(input) - 1] = '\0';  // Remove the '&' from the command
            }

            // Check if the command contains a pipe character '|'
            if (strchr(input, '|') != NULL) {
                // Launch the command with piping
                launch_with_pipes(input);
            } else {
                // Launch the command without piping
                add_to_history(input, -1, -1, -1.0);
                launch(input);  // Pass the 'background' flag to the 'launch' function
            }
        }
    }
}

int main() {
    // Enter the shell loop
struct sigaction sig;
memset(&sig, 0, sizeof(sig));
sig.sa_handler = sigint_handler;
sigaction(SIGINT, &sig, NULL);
if (sigaction(SIGINT, &sig, NULL) == -1) {
    perror("sigaction not handled properly");
    exit(EXIT_FAILURE);
}
sigaction(SIGCHLD, &sig, NULL);
if (sigaction(SIGCHLD, &sig, NULL) == -1) {
    perror("sigchild's sigaction not handled properly");
    exit(EXIT_FAILURE);
}
    shell_loop();
    shell_exit();
    return 0;
}

