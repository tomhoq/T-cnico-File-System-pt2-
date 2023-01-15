#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "logging.h"
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

bool shouldRun = true;

void sighandler() {
    shouldRun = false;
}

int main(int argc, char **argv) {

    if (signal(SIGINT, &sighandler) == SIG_ERR) {
        fprintf(stderr, "Could not set signal handler\n");
        return EXIT_FAILURE;
    }

    if (argc != 4){
        fprintf(stdout, "ERROR: %s\n", INVALID_NUMBER_OF_ARGUMENTS);
        return -1;
    }
    //fprintf(stderr, "usage: pub <register_pipe_name> <box_name>\n");

    char register_pipe[PIPENAME]; 
    char personal_pipe[PIPENAME];
    char box_name[BOXNAME+1];
    char input[MSIZE];
    memset(register_pipe, '\0', sizeof(char)*PIPENAME);
    memset(personal_pipe, '\0', sizeof(char)*PIPENAME);
    memset(box_name, '\0', sizeof(char)*(BOXNAME+1));
    
    box_name[0] = '/';
    if ((strlen(argv[1])>=PIPENAME) || strlen(argv[2])>=PIPENAME){
        fprintf(stdout, "ERROR: %s\n", PIPENAME_TOO_BIG);
        return -1;
    }
    if ((strlen(argv[3])>=BOXNAME)){
        fprintf(stdout, "ERROR: %s\n", INVALID_BOXNAME);
        return -1;
    }
    strcpy(register_pipe, argv[1]);
    strcpy(personal_pipe, argv[2]);
    strcat(box_name, argv[3]);

//REGISTER PIPE-------------------------------------------------------
    
    //verificar que register pipe existe!!
    int reg_pipe = open(register_pipe, O_WRONLY);
    if (reg_pipe == -1) {
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        return -1;
    }

//PERSONAL PIPE-------------------------------------------------------
    //Verificar que personal pipe ainda n existe
    if(mkfifo(personal_pipe, S_IRWXU) < 0){     //0644
        fprintf(stdout, "ERROR: %s\n", EXISTENT_PIPE);
        return -1;
    }

//COMMUNICATION WITH MBROKER-------------------------------------------------
    char *str  = serialize(REGISTER_PUBLISHER, personal_pipe, box_name);
    send_request(reg_pipe, str);
    free(str);
    int fd = open(personal_pipe, O_WRONLY);   //tries to open pipe after mbroker received the request
    if (fd == -1) {                           //program will freeze here if thread is not implemented
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        clear_session(fd, personal_pipe);
        return -1;
    }
    
    char *msg;
    ssize_t state;
    sleep(1);
    //confirmação
    if ((state = write(fd, " ", 1)) < 0) {
        return -1;
    }
    while (fgets(input, MSIZE, stdin) != NULL){
        //printf("%s\n", input);
        msg = serializeMessage(SEND_CODE, input);
        if (write(fd, msg, strlen(msg)+1) != strlen(msg)+1) {
            free(msg);
            break;
        }
        //printf("TT:%s\n", input);
        free(msg);
    }

    //printf("%s %s %s\n", register_pipe, personal_pipe, box_name);
    clear_session(fd, personal_pipe);
    return 0;
}


