#include <signal.h>
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
    char box_name[BOXNAME+1];
    char input[MSIZE];
    memset(register_pipe, '\0', sizeof(char)*PIPENAME);
    memset(personal_pipe, '\0', sizeof(char)*PIPENAME);
    memset(box_name, '\0', sizeof(char)*(BOXNAME+1));
    
    box_name[0] = '/';
    printf("a\n");
    if ((strlen(argv[1])>=PIPENAME) || strlen(argv[2])>=PIPENAME){
        fprintf(stdout, "ERROR: %s\n", PIPENAME_TOO_BIG);
        return -1;
    }
    if ((strlen(argv[3])>=BOXNAME)){
        fprintf(stdout, "ERROR: %s\n", INVALID_BOXNAME);
        return -1;
    }
    printf("A\n");
    strcpy(register_pipe, argv[1]);
    strcpy(personal_pipe, argv[2]);
    strcat(box_name, argv[3]);

//REGISTER PIPE-------------------------------------------------------
    
    //verificar que register pipe existe!!
    printf("YOUWONYGETWAHT\n");
    int reg_pipe = open(register_pipe, O_WRONLY);
    if (reg_pipe == -1) {
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        return -1;
    }

//PERSONAL PIPE-------------------------------------------------------
    printf("B\n");
    //Verificar que personal pipe ainda n existe
    if(mkfifo(personal_pipe, 0644) < 0){
        fprintf(stdout, "ERROR: %s\n", EXISTENT_PIPE);
        return -1;
    }

//COMMUNICATION WITH MBROKER-------------------------------------------------
    char *str  = serialize(REGISTER_PUBLISHER, personal_pipe, box_name);
    send_request(reg_pipe, str);
    free(str);
    printf("b\n");
    int fd = open(personal_pipe, O_WRONLY);   //tries to open pipe after mbroker received the request
    if (fd == -1) {                           //program will freeze here if thread is not implemented
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        clear_session(fd, personal_pipe);
        return -1;
    }
    
    char *msg;
    printf("C\n");
    sleep(1);
    if (write(fd, "\0", 1) < 0) {
        printf("aaaaa\n");
        clear_session(fd, personal_pipe);
        return 0;
    }
    printf("BELEM\n");
    while(scanf("%[^\n]%*c", input) == 1){
        strcat(input, "\0");
        msg = serializeMessage(SEND_CODE, input);
        printf("a\n");
        if (write(fd, msg, strlen(msg)+1) < 0) {
            printf("aaaaa\n");
            break;
        }

        printf("TT:%s\n", input);
        free(msg);
    }

    printf("%s %s %s\n", register_pipe, personal_pipe, box_name);
    clear_session(fd, personal_pipe);
    return 0;
}


