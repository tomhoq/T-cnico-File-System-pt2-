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
    char box_name[BOXNAME+1];
    memset(register_pipe, '\0', sizeof(char)*PIPENAME);
    memset(personal_pipe, '\0', sizeof(char)*PIPENAME);
    memset(box_name, '\0', sizeof(char)*(BOXNAME+1));
    memset(command, '\0', sizeof(char)*COMMAND);

    box_name[0] = '/';
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
        strcat(box_name ,argv[4]);
        printf("%s\n", box_name);
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
    int code, return_code;
    char *buffer = malloc(sizeof(char)*1300);
    char input [MSIZE], error_msg[ERROR_MSG];
    if(read(fd, buffer, MSIZE) <= 0){
        fprintf(stdout, "ERROR: %s\n", "Failed to read");
    }
    sscanf(buffer, "%d %s", &code, input);
    printf("%d\n",code);
    //int last;
    //char name[BOXNAME];
    //__uint64_t size, p, s;

    if (code != 8){
        sscanf(input, "%d %s", &return_code, error_msg);
        printf("%d %s\n",return_code, error_msg);
        if(return_code==-1)
            fprintf(stdout, "ERROR %s\n", error_msg);
        else
            fprintf(stdout, "OK\n");
    }
    else{
        //while(last != 1){
            //sscanf(buffer, "%d %s %lu %lu %lu", &last, name, &size &p &s, input);
        //}
        //fprintf(stdout, "%s %zu %zu %zu\n", box_name, box_size, n_publishers, n_subscribers);
    }
    
    free(buffer);
    clear_session(fd, personal_pipe);
    printf("%s %s %s %s\n", register_pipe, personal_pipe, command, box_name);

    return 0;
}

