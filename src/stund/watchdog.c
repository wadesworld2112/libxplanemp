#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
        if(argc < 2) {
                printf("Usage: %s <program and arguments>\nExample: %s 'server -h 1.2.3.4 -v'\n", argv[0], argv[0]);
                return -1;
        }

        signal(SIGHUP, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGINT, SIG_IGN);
        signal(SIGPIPE, SIG_IGN);

        daemon(1, 0); // nochdir, close files

        while(1 == 1) {
                system(argv[1]);
                sleep(10);
        }
        return 0;
}

