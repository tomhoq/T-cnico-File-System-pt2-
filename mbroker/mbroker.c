#include "logging.h"
#include "operations.h" 
#include <string.h>
#include <pthread.h>

void reg_publisher(args clientInput);
void reg_subscriber(args clientInput);
void create_box(args clientInput);

int reg_pipe;
char register_pipe[PIPENAME];

box *head;

void sig_handler(int sig){
  // UNSAFE: This handler uses non-async-signal-safe functions (printf(),
    if (sig == SIGINT) { //ctrl + c
        clear_session(reg_pipe, register_pipe);
        fprintf(stderr, "Caught SIGINT\n");
        return; // Resume execution at point of interruption
    }   
    return;
}


void *func_main(){
    char* buffer = (char*) malloc(sizeof(char)*400);
    int code;
    ssize_t broker_read= read(reg_pipe, buffer, sizeof(char)*400);
    
    args clientInput;

    do {
        if(broker_read >0 ){
            printf("%s\n",buffer);
            sscanf(buffer, "%d %s %s", &code, clientInput._client_pipe, clientInput._box_name);
            printf("%s %s\n", clientInput._client_pipe, clientInput._box_name);
            switch(code) {
                case 1:
                    printf("reg_pub(buffer)\n");
                    reg_publisher(clientInput);
                    //atribui thread ao publisher
                    //a thread deve ir lendo do client pipe e imprimindo para o tfs
                    break;
                case 2:
                    printf("reg_sub(buffer\n");
                    reg_subscriber(clientInput);
                    //atribui thread ao subscriber
                    //a thread deve ir lendo do tfs e imprimindo para o pipe do client
                    break;
                case 3:
                    printf("box_create(buffer)\n");
                    create_box(clientInput);
                    break;
                case 5:
                    printf("box_remove(buffer\n");
                    break;
                case 7:
                    printf("box_list(buffe\n");
                    break;
                default:
                    printf("default\n");
                    break;
            };

        }
        
    } while((broker_read = read(reg_pipe, buffer, sizeof(char)*400)) >= 0);
    free(buffer);
    buffer = NULL;
    return NULL;
}

int main(int argc, char **argv) {
    if(tfs_init(NULL)==-1)
        return -1;
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
    //apagar lista de boxes
    clear_session(reg_pipe, register_pipe);
    printf("Finished mbroker\n");
    return 0;
}

void create_box(args clientInput){
    printf("enter create\n");
    int fd = open(clientInput._client_pipe, O_WRONLY);
    if (fd == -1) {
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        return;
    }
    int32_t return_code = 0;
    char *error_msg = malloc(sizeof(char)*(ERROR_MSG+100));
    memset(error_msg,'\0',sizeof(char)*(ERROR_MSG+100));
    box *boxToCreate = find_box(clientInput._box_name, head);
    int fh;
    if((fh=tfs_open(clientInput._box_name, TFS_O_APPEND)) !=-1 || boxToCreate->hasWriter != -1){//gestor deve dar erro ao tentar criar caixa ja existente
        return_code = -1;
        strcpy(error_msg, "Can't create box, already exists");
        free(boxToCreate);
        if (tfs_close(fh) == -1)
            fprintf(stdout,"ERROR %s\n", "Failed to close file");
    }
    else{
        printf("creating file:%d\n", fh);
        if((fh = tfs_open(clientInput._box_name, TFS_O_CREAT))==-1){ //file musnt exist
            fprintf(stdout, "ERROR: %s\n", "Failed to open file");
            free(error_msg);
            return;
        }
        printf("creating bx\n");
        boxToCreate->hasWriter = 0;
        boxToCreate->n_readers = 0;
        strcpy(boxToCreate->box_name, clientInput._box_name);
        head = insert_box(boxToCreate, head);
    }
    char *msg = serializeAnswer(ANSWER_CREATE_BOX, return_code, error_msg);
    send_request(fd, msg);

    if (tfs_close(fh) == -1){
        fprintf(stdout,"ERROR %s\n", "Failed to close file");
    }

    free(error_msg);
}

