#include "logging.h"
#include "operations.h" 
#include <string.h>
#include <pthread.h>

//temos que comecar a limpar cenas, tirar printfs e assim!!!!

void reg_publisher(args clientInput);
void reg_subscriber(args clientInput);
void create_box(args clientInput);
void remove_box(args clientInput);
void list_box(args clientInput);

int reg_pipe;
char register_pipe[PIPENAME];

box *head;

void sig_handler(int sig){
  // UNSAFE: This handler uses non-async-signal-safe functions (printf(),
    if (sig == SIGINT) { //ctrl + c
        clear_session(reg_pipe, register_pipe);
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
            sscanf(buffer, "%d %s %s", &code, clientInput._client_pipe, clientInput._box_name);
            if (strlen(clientInput._client_pipe) == 0 || strlen(clientInput._client_pipe) == 0) {
                fprintf(stdout, "ERROR %s\n", "Wrong Client Input");
                continue;   // TODO mudar depois?
            }

            switch(code) {
                case 1:
                    reg_publisher(clientInput);
                    break;
                case 2:
                    reg_subscriber(clientInput);
                    break;
                case 3:
                    create_box(clientInput);
                    break;
                case 5:
                    remove_box(clientInput);
                    break;
                case 7:
                    list_box(clientInput);
                    break;
                default:

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
    free_boxes(head); //É SUPOSTO APAGAR TODO O CONTEUDO DPS DO MBROKER BAZAR??
    if(tfs_destroy()==-1)
        fprintf(stdout, "ERROR: Couldn't clear tfs\n");
    clear_session(reg_pipe, register_pipe);
    return 0;
}

void create_box(args clientInput){
    //printf("enter create\n");
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
        //printf("creating file:%d\n", fh);
        if((fh = tfs_open(clientInput._box_name, TFS_O_CREAT))==-1){ //file musnt exist
            fprintf(stdout, "ERROR: %s\n", "Failed to open file");
            free(error_msg);
            return;
        }
        
        boxToCreate->hasWriter = 0;
        boxToCreate->n_readers = 0;
        strcpy(boxToCreate->box_name, clientInput._box_name);
        //iterate_box(head);
        head = insert_box(boxToCreate, head);
        //iterate_box(head);
    }
    char *msg = serializeAnswer(ANSWER_CREATE_BOX, return_code, error_msg);
    send_request(fd, msg);

    if (tfs_close(fh) == -1){
        fprintf(stdout,"ERROR %s\n", "Failed to close file");
    }

    free(error_msg);
}

void list_box(args clientInput) {
    int fd = open(clientInput._client_pipe, O_WRONLY);
    if (fd == -1) {
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        return;
    }
    box *current = head;
    char *str;
    uint8_t last = 0;
    uint64_t pub, sub, size = 0;

    if(current == NULL){ //no boxes
        last = 1;
        char box_empty[BOXNAME];
        memset(box_empty, '\0', BOXNAME);
        pub=0;
        sub=0;
        str = serializeListing(LIST_ANSWER, last, box_empty, size, pub, sub);
        send_request(fd, str);
        free(str);
        close(fd);
        return;
    }

    while(current!= NULL){
        size = sizeof(current);
        if(current->next == NULL){
            last = 1;
        }
        
        pub = (uint64_t) current->hasWriter;
        sub = (uint64_t) current->n_readers;
        str = serializeListing(LIST_ANSWER, last, current->box_name, size, pub, sub);
        send_request(fd, str);
        free(str);
        current = current->next;
    }
    close(fd);
}

void remove_box(args clientInput) {
    iterate_box(head);
    int fd = open(clientInput._client_pipe, O_WRONLY);
    if (fd == -1) {
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        return;
    }
    int32_t return_code = 0;
    char *error_msg = malloc(sizeof(char)*(ERROR_MSG+100));
    memset(error_msg,'\0',sizeof(char)*(ERROR_MSG+100));
    box *boxToDelete = find_box(clientInput._box_name, head);
    int fh;
    if ((fh=tfs_open(clientInput._box_name, TFS_O_APPEND)) ==-1 || boxToDelete->n_readers == -1) {  //nao existe
        free(boxToDelete);
        strcpy(error_msg, "Box doesn't exist\n");
        return_code = -1;
    } else {
        head = delete_box(boxToDelete->box_name, head);
        
        
        if (tfs_close(fh)==-1){
            fprintf(stdout,"ERROR %s\n", "Failed to close file");
        }

        if (tfs_unlink(clientInput._box_name)==-1){
            return_code = -1;
            fprintf(stdout,"ERROR %s\n", "Failed to delete file");
        }
        
    }
    char *msg = serializeAnswer(ANSWER_CREATE_BOX, return_code, error_msg);
    send_request(fd, msg);
    iterate_box(head);
    free(error_msg);
    close(fd);
}

void reg_publisher(args clientInput) {
    int fd = open(clientInput._client_pipe, O_RDONLY);
    if (fd == -1) {
        fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
        return;
    }
    box* boxToWrite = find_box(clientInput._box_name, head);
    if (boxToWrite->hasWriter != 0){        //box doesnt exist or doesnt have writers or full writers
        printf("cant create box, finishing\n");
        clear_session(fd, clientInput._client_pipe);   // signals the publisher that his request failed
        free(boxToWrite);
        return;
    }
    int fh;
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
    boxToWrite->hasWriter = 1;
    while((read(fd, buffer, MSIZE)) > 0){  //detects if publisher closed the pipe
        //tfs write should detect if file is deleted
        sscanf(buffer, "%d %[^\n]%*c", &code, message);
        if (strlen(message) == 0) {
            break;
        }
        strcat(message," ");
        if((b=tfs_write(fh, message, strlen(message))+1)<0)
            break;
        memset(buffer, '\0', sizeof(char)*(MSIZE+100));

    }
    if (tfs_close(fh) == -1){
        fprintf(stdout,"ERROR %s\n", "Failed to close file");
    }
    boxToWrite->hasWriter = 0;
    close(fd); 
    free(buffer);
    buffer = NULL;
}


void reg_subscriber(args clientInput) {

    int fd = open(clientInput._client_pipe, O_WRONLY);
        if (fd == -1) {
            fprintf(stdout, "ERROR: %s\n", UNEXISTENT_PIPE);
            return;
        }
    box *boxToRead = find_box(clientInput._box_name, head);
    //Box exists?
    if (boxToRead->hasWriter == -1){    // box doesnt exist  
        clear_session(fd, clientInput._client_pipe);    // signals the publisher that his request failed
        free(boxToRead);
        return;
    }
    
    int fh = tfs_open(boxToRead->box_name, TFS_O_CREAT);  //manager already created file
    if (fh == -1){
        clear_session(fd, clientInput._client_pipe);
        free(boxToRead);
        return;
    }
    char *buffer = (char*) malloc(sizeof(char)*(MSIZE+100));
    char *msg;
    //tfs read should detect if file is deleted
    size_t len;
    ssize_t b, prev = 0;
    boxToRead->n_readers += 1; //increase readers
    while((b = tfs_read(fh, buffer, MSIZE)) != -1){                                                                                                    

        sleep(1);
        if (b == prev)
            continue;
        while(b>0){
            len = strlen(buffer);
            msg = serializeMessage(SERVER_SEND, buffer);
            if((write(fd, msg, strlen(msg))) < 0){      //detects if subscriber closed the pipe
                break;
            }
            
            memset(buffer, '\0', sizeof(char)*(MSIZE+100));
            free(msg);
            b -= (ssize_t) len +1;
        }
        prev = b;
        
    }
    if (tfs_close(fh) == -1){
        fprintf(stdout,"ERROR %s\n", "Failed to close file");
    }
    boxToRead->n_readers -= 1;
    unlink(clientInput._client_pipe);
    free(buffer);
    buffer = NULL;
    return;
}

