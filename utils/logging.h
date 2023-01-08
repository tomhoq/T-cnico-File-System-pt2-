#ifndef __UTILS_LOGGING_H__
#define __UTILS_LOGGING_H__

#define PIPENAME 256
#define BOXNAME 32


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> //open
#include <fcntl.h>  //flags for open

static const __uint8_t REGISTER_PUBLISHER = 1;
static const __uint8_t REGISTER_SUBSCRIBER = 2;
static const __uint8_t CREATE_BOX = 3;


static const char PIPENAME_TOO_BIG[] = "The pipe's name has too many characters";
static const char BOXNAME_TOO_BIG[] = "The box's name has too many characters";
static const char INVALID_NUMBER_OF_ARGUMENTS[] = "Insufficient or too many arguments";
static const char INVALID_ARGUMENTS[] = "One of the arguments is invalid";
static const char UNEXISTENT_PIPE[] = "Pipe doesn't exist";
static const char EXISTENT_PIPE[] = "Pipe already exists";
static const char ERROR_WRITING_PIPE[] = "Error writing to pipe";

typedef struct box{
    char boxname[BOXNAME];
    char pub_pipename[PIPENAME];
    struct box *next;
    struct box *previous;
} box;

typedef enum {
    LOG_QUIET = 0,
    LOG_NORMAL = 1,
    LOG_VERBOSE = 2,
} log_level_t;

void set_log_level(log_level_t level);
extern log_level_t g_level;

void send_msg(int tx, char const *str) {
    size_t len = strlen(str);
    size_t written = 0;

    while (written < len) {
        ssize_t ret = write(tx, str + written, len - written);
        if (ret < 0) {
            fprintf(stdout, "ERROR: %s\n", ERROR_WRITING_PIPE);
            exit(EXIT_FAILURE);
        }

        written += (size_t)ret;
    }
}


char* serialize(int code, char* pers_pipe, char* box_name){
    char *str = malloc(sizeof(char)*320);
    sprintf(str, "[ %d ] | [ %s ] | [ %s ]", code, pers_pipe, box_name);
    str = realloc(str,strlen(str)+1);

    return str;
}


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
