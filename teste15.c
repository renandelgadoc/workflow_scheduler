#include <stdio.h>
#include <unistd.h>

int main() {
    printf("%s %d\n", "child pid -", getpid());
    printf("Sleeping for 15 seconds...\n");
    sleep(15);
    // printf("\n");
    exit(0);
}
