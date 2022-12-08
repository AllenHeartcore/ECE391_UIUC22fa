#include <stdint.h>
#include "ece391support.h"
#include "ece391syscall.h"

#define BUFSIZE 128

int main(){
    int fd;     // file descriptor
    int i, j;      // loop interator
    uint8_t args[BUFSIZE];
    uint8_t filename[BUFSIZE];
    uint8_t content[BUFSIZE];
    if (0 != ece391_getargs (args, BUFSIZE)) {
        ece391_fdputs (1, (uint8_t*)"could not read argument\n");
        return 3;
    }
    for (i = 0; i < ece391_strlen((uint8_t*)args) &&
            args[i] != '\0' &&
            args[i] != ' ' &&
            i < BUFSIZE;
         i++){
        filename[i] = args[i];
    }
    filename[i] = '\0';
    for (; i < ece391_strlen((uint8_t*)args) &&
            args[i] != '\0' &&
            args[i] == ' ';
         i++);
    for (j = 0; i < ece391_strlen((uint8_t*)args) &&
            args[i] != '\0' &&
            i < BUFSIZE;
         i++, j++) {
        content[j] = args[i];
    }
    content[j] = '\0';
    ece391_fdputs(1, filename);
    ece391_fdputs(1, (uint8_t*)"\n");
    ece391_fdputs(1, content);
    if ((fd = ece391_open(filename)) == -1){
        ece391_fdputs(1, (uint8_t*)"Can not open this file.\n");
        return 2;
    }
    if (-1 == ece391_write(fd, (void*)content, ece391_strlen(content))){
        ece391_fdputs(1, (uint8_t*)"Can not write into this file.\n");
        return 2;
    }
    return 0;
}