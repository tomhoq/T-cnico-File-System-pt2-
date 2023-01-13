#include "logging.h" 
#include <string.h>

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

// implementar aqui a lista de boxes

int main(int argc, char **argv) {

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

    char client_pipe[PIPENAME];
    char box_name[BOXNAME];
    request* buffer = (request*) malloc(sizeof(request));

    ssize_t broker_read= read(reg_pipe, buffer, sizeof(request));
    
    do {
        printf("received %zdb at %p\n", broker_read, buffer);
        if(broker_read >0){
            strcpy(client_pipe, buffer->_client_pipe);
            strcpy(box_name, buffer->_box_name);
            switch(buffer->_code) {
                case 1:
                    printf("reg_pub(buffer\n");
                    //reg_publisher(box_name);
                    break;
                case 2:
                    printf("reg_sub(buffer\n");
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
                    printf("%d, %s, %s\n", buffer->_code, buffer->_client_pipe, buffer->_box_name);
                    break;
            }
        }
    } while((broker_read = read(reg_pipe, buffer, MSIZE)) >= 0);
    printf("%zd\n", broker_read);
    
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

