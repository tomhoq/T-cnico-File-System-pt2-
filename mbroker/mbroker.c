#include "logging.h"
#include "operations.h" 
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

void *func_main(){
    char* buffer = (char*) malloc(sizeof(char)*400);
    int code;
    ssize_t broker_read= read(reg_pipe, buffer, sizeof(char)*400);
    
    args clientInput;

    do {
        printf("bb\n");
        if(broker_read >0 ){
            printf("%s\n",buffer);
            sscanf(buffer, "%d %s %s", &code, clientInput._client_pipe, clientInput._box_name);
            printf("%s %s\n", clientInput._client_pipe, clientInput._box_name);
            switch(code) {
                case 1:
                    printf("reg_pub(buffer\n");
                    //reg_publisher(clientInput);
                    //atribui thread ao publisher
                    //a thread deve ir lendo do client pipe e imprimindo para o tfs
                    break;
                case 2:
                    printf("reg_sub(buffer\n");
                    //reg_subscriber(clientInput);
                    //atribui thread ao subscriber
                    //a thread deve ir lendo do tfs e imprimindo para o pipe do client
                    break;
                case 3:
                    printf("box_create(buffer)\n");
                    //create_box(clientInput);
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
                    printf("default\n");
                    break;
            };

        }
        
        printf("aaaaa\n");
    } while((broker_read = read(reg_pipe, buffer, MSIZE)) >= 0);
    free(buffer);
    buffer = NULL;
    return NULL;
}

int main(int argc, char **argv) {
    tfs_init(NULL);
    pthread_t main_thread;
    
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

    
    if (pthread_create(&main_thread, NULL, func_main, NULL)!= 0){
        fprintf(stdout, "ERROR: %s\n", ERROR_CREATING_THREAD);
        return -1;
    }
    if(pthread_join(main_thread, NULL) != 0) {
        fprintf(stdout, "ERROR: Couldn't join thread\n");
        return -1;
    }
    
    //mbroker
    clear_session(reg_pipe, register_pipe);
    printf("Finished mbroker\n");
    return 0;
}
/*
void create_box(args clientInput){
    if(tfs_open(clientInput._box_name, TFS_O_CREAT)){//gestor deve dar errro ao criar caixa ja sxistente
    }
    box b;
    strcpy(b.box_name, clientInput._box_name);
    //insertBox(b);

}

void reg_publisher(args clientInput) {
    //aceitar se box existir e estiver livre
    //rejeitar se box ja tiver pub associado (fechar fifo)
    //box boxToWrite = find_box(clientInput._box_name);
    if (boxToWrite.hasWriter != 0){                //TO IMPLEMENT FIND BOX AND BOX RELATED FUNCTIONS!!!!!!!!
        int fd = open(clientInput._client_pipe, O_RDONLY);  
        if (fd == -1) {
            fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        }
        close(fd);                              // signals the publisher that his request failed
    }
    char* buffer = (char*) malloc(sizeof(char)*400);
    int fh = tfs_open(boxToWrite.box_name, TFS_O_APPEND);  //manager already created file

    while((read(fd, buffer, MSIZE)) > 0){  //detects if publisher closed the pipe
        //tfs write should detect if file is deleted
        if(tfs_write(fh, buffer, strlen(buffer)+1)==-1)
            break;

    }
    
    free(buffer);
    buffer = NULL;
}

//find box devolve uma caixa da lista 
//se nao existir na lista devolve uma caixa criada cujo argumento has_Writers deve ser inicializado a -1.
box find_box();

void reg_subscriber(args clientInput) {
    box boxToWrite = find_box(clientInput._box_name);
    if (boxToWrite.hasWriter == -1){             // box doesnt exist
        int fd = open(clientInput._client_pipe, O_RDONLY);  
        if (fd == -1) {
            fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        }
        close(fd);                              // signals the publisher that his request failed
        return NULL;
    }
    char* buffer = (char*) malloc(sizeof(char)*400);
    int fh = tfs_open(boxToWrite.box_name, TFS_O_APPEND);  //manager already created file

    //tfs read should detect if file is deleted
    while(tfs_read(fh, buffer, strlen(buffer)+1)!=-1){  
        if(write(fh, buffer, strlen(buffer)+1)==-1)//detects if publisher closed the pipe
            break;
    }
    
    free(buffer);
    buffer = NULL;
    return NULL;
}
*/
