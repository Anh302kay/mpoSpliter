#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXFILELENGTH 256

int main(int argc, char* argv[])
{
    if(argc < 2)
    return -1;
    
    FILE* MPO=fopen(argv[1], "rb");
    if(!MPO)
    {
    perror("Couldn't open file");
    return -1;
    }
    
    fseek(MPO, 0, SEEK_END);
    int size = ftell(MPO);
    if(size == -1)
    {
    perror("Couldn't get file size");
    return -1;
    }
    
    rewind(MPO);

    unsigned char* data = malloc(size);
    memset(data, 0, size);
    fread(data, 1, size, MPO);

    fclose(MPO);
    MPO = NULL;

    const unsigned char header[4] = {0xFF, 0xD8, 0xFF, 0xE1};
    if(memcmp(data, header, 4) != 0)
    {
    perror("JPEG HEADER NOT DETECTED");
    return -1;
    }


    int rViewStart = 0;
    // 1088 is when header ends
    for(int i = 1088; i < size; i++)
    {
        if(memcmp(&data[i], header, 4) == 0)
        {
            rViewStart = i;
            break;
        }
    }
    if(rViewStart == 0)
    {
        printf("Couldn't find 2nd JPEG HEADER");
        return -1;
    }

    // fix path
    char filePath[MAXFILELENGTH];
    strncpy(filePath, argv[1], MAXFILELENGTH);
    for(int i = 0; i < MAXFILELENGTH; i++)
    {
        if(filePath[i] == '\\')
            filePath[i] = '/';
    }
    // get rid of file extension
    const char* fileName = strrchr(filePath, '/')+1;
    *strrchr(filePath, '.') = '\0';

    char leftName[MAXFILELENGTH];
    char rightName[MAXFILELENGTH];

    strcpy(leftName, fileName);
    strcat(leftName, "-left.jpg");

    strcpy(rightName, fileName);
    strcat(rightName, "-right.jpg");

    FILE* left=fopen(leftName, "wb");
    fwrite(data, 1, rViewStart, left);
    fclose(left);

    FILE* right=fopen(rightName, "wb");
    fwrite(&data[rViewStart], 1, size-rViewStart, right);
    fclose(right);

    free(data);

    return 0;
}