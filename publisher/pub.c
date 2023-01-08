#include "logging.h"
#include <string.h>
#include <unistd.h>


int main(int argc, char **argv) {

    if (argc != 4){
        fprintf(stdout, "ERROR: %s\n", INVALID_NUMBER_OF_ARGUMENTS);
        return -1;
    }
    //fprintf(stderr, "usage: pub <register_pipe_name> <box_name>\n");

    char register_pipe[PIPENAME]; 
    char personal_pipe[PIPENAME];
    char box_name[BOXNAME];
    memset(register_pipe, '\0', sizeof(char)*PIPENAME);
    memset(personal_pipe, '\0', sizeof(char)*PIPENAME);
    memset(box_name, '\0', sizeof(char)*BOXNAME);
    
    if ((strlen(argv[1])>=PIPENAME) || strlen(argv[2])>=PIPENAME){
        fprintf(stdout, "ERROR: %s\n", PIPENAME_TOO_BIG);
        return -1;
    }
    strcpy(register_pipe, argv[1]);
    
    //verificar que register pipe existe!!
    /*
    int reg_pipe = open(register_pipe, O_WRONLY);
    if (reg_pipe == -1) {
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        return -1;
    }
    */
    
    strcpy(personal_pipe, argv[2]);
    /*
    int prs_pipe = open(personal_pipe, O_WRONLY);
    //Verificar que personal pipe ainda n existe
    if (prs_pipe != -1) {
        fprintf(stdout, "ERROR: %s\n", EXISTENT_PIPE);
        return -1;
    }  
    */

    if ((strlen(argv[3])>=PIPENAME)){
        fprintf(stdout, "ERROR: %s\n", BOXNAME_TOO_BIG);
        return -1;
    }

    strcpy(box_name, argv[3]);

    //enviar pedido de registo ao mbroker
    char *ser = serialize(REGISTER_PUBLISHER, personal_pipe, box_name);
    printf("%s\n",ser);
    //send_msg(reg_pipe, serialize(REGISTER_PUBLISHER, personal_pipe, box_name));

    free(ser);
    printf("%s %s %s\n", register_pipe, personal_pipe, box_name);
    return 0;
}

