#include "logging.h"

log_level_t g_level = LOG_QUIET;

void set_log_level(log_level_t level) { g_level = level; }

void clear_session(int fd, char* fn){
    close(fd);
    unlink(fn);
}

/*writes to pipe tx a pointer with information*/
void send_request(int tx, request *r1) {
    ssize_t ret = write(tx, &r1, sizeof(r1)); 
    if (ret < 0) {
        fprintf(stdout, "ERROR: %s\n", ERROR_WRITING_PIPE);
        exit(EXIT_FAILURE);
    }
}

/*Returns a pointer to a struct containing the request*/
request *serialize(int code, char* client_pipe, char* box_name){
    request *r1 = (request*) malloc(sizeof(request));
    r1->_code  = code;
    strcpy(r1->_client_pipe, client_pipe); 
    strcpy(r1->_box_name, box_name); 
    return r1;
}