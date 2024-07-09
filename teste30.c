#include <stdio.h>
#include <unistd.h>

int main() {
    printf("%s %d\n", "child pid -", getpid());
    printf("Sleeping for 30 seconds...\n");
    sleep(30);
    // printf("\n");
    exit(0);
}
