#include <stdio.h>
#include <string.h>


int main(){
    char *a =  "Ola\0o meu nome\0 é jose";
    int s = sizeof(a);
    printf("%d", s);
    while(s > 0) {
        int len =strlen(a);
        printf("%d\n", len);
        printf("%s\n", a);
        a+= len+1;
        s-= len +1;
        //printf("%s\n", a++);

        //printf("hey:%ld\n", strlen(a));
    }

    

    /*
    char *p = a;
    char *plast;
    


    while( p != NULL ) {
    plast = p;
    p = strchr(p, '\0');

    if( p != NULL ) {
        // Found a \0.  Count characters and advance to next char.
        count = p - plast;
        p++;
    } else {
        // Found no semi-colon.  Count characters to the end of the string.
        count = pend - p;
    }

    printf( "Number of characters: %d\n", count );
}*/
}



/*
while (ch != '\n') {
do {
ch = getchar();
str[i++] = ch;

} while (ch != '\0')
}
    

*/