#ifndef __UTILS_LOGGING_H__
#define __UTILS_LOGGING_H__

#define PIPENAME 256
#define BOXNAME 32
#define MSIZE 256
#define COMMAND 8

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> //open
#include <fcntl.h>  //flags for open
#include <unistd.h>

static const __uint8_t REGISTER_PUBLISHER = 1;
static const __uint8_t REGISTER_SUBSCRIBER = 2;
static const __uint8_t CREATE_BOX = 3;
static const __uint8_t REMOVE_BOX = 5;
static const __uint8_t LIST_BOX = 7;

static const char ERROR_CREATING_THREAD[] = "Couldn't create thread";
static const char INVALID_SESSIONS[] = "Invalid number of sessions";
static const char PIPENAME_TOO_BIG[] = "The pipe's name has too many characters";
static const char INVALID_BOXNAME[] = "The box's name is invalid";
static const char INVALID_NUMBER_OF_ARGUMENTS[] = "Insufficient or too many arguments";
static const char INVALID_ARGUMENTS[] = "One of the arguments is invalid";
static const char UNEXISTENT_PIPE[] = "Pipe doesn't exist";
static const char EXISTENT_PIPE[] = "Pipe already exists";
static const char ERROR_WRITING_PIPE[] = "Error writing to pipe";
static const char SIGNAL_FAIL[] ="Failed to initialize signals";

typedef struct box{
    char boxname[BOXNAME];
    int hasWriter;
    int n_readers;
    struct box *next;
} box;

typedef struct req{
    int _code;
    char _client_pipe[PIPENAME];
    char _box_name[BOXNAME];
} request;

typedef enum {
    LOG_QUIET = 0,
    LOG_NORMAL = 1,
    LOG_VERBOSE = 2,
} log_level_t;

void clear_session(int fd, char* fn);
/*writes to pipe tx a pointer with information*/
void send_request(int tx, char *r1);

/*Returns a pointer to a struct containing the request*/
char *serialize(int code, char* client_pipe, char* box_name);

void sig_handler(int sig);

void set_log_level(log_level_t level);
extern log_level_t g_level;

#define INFO(...)                                                              \
    do {                                                                       \
        char buf[2048];                                                        \
        snprintf(buf, 2048, __VA_ARGS__);                                      \
        fprintf(stderr, "[INFO]:  %s:%d :: %s :: %s\n", __FILE__, __LINE__,    \
                __func__, buf);                                                \
    } while (0);

#define PANIC(...)                                                             \
    do {                                                                       \
        char buf[2048];                                                        \
        snprintf(buf, 2048, __VA_ARGS__);                                      \
        fprintf(stderr, "[PANIC]: %s:%d :: %s :: %s\n", __FILE__, __LINE__,    \
                __func__, buf);                                                \
        exit(EXIT_FAILURE);                                                    \
    } while (0);

#define WARN(...)                                                              \
    do {                                                                       \
        if (g_level == LOG_NORMAL || g_level == LOG_VERBOSE) {                 \
            char buf[2048];                                                    \
            snprintf(buf, 2048, __VA_ARGS__);                                  \
            fprintf(stderr, "[WARN]:  %s:%d :: %s :: %s\n", __FILE__,          \
                    __LINE__, __func__, buf);                                  \
        }                                                                      \
    } while (0);

#define LOG(...)                                                               \
    do {                                                                       \
        if (g_level == LOG_NORMAL || g_level == LOG_VERBOSE) {                 \
            char buf[2048];                                                    \
            snprintf(buf, 2048, __VA_ARGS__);                                  \
            fprintf(stderr, "[LOG]:   %s:%d :: %s :: %s\n", __FILE__,          \
                    __LINE__, __func__, buf);                                  \
        }                                                                      \
    } while (0);

#define DEBUG(...)                                                             \
    do {                                                                       \
        if (g_level == LOG_VERBOSE) {                                          \
            char buf[2048];                                                    \
            snprintf(buf, 2048, __VA_ARGS__);                                  \
            fprintf(stderr, "[DEBUG]: %s:%d :: %s :: %s\n", __FILE__,          \
                    __LINE__, __func__, buf);                                  \
        }                                                                      \
    } while (0);

#endif // __UTILS_LOGGING_H__
