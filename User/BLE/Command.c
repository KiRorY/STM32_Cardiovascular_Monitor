#include "Command.h"
#include "Constant.h"
#include <stdint.h>

/******************内部函数*******************/

//计算CRC16
uint16_t get_CRC16(uint8_t* data, uint16_t data_size)
{
    uint16_t crc = 0xFFFF;
    uint16_t polynomial = 0x8005;

    for (uint16_t index = 0; index < data_size; index++) {
        crc ^= ((uint16_t)data[index]) << 8;
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x8000){
                crc = (crc << 1) ^ polynomial;
            }
            else{
                crc = crc << 1;
            }
        }
    }

    return crc;
}


/******************外部函数*******************/

/*
 * @brief 解析传输报文
 * @param[in] data 原始报文
 * @param[in] data_size 原始报文长度
 * @param[out] output 报文结构体
 * @return 解析成功为TRUE; 报文校验出错返回FALSE,且output不赋值
 * */
BOOL command_parse(uint8_t* data, uint16_t data_size, BLECommandTypedef* output)
{
    int ret = FALSE;
    if (data_size > DATALEN){
        return ret; 
    }
    uint8_t index = 0;
    //cmd[2]
    output->cmd[0] = data[index];
    index ++;
    output->cmd[1] = data[index]; 
    index ++;

    //datalen
    output->datalen = data[index] << 8;
    index ++;
    output->datalen += data[index];
    index ++;

    //data
    for (uint8_t i = 0; i < output->datalen; i++){
        output->data[i] = data[index + i];
    }
    index += output->datalen;

    //CRC
    output->crc[0] = data[index];
    index ++;
    output->crc[1] = data[index];
    index ++;
    
    ret = TRUE;
    return ret;
}

/*
 * @brief 组装报文(自动获取组装CRC校验码)
 * @param[in] cmd 报文结构体
 * @param[out] output 输出组装后数据
 * @param[out] 输出数据长度
 * @return 解析成功为TRUE; 报文结构出错返回FALSE,且output不赋值
 * */
BOOL command_decipher(BLECommandTypedef* cmd, uint8_t* output, uint16_t* data_size)
{
    BOOL ret = FALSE;
    uint8_t index = 0;

    //cmd[2]
    output[index] = cmd->cmd[0];
    index++;
    output[index] = cmd->cmd[1];
    index++;

    //datalen
    output[index] = (cmd->datalen >> 8) & 0xFF;
    index++;
    output[index] = cmd->datalen & 0xFF;
    index++;

    //data
    for (uint8_t i = 0; i < cmd->datalen; i++){
        output[index + i] = cmd->data[i];
    }
    index += cmd->datalen;

    //CRC
    uint16_t crc = get_CRC16(output, index);
    output[index] = (crc >> 8) & 0xFF;
    index++;
    output[index] = crc & 0xFF;
    index++;

    *data_size = index;
    
    ret = TRUE;
    return ret;
}
