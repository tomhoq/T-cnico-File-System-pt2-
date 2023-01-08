#include "logging.h"
#include <string.h>

static void print_usage() {
    fprintf(stderr, "usage: \n"
                    "   manager <register_pipe_name> create <box_name>\n"
                    "   manager <register_pipe_name> remove <box_name>\n"
                    "   manager <register_pipe_name> list\n");
}

int main(int argc, char **argv) {
    print_usage();

    if (argc > 5 || argc < 4)
        fprintf(stdout, "ERROR: %s\n", INVALID_NUMBER_OF_ARGUMENTS);

    char register_pipe[PIPENAME];               
    char personal_pipe[PIPENAME];
    char command[8];

    if (strlen(argv[1]) >= PIPENAME || strlen(argv[2]) >= PIPENAME)
        fprintf(stdout, "ERROR: %s\n", PIPENAME_TOO_BIG);

    strcpy(register_pipe, argv[1]);
    strcpy(personal_pipe, argv[2]);
    strcpy(command, argv[3]);

    if (!strcmp(command, "create")) {
        //create box
    }
    else if(!strcmp(command, "remove")){
        //remove box
    } else if (!strcmp(command, "list")){
        //list all boxes
    } else {
        fprintf(stdout, "ERROR: %s\n", INVALID_ARGUMENTS);
    }
 


    printf("%s %s", register_pipe, personal_pipe);

    return 0;
}

