#include "logging.h"

log_level_t g_level = LOG_QUIET;

void set_log_level(log_level_t level) { g_level = level; }

void clear_session(int fd, char* fn){
    close(fd);
    unlink(fn);
}

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