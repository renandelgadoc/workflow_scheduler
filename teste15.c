#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Sleeping for 15 seconds...\n");
    sleep(15);
    printf("Awake now!\n");
    return 0;
}
