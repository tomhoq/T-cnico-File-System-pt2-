#include "logging.h"

log_level_t g_level = LOG_QUIET;

void set_log_level(log_level_t level) { g_level = level; }

void clear_session(int fd, char* fn){
    close(fd);
    unlink(fn);
}

box *find_box(char box_name[], box *head){
    box *unexistent_box = malloc(sizeof(box));
    box *current = head;
    while(current!=NULL){
        if(!strcmp(current->box_name, box_name))
            return current;
        current = current->next;
    }
    strcpy(unexistent_box->box_name, "unexistent");
    unexistent_box->n_readers = -1;
    unexistent_box->hasWriter = -1;
    return unexistent_box;
}

box *insert_box(char box_name[], box *head){
    box *newBox = malloc(sizeof(box));
    strcpy(newBox->box_name,box_name);
    newBox->next = NULL;

    box *temp = head;
    while(temp->next != NULL){
        temp = temp->next;
    }

    temp->next = newBox;
    return head;
}
/*
box *delete_box(char box_name[], box *head){
    box *previous, *current = head;

    if (current != NULL && current->data == key) {
        *head_ref = temp->next; // Changed head
        free(temp); // free old head
        return;
    }
    while(current!=NULL){
        if(!strcmp(current->box_name, box_name))
            return current;
        previous = current;
        current = current->next;
    }
    return head;
}*/

/*writes to pipe tx a pointer with information*/
void send_request(int tx, char *r1) {
    ssize_t ret = write(tx, r1, sizeof(char)*400); 
    if (ret < 0) {
        fprintf(stdout, "ERROR: %s\n", ERROR_WRITING_PIPE);
        exit(EXIT_FAILURE);
    }
}

/*Returns a pointer to a struct containing the char*/
char *serialize(int code, char* client_pipe, char* box_name){
    char *r1 = (char*) malloc(sizeof(char)*400);
    sprintf(r1, "%d %s %s", code, client_pipe, box_name);
    r1 = realloc(r1, strlen(r1)+1);
    return r1;
}