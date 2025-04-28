#ifndef COMMAND_AND_PARSE_H_
#define COMMAND_AND_PARSE_H_

#include "main.h"
#include "Constant.h"
#include <stdint.h>

#define DATALEN 255 - 2 - 2 - 1

typedef struct{
    uint8_t         cmd[2];
    uint8_t         datalen;
    uint8_t         data[DATALEN];
    uint8_t         crc[2];
} BLECommandTypedef;

enum Command_Check{
    OK,
    Overflow,
    CRCerr 
};

extern BOOL command_parse(uint8_t* data, uint16_t data_size, BLECommandTypedef* output);
extern BOOL command_decipher(BLECommandTypedef* cmd, uint8_t* output, uint16_t* data_size);
#endif /*COMMAND_AND_PARSE_H_*/
