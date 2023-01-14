#include "logging.h" 
#include <string.h>
#include <pthread.h>

int reg_pipe;
char register_pipe[PIPENAME];

void sig_handler(int sig){
  // UNSAFE: This handler uses non-async-signal-safe functions (printf(),
    if (sig == SIGINT) { //ctrl + c
        clear_session(reg_pipe, register_pipe);
        fprintf(stderr, "Caught SIGINT\n");
        return; // Resume execution at point of interruption
    }   
    return;
}

/*void* func_publisher(char **args){
    char* client_pipe, box_name; 
    strcpy(client_pipe,args[0]);
    strcpy(box_name,args[1]);
    //open(box)
}
*/
// implementar aqui a lista de boxes

int main(int argc, char **argv) {
    
    pthread_t slave1;
    
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        fprintf(stdout, "ERROR: %s\n", SIGNAL_FAIL);
        exit(EXIT_FAILURE);
    }

    if (argc != 3){
        fprintf(stdout, "ERROR: %s\n", INVALID_NUMBER_OF_ARGUMENTS);
        return -1;
    }

    int max_sessions = atoi(argv[2]);

    if (max_sessions<1){
        fprintf(stdout, "ERROR: %s\n", INVALID_SESSIONS);
        return -1;
    }

    //fprintf(stderr, "usage: mbroker <pipename>\n");


    
    printf("FALTA USAR %d SESSIONS\n", max_sessions);
       
    strcpy(register_pipe, argv[1]);

    unlink(register_pipe);
    
    if(mkfifo(register_pipe, 0644) < 0)
        exit(EXIT_FAILURE);
    
    if ((reg_pipe = open(register_pipe, O_RDONLY)) < 0){ //freezes at open until a writer joins the pipe
        exit(EXIT_FAILURE);
    }

    
    char* buffer = (char*) malloc(sizeof(char)*400);
    int code;
    char *client_pipe;
    char *box_name;
    char *args[2];

    ssize_t broker_read= read(reg_pipe, buffer, sizeof(char)*400);
    
    do {
        if(broker_read >0){
            printf("%s\n",buffer);
            code = atoi(strtok(buffer,"|"));
            client_pipe = strtok(NULL,"|");
            box_name = strtok(NULL,"|");
            strcpy(args[0], client_pipe);
            strcpy(args[1], box_name);
            printf("%s %s\n", args[0], args[1]);
            switch(code) {
                case 1:
                    printf("reg_pub(buffer\n");
                    /*if (find_box(box_name).hasWriter == 1){
                        int fd = open(client_pipe, O_RDONLY);  
                        if (fd == -1) {
                            fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
                        }
                        close(fd);                              // signals the publisher that his request failed
                        break;
                    }
                    if (pthread_create(&tid, NULL, func_publisher, args) != 0) {  //TO IMPLEMENT FIND BOX AND BOX RELATED FUNCTIONS!!!!!!!!
                        fprintf(stdout, "ERROR: %s\n", ERROR_CREATING_THREAD);
                        return -1;
                    }*/
                    //reg_publisher(box_name);
                    //atribui thread ao publisher
                    //a thread deve ir lendo do client pipe e imprimindo para o tfs
                    break;
                case 2:
                    printf("reg_sub(buffer\n");
                    //atribui thread ao subscriber
                    //a thread deve ir lendo do tfs e imprimindo para o pipe do client
                    break;
                case 3:
                    printf("box_create(buffer)\n");
                    //create_box(client_pipe, box_name);
                    break;
                case 4:
                    printf("resposta ao pedido de criacao\n");
                    break;
                case 5:
                    printf("box_remove(buffer\n");
                    break;
                case 6:
                    printf("resposta ao pedido de remocao\n");
                    break;
                case 7:
                    printf("box_list(buffe\n");
                    break;
                case 8:
                    printf("resposta ao pedido de listagem\n");
                    break;
                case 9:
                    printf("mensagens enviadas pelo publisher\n");
                    break;
                case 10:          
                    printf("mensagens enviadas pelo subscriber\n");
                    break;
                default:
                    printf("%d, %s, %s\n", code, client_pipe, box_name);
                    break;
            };
            buffer = realloc(buffer, sizeof(char)*400); //segmentation fault if more than 2 clients!! bcs of strtok
        }
        
        
    } while((broker_read = read(reg_pipe, buffer, MSIZE)) >= 0);
    
    //mbroker
    clear_session(reg_pipe, register_pipe);
    free(buffer);
    buffer = NULL;
    printf("Finished mbroker\n");
    return 0;
}




/*

int reg_publisher(request* req) {
    aceitar se box existir e estiver livre
    rejeitar se box ja tiver pub associado (fechar fifo)
}

int reg_sub(request* req) {
    aceitar se box existir e estiver livre
    rejeitar se box ja tiver pub associado (fechar fifo)
}*/