void reg_publisher(args clientInput) {
    printf("entered publisher\n");
    int fd = open(clientInput._client_pipe, O_RDONLY);
    if (fd == -1) {
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        return;
    }
    box* boxToWrite = find_box(clientInput._box_name, head);
    printf("Box has writer? %d\n", (boxToWrite->hasWriter));
    if (boxToWrite->hasWriter != 0){        //box doesnt exist or doesnt have writers or full writers
        printf("cant create box, finishing\n");
        clear_session(fd, clientInput._client_pipe);   // signals the publisher that his request failed
        free(boxToWrite);
        return;
    }
    int fh;
    printf("opening\n");
    if((fh = tfs_open(boxToWrite->box_name, TFS_O_APPEND))==-1){ //file doesnt exist
        free(boxToWrite);
        close(fd);
        return;
    }
    char* buffer = (char*) malloc(sizeof(char)*(MSIZE+100));
    char message[MSIZE];
    memset(message, '\0', MSIZE);
    int code;
    ssize_t b;
    printf("start reading\n");
    while((read(fd, buffer, MSIZE)) > 0){  //detects if publisher closed the pipe
        //tfs write should detect if file is deleted
        printf("BUFF:%s\n", buffer);
        sscanf(buffer, "%d %[^\n]%*c", &code, message);
        printf("MESS:%s", message);
        strcat(message," ");
        if((b=tfs_write(fh, message, strlen(message))+1)<0)
            break;
        printf("%ld\n",b);
        memset(buffer, '\0', sizeof(char)*(MSIZE+100));

    }
    if (tfs_close(fh) == -1){
        fprintf(stdout,"ERROR %s\n", "Failed to close file");
    }
    printf("finished reading");
    close(fd); 
    free(buffer);
    buffer = NULL;
}


void reg_subscriber(args clientInput) {
    printf("entered subscriber\n");
    int fd = open(clientInput._client_pipe, O_WRONLY);
    if (fd == -1) {
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        return;
    }
    box *boxToRead = find_box(clientInput._box_name, head);
    printf("Box exists? %d\n", (boxToRead->hasWriter));
    if (boxToRead->hasWriter == -1){             // box doesnt exist  
        close(fd);                              // signals the publisher that his request failed
        free(boxToRead);
        return;
    }
    
    int fh = tfs_open(boxToRead->box_name, TFS_O_CREAT);  //manager already created file
    if (fh == -1){
        close(fd);
    }
    char *buffer = (char*) malloc(sizeof(char)*(MSIZE+100));
    char *msg;
    //tfs read should detect if file is deleted
    size_t len;
    ssize_t b;
        
    while((b = tfs_read(fh, buffer, MSIZE))!=-1){   //ESTÁ A FICAR PRESO AQUI. o unlink exterior que se 
                                                    //dá no sub.c não funciona dentro do TFS
        sleep(1);
        if (strcmp(buffer,"\0")){
            while(b>0){
                len = strlen(buffer);
                printf("read from tfs: %s\n", buffer);
                msg = serializeMessage(SERVER_SEND, buffer);
                printf("ESTA É A MENS. QUE SERÁ ENVIADA:%s\n", msg);
                if((write(fd, msg, sizeof(msg))) < 0){//detects if subscriber closed the pipe
                    printf("subs has left the chat\n");
                    break;
                }
                memset(buffer, '\0', sizeof(char)*(MSIZE+100));
                free(msg);
                buffer += len +1;
                b -= (ssize_t) len +1;
            }
        }
        if(write(fd, " ", 1) < 0){//detects if subscriber closed the pipe {
            printf("subs has left the chat\n");
            break;
          }
        
    }

    

    printf("i am leaving\n");
    if (tfs_close(fh) == -1){
        fprintf(stdout,"ERROR %s\n", "Failed to close file");
    }
    printf("leaving subscriber\n");
    close(fd);
    free(buffer);
    buffer = NULL;
    return;
}

