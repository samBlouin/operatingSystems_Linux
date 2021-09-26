#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char etext, edata, end;

int main(int argv, char *argc[]){
    char *buf = malloc(1024*1024*4);
    printf("   etext %p\n", &etext);
    printf("   edata %p\n", &edata);
    printf("   end   %p\n", &end);
    printf("   sbrk  %p\n", sbrk(0));
    pause();
    return 42;
}
