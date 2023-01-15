#include <stdio.h>
#include <string.h>


int main(int argc, char argv[]) {

    char str[64];
    int j = 0;

    strcpy(str, "OLA\0ADEUS\0IMAGINA\p");

    while (str[j] != '\p') {
        for (int i = 0; i < strlen(str); i++) {
            printf("eu:%c", str[i]);
            j++;
        }
        printf("\n");
    }

    printf("fim\n");
}