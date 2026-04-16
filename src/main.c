#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdio.h>

void main(void) {
    DISPLAY_ON;
    printf("Hello, GBDK!\n");
    printf("GitHub Actions build OK\n");
    printf("Press START...\n");
    waitpad(J_START);
}
