#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Sleeping for 30 seconds...\n");
    sleep(30);
    printf("Awake now!\n");
    return 0;
}
