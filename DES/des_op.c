/*************************************
 * CPSC3730 Programming Assignmen #1
 * Student: Kun Han
 * ID: 001178182
 ************************************/
//des_op.c
//Main function to use DES
#include <stdio.h>
#include <string.h>

#include "des.h"

//Declare the name of flags
#define ENCRYPT "-e"
#define DECRYPT "-d"
#define HELP "-h"

//Declare the key size
#define KEY_SIZE 8

int main(int argc, char *argv[])
{
    //Check if the input is the correct form
    if(argc < 2 || argc > 5)
    {
        puts("You must provide at lease 1 parameter, and no more than 4 parameters\n");
        return 1;
    }

    //Get the flag, and start doing corresponding operation
    if(strcmp(argv[1], ENCRYPT) == 0)
    {
        DES_Encrypt(argv[2], argv[3], argv[4]);
    }
    else if(strcmp(argv[1], DECRYPT) == 0)
    {
        //TODO: Decrypt the file
    }
    else if(strcmp(argv[1], HELP) == 0)
    {
        //TODO: Show instruction
        puts("Instruction:\n");
    }
    else
    {
        puts("Incorrect format, please use ./des_op -h to get the instruction\n");
        return 1;
    }

    return 0;
}
