#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {


    // Usage filein fileout h:s h:s

    FILE* filein = fopen(argv[1], "rb");
    FILE* fileout = fopen(argv[2], "wb");

    int old_h, old_s;
    int new_h, new_s;

    sscanf(argv[3], "%d:%d", &old_h, &old_s);
    sscanf(argv[4], "%d:%d", &new_h, &new_s);

    int c = 0;

    char buffer[512];
    int status = 1;

    while (status) {

        for (int h = 0; (h < new_h) && status; h++) {
            for (int s = 0; (s < new_s) && status; s++) {
                status = 0;
                if ((h < old_h) && (s < old_s)) {
                    int lba = (old_h * old_s * c) + (old_s * h) + s;
                    fseek(filein, lba * 512, SEEK_SET);
                    status = fread(buffer, 512, 1, filein);
                } else {
                    memset(buffer, 0x00, 512);
                    status = 1;
                }
                if (status)
                    fwrite(buffer, 512, 1, fileout);
            }
        }


        c++;

    };



    return 0;
}