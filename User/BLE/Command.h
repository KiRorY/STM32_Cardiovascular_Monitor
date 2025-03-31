#ifndef COMMAND_AND_PARSE_H_
#define COMMAND_AND_PARSE_H_

#include "main.h"
#include "Constant.h"
#include <stdint.h>


typedef struct{
    uint8_t         cmd[2];
    uint16_t        datalen;
    uint8_t         data[DATALEN];
    uint8_t         crc[2];
} Command_Typedef;

enum Command_Check{
    OK,
    Overflow,
    CRCerr 
};

extern BOOL Command_Parse(uint8_t* data, uint16_t data_size, Command_Typedef* output);
extern BOOL Command_Decipher(Command_Typedef* cmd, uint8_t* output, uint16_t* data_size);
#endif /*COMMAND_AND_PARSE_H_*/
