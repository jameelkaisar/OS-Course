#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    // create
    int fd = creat("test.txt", 0642);
    if (fd == -1) {
        printf("Cannot create file!\n");
    }
    else {
        printf("File created sucessfully\n");
        // close
        if (close(fd) == -1)
            printf("Cannot close file!\n");
        else
            printf("File closed sucessfully\n");
    }

    return 0;
}
