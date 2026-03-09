#include <stdio.h>
#include <stdlib.h>

int a = 10;
int b = 20;

int c;
int d;

int main(int argc, char *argv[]) {
    int e = 30;
    int f = 40;
    int *g = malloc(sizeof(int));
    int *h = malloc(sizeof(int));

    printf("a: %p\n", (void*) &a);
    printf("b: %p\n", (void*) &b);
    printf("c: %p\n", (void*) &c);
    printf("d: %p\n", (void*) &d);
    printf("e: %p\n", (void*) &e);
    printf("f: %p\n", (void*) &f);
    printf("g: %p\n", (void*) g);
    printf("h: %p\n", (void*) h);
}