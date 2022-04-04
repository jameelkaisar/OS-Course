#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CMD_BUFFER 100
#define MAX_CMD_ARGS 25
#define MAX_ARG_LEN 50

// Handle Crtl + C in Shell
// Configure Arrow Keys

void new_command() {
    printf("$ ");
}

void cmd_help() {
    printf("Command     Description\n");
    printf("=======     ===========\n");
    printf("help        Display help menu\n");
    printf("pid         Display process id of current process\n");
    printf("ppid        Display process id of parent of current process\n");
    printf("exit        Exit shell\n");
    printf("\n");
    printf("Other shell commands are also supported!\n");
}

void cmd_pid() {
    printf("Process ID: %d\n", getpid());
}

void cmd_ppid() {
    printf("Parent Process ID: %d\n", getppid());
}

int get_command(char cmd[], char command[]) {
    int quote = 0;
    int i = 0;
    int j = 0;
    int len = strlen(cmd);
    // printf("Length: %d\n", len);
    for (i = 0; i != len; ++i) {
        if (cmd[i] == '\n') {
            break;
        }
        else if (cmd[i] == '\\') {
            command[j] = cmd[i + 1];
            ++j;
            ++i;
        } else if (cmd[i] == '\"') {
            quote = !quote;
        } else if (cmd[i] == ' ' && !quote) {
            break;
        } else {
            command[j] = cmd[i];
            ++j;
        }
    }
    command[j] = '\0';
    // printf("Command: %s (%lu)\n", command, strlen(command));
    return i;
}

// int get_args_list(char cmd[], char* args_list[]) {
//     int index = 0;
//     int count = 0;
//     int i = 0;
//     int len = strlen(cmd);
//     while (1) {
//         for (; i != len; ++i) {
//             if (cmd[i] == '\\') {
//                 args_list[index][i] = cmd[i];
//                 args_list[index][i + 1] = cmd[i + 1];
//                 ++i;
//             } else if (cmd[i] == '\"') {
//                 args_list[index][i] = cmd[i];
//                 ++count;
//             } else if (cmd[i] == ' ' && count == 0) {
//                 break;
//             } else {
//                 args_list[index][i] = cmd[i];
//             }
//         }
//         printf("%s\n", args_list[index]);
//         args_list[index][i] = '\0';
//         ++index;
//         if (i == len) {
//             break;
//         }
//     }
//     args_list[index] = NULL;
//     return args_list;
// }

void handle_exec(char cmd[]) {
    int f = fork();
    if (f == 0) {
        char command[MAX_CMD_BUFFER];
        // char args_list[MAX_CMD_ARGS + 1][MAX_ARG_LEN] = {""};
        int command_index = get_command(cmd, command);
        // strcpy(args_list[0], command);
        // int total_args = get_args_list(cmd, args_list);
        // int total_args = 1;
        // strcpy(args_list[total_args], (char*)NULL);

        // int status_code = execlp(gc, cmd, (char*)NULL);
        int status_code = execlp(command, "ls", "-a", (char*)NULL);
        // int status_code = execlp(command, cmd, (char*)NULL);

        // int status_code = execvp(command, args_list);
        if (status_code == -1) {
            printf("[EXCEPTION] Process did not terminate correctly!\n");
            exit(EXIT_FAILURE);
        }
    }
    wait(NULL);
}

int handle_command(char cmd[]) {
    if (!strcmp("exit\n", cmd)) {
        return 0;
    } else if (!strcmp("help\n", cmd)) {
        cmd_help();
    } else if (!strcmp("pid\n", cmd)) {
        cmd_pid();
    } else if (!strcmp("ppid\n", cmd)) {
        cmd_ppid();
    } else {
        handle_exec(cmd);
    }
    return 1;
}

int main() {
    char cmd[MAX_CMD_BUFFER];

    do {
        new_command();
        fgets(cmd, MAX_CMD_BUFFER, stdin);
    } while (handle_command(cmd));

    return 0;
}
