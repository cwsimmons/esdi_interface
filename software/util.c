
#include "util.h"

#include <stdio.h>

void hex_print(uint8_t* buffer, int length) {
    for (int i = 0; i < length; i++) {
        printf("%.2x ", buffer[i]);
        if ((i % 16) == 15) {
            printf("\n");
        }
    }
    if ((length % 16)) {
        printf("\n");
    }
}
