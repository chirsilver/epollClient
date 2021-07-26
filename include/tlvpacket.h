#ifndef _TLVPACKET_H_
#define _TLVPACKET_H_

#define DATASIZE 1024
#define DATA 1
#define ERROR 2
#define OTHER 3

#include <string.h>

union TLVPacket{
    struct {
        int type;
        int length;
        char value[DATASIZE - 8];
    }node;
    char data[DATASIZE];
};

const char *prase(int type) {
    if(type == 1) return "DATA";
    if(type == 2) return "ERROR";
    else return "OTHER";
}

int reprase(const char *str) {
    char cstr[7];
    strncpy(cstr, str, 7);
    cstr[6] = '\0';
    if(strcmp("ERROR", cstr) == 0) return 2;
    cstr[5] = '\0';
    if(strcmp("DATA", cstr) == 0) return 1;
    return 3;
}

#endif