#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ITERS 1000 // number of iterations per process

int main(void) {
    int pid1 = fork();

    if (pid1 == 0) {
        // Child 1: High priority
        set_priority(1000);
        // int prio = get_priority();

        // printf("Child 1 (PID=%d) priority set to %d\n", getpid(), prio);

        for (int i = 0; i < ITERS; i++) {
            printf("1");
            for(int j=0; j<2e8; j++);
            // sleep(1);  // pause to reduce interleaving
        }
        exit(0);
    }

    int pid2 = fork();
    if (pid2 == 0) {
        // Child 2: Low priority
        set_priority(100);
        // int prio = get_priority();
        // for(int i=1; i<=1e8; i++);
        // printf("Child 2 (PID=%d) priority set to %d\n", getpid(), prio);

        for (int i = 0; i < ITERS; i++) {
            printf("2");
            for(int j=0; j<2e8; j++);
            // sleep(1);  // pause to reduce interleaving
        }
        exit(0);
    }

    // Parent waits for both children
    // wait(0);
    // wait(0);

    // printf("Parent: both children finished. Test complete.\n");
    exit(0);
}
