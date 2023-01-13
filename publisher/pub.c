#include <signal.h>
#include "logging.h"
#include <string.h>
#include <unistd.h>


void sig_handler(int sig){
  // UNSAFE: This handler uses non-async-signal-safe functions (printf())
    if (sig == SIGQUIT) { //ctrl + d
        fprintf(stderr, "Caught SIGINT\n"); 
        return; // Resume execution at point of interruption
    }   // Must be SIGQUIT - print a message and terminate the process //ctrl + d
    return;
}

int main(int argc, char **argv) {
    
    if (signal(SIGQUIT, sig_handler) == SIG_ERR) {
        fprintf(stdout, "ERROR: %s\n", SIGNAL_FAIL);
        exit(EXIT_FAILURE);
    }

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
    if ((strlen(argv[3])>=BOXNAME)){
        fprintf(stdout, "ERROR: %s\n", INVALID_BOXNAME);
        return -1;
    }

    strcpy(register_pipe, argv[1]);
    strcpy(personal_pipe, argv[2]);
    strcpy(box_name, argv[3]);


//REGISTER PIPE-------------------------------------------------------
    
    //verificar que register pipe existe!!
    
    int reg_pipe = open(register_pipe, O_WRONLY);
    if (reg_pipe == -1) {
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        return -1;
    }

//PERSONAL PIPE-------------------------------------------------------

    //Verificar que personal pipe ainda n existe
    if(mkfifo(personal_pipe, 0644) < 0){
        fprintf(stdout, "ERROR: %s\n", EXISTENT_PIPE);
        return -1;
    }

//COMMUNICATION WITH MBROKER-------------------------------------------------

    send_request(reg_pipe, serialize(REGISTER_PUBLISHER, personal_pipe, box_name));

    int fd = open(personal_pipe, O_WRONLY);   //tries to open pipe after mbroker received the request
    if (fd == -1) {                           //program will freeze here while thread is not implemented
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        clear_session(fd, personal_pipe);
        return -1;
    }

    while(!feof(stdin)){ // Loop forever, waiting for EOF NOT WORKING YET
        if(write(fd,"a",2)<0)
            break;
    }
    printf("%s %s %s\n", register_pipe, personal_pipe, box_name);
    clear_session(fd, personal_pipe);
    return 0;
}
