#include "logging.h"
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 4){
        fprintf(stdout, "ERROR: %s\n", INVALID_NUMBER_OF_ARGUMENTS);
        return -1;
    }
    //fprintf(stderr, "usage: pub <register_pipe_name> <box_name>\n");

    char register_pipe[PIPENAME]; 
    char personal_pipe[PIPENAME];
    char box_name[BOXNAME];
    
    if ((strlen(argv[1])>=PIPENAME) || strlen(argv[2])>=PIPENAME){
        fprintf(stdout, "ERROR: %s\n", PIPENAME_TOO_BIG);
        return -1;
    }
    //verificar que register pipe existe!!

    //verificar que box ja existe!!!
    
    strcpy(register_pipe, argv[1]);
    
    strcpy(personal_pipe, argv[2]);
    
    if ((strlen(argv[3])>=PIPENAME)){
        fprintf(stdout, "ERROR: %s\n", BOXNAME_TOO_BIG);
        return -1;
    }

    strcpy(box_name, argv[3]);
    
    printf("%s %s %s\n", register_pipe, personal_pipe, box_name);
    return 0;
}
