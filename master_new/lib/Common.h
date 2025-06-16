#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <string.h>

float String2Float(uint8_t *data_string, uint16_t size);
void transposeArray(char *Dest, char *Src ,uint8_t col, uint8_t row);

#endif /* __COMMON_H__ */