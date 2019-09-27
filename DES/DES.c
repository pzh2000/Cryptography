/**************************************
 * CPSC3730 Programming Assignment 1
 * Student: Kun Han
 * ID: 001178182
 **************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PLAIN_FILE_OPEN_ERROR -1
#define KEY_FILE_OPEN_ERROR -2
#define CIPHER_FILE_OPEN_ERROR -3
#define ILLEGAL_CIPHER_FILE -4

//Table for initial permutation
int IP_Table[64] = { 57, 49, 41, 33, 25, 17, 9, 1,
                     59, 51, 43, 35, 27, 19, 11, 3,
                     61, 53, 45, 37, 29, 21, 13, 5,
                     63, 55, 47, 39, 31, 23, 15, 7,
                     56, 48, 40, 32, 24, 16, 8, 0,
                     58, 50, 42, 34, 26, 18, 10, 2,
                     60, 52, 44, 36, 28, 20, 12, 4,
                     62, 54, 46, 38, 30, 22, 14, 6};

//Inversed table for final permutation
int IP_inv_Table[64] = {39,7,47,15,55,23,63,31,38,6,46,14,54,22,62,30,37,5,45,13,53,21,61,29,36,4,44,12,52,20,60,28,
              35,3,43,11,51,19,59,27,34,2,42,10,50,18,58,26,33,1,41,9,49,17,57,25,32,0,40,8,48,16,56,24};

//Expansion Table
int E_Table[48] = { 31, 0, 1, 2, 3, 4, 3, 4,
                    5, 6, 7, 8, 7, 8, 9, 10,
                    11, 12, 11, 12, 13, 14, 15, 16,
                    15, 16, 17, 18, 19, 20, 19, 20,
                    21, 22, 23, 24, 23, 24, 25, 26,
                    27, 28, 27, 28, 29, 30, 31, 0};

//Table for permutaion within the round function
int P_Table[32] = { 15, 6, 19, 20, 28, 11, 27, 16,
                    0, 14, 22, 25, 4, 17, 30, 9,
                    1, 7, 23, 13, 31, 26, 2, 8,
                    18, 12, 29, 5, 21, 10, 3, 24};

//S-Box
int S[8][4][16] =

{   
    //S1
    {
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
    },
    //S2
    {
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
    },
    //S3
    {
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
    },
    //S4
    {
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
    },
    //S5
    {
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
    },
    //S6
    {
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
    },
    //S7
    {
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
    },
    //S8
    {
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
    }
};

//Table for permutation choice 1
int PC_1[56] = {56,48,40,32,24,16,8,0,57,49,41,33,25,17,9,1,58,50,42,34,26,18,10,2,59,51,43,35,
                62,54,46,38,30,22,14,6,61,53,45,37,29,21,13,5,60,52,44,36,28,20,12,4,27,19,11,3};

//Table for permutaiton choice 2
int PC_2[48] = {13, 16, 10, 23, 0, 4, 2, 27,
                14, 5, 20, 9, 22, 18, 11, 3,
                25, 7, 15, 6, 26, 19, 12, 1,
                40, 51, 30, 36, 46, 54, 29, 39,
                50, 44, 32, 46, 43, 48, 38, 55,
                33, 52, 45, 41, 49, 35, 28, 31};

//Times for left circular shift
int Left_Cir_Shift[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

int ByteToBit(char ch,char bit[8]); //Convert Bytes into bits

int BitToByte(char bit[8],char *ch); //Convert bits into Bytes

int CharToBit(char ch[8],char bit[64]); //Convert a 8 Byte character into 64 bits

int BitToChar(char bit[64],char ch[8]); //Convert 64 bits into 8 Byte characters

int DES_MakeSubKeys(char key[64],char subKeys[16][48]); //Generate the subkey

int DES_PC1(char key[64], char tempbts[56]); //Permuted choice 1

int DES_PC2(char key[56], char tempbts[48]); //Permuted choice 2

int DES_LShift(char data[56], int time); //Left circular shift

int DES_IP(char data[64]); //Initial permutation

int DES_IP_inv(char data[64]); //Final permutation

int DES_Expansion(char data[48]); //Expand 32 bit input to 48 bit text

int DES_Perm(char data[32]); //Permutation in the F function

int DES_SBOX(char data[48]); //S-Box transformation

int DES_XOR(char R[48], char L[48],int count); //XOR

int DES_Swap(char left[32],char right[32]); //Swap the left half and the right half of the text

int DES_EncryptBlock(char plainBlock[8], char subKeys[16][48], char cipherBlock[8]); //Encrypt a single block

int DES_DecryptBlock(char cipherBlock[8], char subKeys[16][48], char plainBlock[8]); //Decrypt a single block

int DES_Encrypt(char *plainFile, char *keyStr,char *cipherFile); //Encrypt the file

int DES_Decrypt(char *cipherFile, char *keyStr,char *plainFile); //Decrypt the file

//Convert Byte into bits
int ByteToBit(char ch, char bit[8])
{
    int count;
    for(count = 0;count < 8; count++)
    {
        *(bit+count) = (ch>>count)&1;
    }
    return 0;
}

//Convert bits into a Byte
int BitToByte(char bit[8],char *ch)
{
    int count;
    for(count = 0;count < 8; count++)
    {
        *ch |= *(bit + count)<<count;
    }
    return 0;
}

//Convert a 8 Byte characters to 64 bits
int CharToBit(char ch[8],char bit[64])
{
    int count;
    for(count = 0; count < 8; count++)
    { 
        ByteToBit(*(ch+count),bit+(count<<3));
    }
    return 0;
}

//Convert 64 bits to 8 Byte characters
int BitToChar(char bit[64],char ch[8])
{
    int count;
    memset(ch,0,8);
    for(count = 0; count < 8; count++)
    {
        BitToByte(bit+(count<<3),ch+count);
    }
    return 0;
}

//Generate the sub-keys
int DES_MakeSubKeys(char key[64],char subKeys[16][48])
{
    char temp[56];
    int count;
    DES_PC1(key,temp);  //Do the permuted choice 1
    for(count = 0; count < 16; count++) //Iterate 16 times to get 16 sub-keys
    {
        DES_LShift(temp,Left_Cir_Shift[count]); //Left circular shift
        DES_PC2(temp,subKeys[count]);   //Do the permuted choice 2
    }
    return 0;
}

//Permuted choice 1, then get a 56 bit key
int DES_PC1(char key[64], char tempbts[56])
{
    int count;   
    for(count = 0; count < 56; count++)
    {
        tempbts[count] = key[PC_1[count]];
    }
    return 0;
}

//Permuted choice 2, then get a 48 bit key
int DES_PC2(char key[56], char tempbts[48])
{
    int count;
    for(count = 0; count < 48; count++)
    {
        tempbts[count] = key[PC_2[count]];
    }
    return 0;
}

//Left circular shift
int DES_LShift(char data[56], int time)
{
    char temp[56];
    memcpy(temp,data,time); //Save the right half which will be shifted
    
    //Shift 28 bits forward
    memcpy(temp+time,data+28,time); 
    memcpy(data,data+time,28-time);
    memcpy(data+28-time,temp,time);
    
    //Shift 28 bits backward
    memcpy(data+28,data+28+time,28-time);
    memcpy(data+56-time,temp+time,time);   
    return 0;
}

//Initial permutation
int DES_IP(char data[64])
{
    int count;
    char temp[64];
    for(count = 0; count < 64; count++)
    {
        temp[count] = data[IP_Table[count]];
    }
    memcpy(data,temp,64);
    return 0;
}

//Final permutation
int DES_IP_inv(char data[64])
{
    int count;
    char temp[64];
    for(count = 0; count < 64; count++)
    {
        temp[count] = data[IP_inv_Table[count]];
    }
    memcpy(data,temp,64);
    return 0;
}

//Expand 32 bits data to 48 bits
int DES_Expansion(char data[48])
{
    int count;
    char temp[48];
    for(count = 0; count < 48; count++)
    {
        temp[count] = data[E_Table[count]];
    }   
    memcpy(data,temp,48);
    return 0;
}

//Permutation in the round function
int DES_Perm(char data[32])
{
    int count;
    char temp[32];
    for(count = 0; count < 32; count++)
    {
        temp[count] = data[P_Table[count]];
    }   
    memcpy(data,temp,32);
    return 0;
}

//XOR
int DES_XOR(char R[48], char L[48] ,int t)
{
    int count;
    for(count = 0; count < t; count++)
    {
        R[count] ^= L[count];
    }
    return 0;
}

//S-Box transformation, transform a 48 bit input to a 32 bit output
int DES_SBOX(char data[48])
{
    int count;
    int column,row,output;
    int cur1,cur2;
    for(count = 0; count < 8; count++)
    {
        cur1 = count*6; //Divide into 8 parts, each part has 6 bits into the S-Box, then get a 4 bit data
        cur2 = count<<2;    //Divide into 8 parts, each part has 4 bit output
        
        //Calculate the row number and column number in the S-Box
        column = (data[cur1]<<1) + data[cur1+5];
        row = (data[cur1+1]<<3) + (data[cur1+2]<<2)
            + (data[cur1+3]<<1) + data[cur1+4];
        output = S[count][column][row];
        
        //Convert the data into binary
        data[cur2] = (output&0X08)>>3;
        data[cur2+1] = (output&0X04)>>2;
        data[cur2+2] = (output&0X02)>>1;
        data[cur2+3] = output&0x01;
    }   
    return 0;
}

//Swap the left half and the right half of the data
int DES_Swap(char left[32], char right[32]){
    char temp[32];
    memcpy(temp,left,32);   
    memcpy(left,right,32);   
    memcpy(right,temp,32);
    return 0;
}


//Encrypt a single block
int DES_EncryptBlock(char plainBlock[8], char subKeys[16][48], char cipherBlock[8])
{
    char plainBits[64];
    char rightHalf[48];
    int count;
    CharToBit(plainBlock,plainBits);       
    //Initial permutation
    DES_IP(plainBits);
    //Iterate 16 rounds
    for(count = 0; count < 16; count++)
    {
        memcpy(rightHalf,plainBits+32,32);
        //Expand the right half of data from 32 bits to 48 bits
        DES_Expansion(rightHalf);
        //Right half of data XOR with the sub-key
        DES_XOR(rightHalf,subKeys[count],48);   
        //Put the result into the S-Box, then get a 32 bit result
        DES_SBOX(rightHalf);
        //Permutation within the round function
        DES_Perm(rightHalf);
        //Left half of the plaintext XOR with the right half of data
        DES_XOR(plainBits,rightHalf,32);
        if(count != 15){
            //Finalize the swapping
            DES_Swap(plainBits,plainBits+32);
        }
    }
    //Do the final permutation
    DES_IP_inv(plainBits);
    //Convert bits into characters
    BitToChar(plainBits,cipherBlock);
    return 0;
}

//Decrypt a single block
int DES_DecryptBlock(char cipherBlock[8], char subKeys[16][48],char plainBlock[8])
{
    char cipherBits[64];
    char rightHalf[48];
    int count;
    CharToBit(cipherBlock,cipherBits);       
    //Initial permutation
    DES_IP(cipherBits);
    //Iterate 16 rounds to get 16 sub-keys
    for(count = 15; count >= 0; count--)
    {
        memcpy(rightHalf,cipherBits+32,32);
        //Expand the right half of the plaintext from 32 bits to 48 bits
        DES_Expansion(rightHalf);
        //The right half of data XOR with the sub-key
        DES_XOR(rightHalf,subKeys[count],48);       
        //Put the result into S-Box, get the 32 bits result
        DES_SBOX(rightHalf);
        //Permutation within the round function
        DES_Perm(rightHalf);       
        //The left half of the plaintext XOR with the right part of the data
        DES_XOR(cipherBits,rightHalf,32);
        if(count != 0)
        {
            //Do the final swapping
            DES_Swap(cipherBits,cipherBits+32);
        }
    }
    //Final permutation
    DES_IP_inv(cipherBits);
    BitToChar(cipherBits,plainBlock);
    return 0;
}

//Encrypt the file
int DES_Encrypt(char *plainFile, char *key,char *cipherFile)
{
    FILE *plain,*cipher;
    int count;
    char plainBlock[8],cipherBlock[8],keyBlock[8];
    char binKey[64];
    char subKeys[16][48];
    if((plain = fopen(plainFile,"rb")) == NULL)
    {
        return PLAIN_FILE_OPEN_ERROR;
    }   
    //If the file does not exist, creat a new one, if exists, overwrite it
    if((cipher = fopen(cipherFile,"wb+")) == NULL)
    {
        return CIPHER_FILE_OPEN_ERROR;
    }
    //Setup the key
    memcpy(keyBlock, key, 8);
    //Convert the key into binary
    CharToBit(keyBlock,binKey);
    //Generate sub-keys
    DES_MakeSubKeys(binKey,subKeys);
    while(!feof(plain)){
        //Read 8 byte everytime and return the number of bytes have read
        if((count = fread(plainBlock,sizeof(char),8,plain)) == 8)
        {
            DES_EncryptBlock(plainBlock,subKeys,cipherBlock);
            fwrite(cipherBlock,sizeof(char),8,cipher);   
        }
    }
    if(count)
    {
        //Fill the blank
        memset(plainBlock + count,'\0',7 - count);
        //Save the last character include the number of blanks filled
        plainBlock[7] = 8 - count;
        DES_EncryptBlock(plainBlock,subKeys,cipherBlock);
        fwrite(cipherBlock,sizeof(char),8,cipher);
    }
    fclose(plain);
    fclose(cipher);
return 0;
}

//Decrypt the file
int DES_Decrypt(char *cipherFile, char *key,char *plainFile)
{
    FILE *plain, *cipher;
    int count,times = 0;
    long fileLen;
    char plainBlock[8],cipherBlock[8],keyBlock[8];
    char binKey[64];
    char subKeys[16][48];
    if((cipher = fopen(cipherFile,"rb")) == NULL)
    {
        return CIPHER_FILE_OPEN_ERROR;
    }
    //If the file does not exist, creat a new one, if exists, overwrite it
    if((plain = fopen(plainFile,"wb+")) == NULL)
    {
        return PLAIN_FILE_OPEN_ERROR;
    }
    //Setup the key
    memcpy(keyBlock, key, 8);
    //Convert the key into binary
    CharToBit(keyBlock,binKey);
    //Generate the sub-key
    DES_MakeSubKeys(binKey,subKeys);
    //Get the length of the file
    fseek(cipher,0,SEEK_END);   //Let the file pointer point to the tail of the file
    fileLen = ftell(cipher);    //Get the current location of the file pointer
    rewind(cipher);             //Let the file pointer point to the head of the file
    while(1)
    {
        //The Byte length of the ciphertext must be multiple times of 8
        fread(cipherBlock,sizeof(char),8,cipher);
        DES_DecryptBlock(cipherBlock,subKeys,plainBlock);                       
        times += 8;
        if(times < fileLen)
        {
            fwrite(plainBlock,sizeof(char),8,plain);
        }
        else
        {
            break;
        }
    }
    //Determine if the file has been filled
    if(plainBlock[7] < 8){
        for(count = 8 - plainBlock[7]; count < 7; count++)
        {
            if(plainBlock[count] != '\0')
            {
                break;
            }
        }
    }
    else
    {
        return ILLEGAL_CIPHER_FILE; 
    }
    //If yes   
    if(count == 7)
    {
        fwrite(plainBlock,sizeof(char),8 - plainBlock[7],plain);
    }
    //If not
    else
    {
        fwrite(plainBlock,sizeof(char),8,plain);
    }
    fclose(plain);
    fclose(cipher);
return 0;
}

int main(int argc, char *argv[])
{   
    if(argc != 5)
    {
        puts("Incorrect input form, please read README");
        return 0;
    } 
    if(strcmp(argv[1], "-d") == 0)
    {
        DES_Decrypt(argv[2], argv[3], argv[4]);
    }
    else if(strcmp(argv[1], "-e") == 0)
    {
        DES_Encrypt(argv[2], argv[3], argv[4]);
    }
    else
    {
        puts("Wrong flag, Please read README.");
    }
    return 0;

}