#include "logging.h"
#include <string.h>

static void print_usage() {
    fprintf(stderr, "usage: \n"
                    "   manager <register_pipe_name> create <box_name>\n"
                    "   manager <register_pipe_name> remove <box_name>\n"
                    "   manager <register_pipe_name> list\n");
}

int main(int argc, char **argv) {
    if (argc > 5 || argc < 4){
        fprintf(stdout, "ERROR: %s\n", INVALID_NUMBER_OF_ARGUMENTS);
        print_usage();
        return -1;
    }

    char register_pipe[PIPENAME];               
    char personal_pipe[PIPENAME];
    char command[COMMAND];
    char box_name[BOXNAME];
    memset(register_pipe, '\0', sizeof(char)*PIPENAME);
    memset(personal_pipe, '\0', sizeof(char)*PIPENAME);
    memset(box_name, '\0', sizeof(char)*BOXNAME);
    memset(command, '\0', sizeof(char)*COMMAND);

    if (strlen(argv[1]) >= PIPENAME || strlen(argv[2]) >= PIPENAME){
        fprintf(stdout, "ERROR: %s\n", PIPENAME_TOO_BIG);
        return -1;
    }
    if (strcmp(argv[3], "create")&&strcmp(argv[3], "remove")&&strcmp(command, "list")) {
        fprintf(stdout, "ERROR: %s\n", INVALID_ARGUMENTS);
        return -1;
    }
    if (argc == 5){
        if ((strlen(argv[4])>=BOXNAME)){
            fprintf(stdout, "ERROR: %s\n", INVALID_BOXNAME);
            return -1;
        }
        strcpy(box_name, argv[4]);
    }

    strcpy(register_pipe, argv[1]);
    strcpy(personal_pipe, argv[2]);
    strcpy(command, argv[3]);
    

//REGISTER PIPE---------------------------------------------------------------
    //verificar que register pipe existe!!
    int reg_pipe = open(register_pipe, O_WRONLY);
    if (reg_pipe == -1) {
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        return -1;
    }

//PERSONAL PIPE---------------------------------------------------------------
    //Verificar que personal pipe ainda n existe
    if(mkfifo(personal_pipe, 0644) < 0){
        fprintf(stdout, "ERROR: %s\n", EXISTENT_PIPE);
        return -1;
    }

//COMMAND-------------------------------------------------------------------
    char *str;
    if (!strcmp(command, "create")) { 
        str = serialize(CREATE_BOX, personal_pipe, box_name);
    }
    else if(!strcmp(command, "remove")){ 
        str = serialize(REMOVE_BOX, personal_pipe, box_name);
    } else{ //list case
        str = serialize(LIST_BOX, personal_pipe, "");
    } 

    send_request(reg_pipe, str);

    int fd = open(personal_pipe, O_RDONLY);   //tries to open personal pipe after mbroker received the request
    if (fd == -1) {
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        return -1;
    }
    
    clear_session(fd, personal_pipe);
    printf("%s %s %s %s\n", register_pipe, personal_pipe, command, box_name);

    return 0;
}

