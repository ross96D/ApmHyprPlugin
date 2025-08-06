#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

const char* fifo_file_path = "/tmp/__testMyPlugin";

int         main(int argc, char const* argv[]) {

    int  fd = open(fifo_file_path, O_RDONLY | O_NONBLOCK);

    char buffer[500];
    int  n = 0;
    while (1) {
        if (fd == 0) {
            // fd = open(fifo_file_path, O_RDONLY | O_NONBLOCK);
        }
        n = read(fd, &buffer, 500);
        if (n == -1) {
            continue;
        }
        if (n == 0) {
            // close(fd);
            // fd = 0;
            continue;
        }
        buffer[n] = 0;
        printf("%s", buffer);
    }

    return 0;
}
