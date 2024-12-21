#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void task(int signum) {
    printf("Scheduled task executed!\n");
    alarm(5);  // Reschedule the task after 5 seconds
}

int main() {
    signal(SIGALRM, task);  // Set up signal handler for SIGALRM
    alarm(5);  // Schedule the first task after 5 seconds

    while (1) {
    	printf("Hello wowrld");
        pause();  // Wait for signal
    }
    return 0;
}