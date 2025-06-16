#include "Common.h"

float String2Float(uint8_t *data_string, uint16_t size) {
    
}

void transposeArray(char *Dest, char *Src, uint8_t col, uint8_t row) {
    for (int i = 0; i < col; i++) {
        for (int j = 0; j < row; j++) {
            Dest[i * row + j] = Src[j * col + i];
        }
    }
}
