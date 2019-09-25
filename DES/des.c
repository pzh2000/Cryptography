//des.c
//Implementation of DES encryption and decryption
#include <string.h>
#include <stdio.h>
#include "des.h"

//Initial permutation table
int IP_table[64] = {57, 49, 41, 33, 25, 17, 9, 1,
                    59, 51, 43, 35, 27, 19, 11, 3,
                    61, 53, 45, 37, 29, 21, 13, 5,
                    63, 55, 47, 39, 31, 23, 15, 7,
                    56, 48, 40, 32, 24, 16, 8, 0,
                    58, 50, 42, 34, 26, 18, 10, 2,
                    60, 52, 44, 36, 28, 20, 12, 4,
                    62, 54, 46, 38, 30, 22, 14, 6};

//Final permutation table IP^(-1)
int IP_inv_table[64] = {39, 7, 47, 15, 55, 23, 63, 31,
						38, 6, 46, 14, 54, 22, 62, 30, 
						37, 5, 45, 13, 53, 21, 61, 29,
						36, 4, 44, 12, 52, 20, 60, 28,
						35, 3, 43, 11, 51, 19, 59, 27,
						34, 2, 42, 10, 50, 18, 58, 26,
						33, 1, 41, 9, 49, 17, 57, 25,
						32, 0, 40, 8, 48, 16, 56, 24};

//Expand table
int E_Table[48] =  {31, 0, 1, 2, 3, 4,
					3, 4, 5, 6, 7, 8,
					7, 8, 9, 10, 11, 12,
					11, 12, 13, 14, 15, 16,
					15, 16, 17, 18, 19, 20,
					19, 20, 21, 22, 23, 24,
					23, 24, 25, 26, 27, 28,
					27, 28, 29, 30, 31, 0};

//S-Box
int S_Box[8][4][16] = {//S1
						{{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
						 {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
						 {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
						 {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6,13}},
						//S2
						{{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
						 {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
						 {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
						 {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
						//S3
						{{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
						 {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
						 {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
						 {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
						//S4
						{{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
						 {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
						 {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
						 {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
						//S5
						{{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
						 {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
						 {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
						 {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
						//S6
						{{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
						 {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
						 {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
						 {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
						//S7
						{{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
						 {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
						 {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
						 {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
						//S8
						{{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
						 {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
						 {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
						 {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};

//Permutation table in the round function
int P_Table[32] =  {15, 6, 19, 20,
					28, 11, 27, 16,
					0, 14, 22, 25,
					4, 17, 30, 9,
					1, 7, 23, 13,
					31, 26, 2, 8,
					18, 12, 29, 5,
					21, 10, 3, 24};

//Permuted Choice1
int PC_1[56] = {56, 48, 40, 32, 24, 16, 8,
				0, 57, 49, 41, 33, 25, 17,
				9, 1, 58, 50, 42, 34, 26,
				18, 10, 2, 59, 51, 43, 35,
				62, 54, 46, 38, 30, 22, 14,
				6, 61, 53, 45, 37, 29, 21,
				13, 5, 60, 52, 44, 36, 28,
				20, 12, 4, 27, 19, 11, 3};

//Permuted Choice2
int PC_2[48] = {13, 16, 10, 23, 0, 4,
				2, 27, 14, 5, 20, 9,
				22, 18, 11, 3, 25, 7,
				15, 6, 26, 19, 12, 1,
				40, 51, 30, 36, 46, 54,
				29, 39, 50, 44, 32, 47,
				43, 48, 38, 55, 33, 52,
				45, 41, 49, 35, 28, 31};

//Times of left circular shift
int circular_shift[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

//Convert Byte into 8 bits binary number array
int ByteToBit(char ch, char bin[8])
{
	int count;
	for(count = 0; count < 8; count++)
	{
		*(bin + count) = (ch >> count) & 1;
	}
	return 0;
}

//Convert binary bits into Byte
int BitToByte(char bin[8], char *ch)
{
    int count;
    for(count = 0; count < 8; count++)
    {
        *ch |= *(bin + count) << count;
    }
    return 0;
}

//Convert 8 Byte string into 64 bit binary numbers
int CharToBit(char str[8], char bin[64])
{
    int count;
    for(count = 0; count < 8; count++)
    {
        ByteToBit(*(str + count), (bin + (count << 3)));
    }
    return 0;
}

//Convert 64 bit binary numbers into 8 Byte string
int BitToChar(char bin[64], char str[8])
{
    int count;
    for(count = 0; count < 8; count++)
    {
        BitToByte((bin + (count << 3)), str + count);
    }
    return 0;
}

//Generate sub-keys
int GenSubKey(char key[64], char subkey[16][48])
{
	char temp[56];
	int count;
	DES_PC1(key, temp); //Do the permutation using Permuted Choice 1
	for(count = 0; count < 16; count++)//Iterate 16 rounds in order to generate 16 sub-keys
	{
		Left_Cir_Shift(temp, circular_shift[count]);
	}
	return 0;
}

//Implementation of Permuted Choice 1
int DES_PC1(char key[64], char key_pc1[56])
{
	int count;
	for(count = 0; count < 56; count++)
	{
		key_pc1[count] = key[PC_1[count]];
	}
	return 0;
}

//Implementaton of Permuted Choice 2
int DES_PC2(char key[56], char key_pc2[48])
{
	int count;
	for(count = 0; count < 48; count++)
	{
		key_pc2[count] = key[PC_2[count]];
	}
	return 0;
}

//Left circular shift
int Left_Cir_Shift(char key[56], int t)
{
	char temp[56];

	//Save the bits which will be moved to the right
	memcpy(temp, key, t);
	memcpy(temp + t, key + 28, t);

	//Move 28 bits forward
	memcpy(key, key + t, 28 - t);
	memcpy(key + 28 - t, temp, t);

	//Move 28 bits backward
	memcpy(key + 28, key + 28 + t, 28 - t);
	memcpy(key + 56 - t, temp + t, t);

	return 0;
}

//Initial permutation
int DES_IP(char data[64])
{
	int count;
	char temp[64];
	for(count = 0; count < 64; count++)
	{
		temp[count] = data[IP_table[count]];
	}
	memcpy(data, temp, 64);
	return 0;
}

//Inverse of inital permutation
int DES_IP_inv(char data[64])
{
	int count;
	char temp[64];
	for(count = 0; count < 64; count++)
	{
		temp[count] = data[IP_inv_table[count]];
	}
	memcpy(data, temp, 64);
	return 0;
}

//Expansion
int DES_Exp(char data[32])
{
	int count;
	char temp[48];
	for(count = 0; count < 48; count++)
	{
		temp[count] = data[E_Table[count]];
	}
	memcpy(data, temp, 64);
	return 0;
}

//Permutation in the F function
int DES_Per(char data[32])
{
	int count;
	char temp[32];
	for(count = 0; count < 32; count++)
	{
		temp[count] = data[P_Table[count]];
	}
	memcpy(data, temp, 32);
	return 0;
}

//XOR
int DES_XOR(char R[48], char L[48], int t)
{
	int count;
	for(count = 0; count < t; count++)
	{
		R[count] ^= L[count];
	}
	return 0;
}

//S-Box permutation
int DES_SBox(char data[48])
{
	int count;
	int col, row, consequence;
	int num1, num2;
	for(count = 0; count < 8; count++)
	{
		num1 = count * 6;
		num2 = count << 2;

		//Calculate the row and column in S-Box
		col = (data[num1] << 1) + data[num1 + 5];
		row = (data[num1 + 1] << 3) + (data[num1 + 2] << 2)
			   + (data[num1 + 3] << 1) + data[num1 + 4];
		consequence = S_Box[count][col][row];

		//Convert into binary numbers
		data[num2] = (consequence & 0X08) >> 3;
		data[num2 + 1] = (consequence & 0X04) >> 2;
		data[num2 + 2] = (consequence & 0X02) >>1;
		data[num2 + 3] = consequence & 0X01;
	}
	return 0;
}

//Swap function
int DES_Swap(char L[32], char R[32])
{
	char temp[32];
	memcpy(temp, L, 32);
	memcpy(L, R, 32);
	memcpy(R, temp, 32);
	return 0;
}

//Encrypt a single block
int DES_EncryptBlock(char plain[8], char subKey[16][48], char cipher[8])
{
	char plainBit[64];
	char rightHalf[48];
	int count;

	CharToBit(plain, plainBit);
	//Initial permutation
	DES_IP(plainBit);

	//16 rounds of iteration
	for(count = 0; count < 16; count++)
	{
		//Copy the right half of the plain bits and do the expansion
		memcpy(rightHalf, plainBit + 32, 32);
		DES_Exp(rightHalf);
		//Right half XOR with key
		DES_XOR(rightHalf, subKey[count], 48);
		//Put the XORed data into the S-Box, get a 32-bit result
		DES_SBox(rightHalf);
		//Permutation within the round function
		DES_Per(rightHalf);
		//Left half of the plain bits XOR with the right half
		DES_XOR(plainBit, rightHalf, 32);
		//Swap the two sides of data
		if(count != 15)
		{
			DES_Swap(plainBit, plainBit + 32);
		}
	}
	//Do the inversed Initial Permutation
	DES_IP_inv(plainBit);
	BitToChar(plainBit, cipher);
	return 0;
}

//Decrypt a single block
int DES_DecryptBlock(char cipher[8], char subKey[16][48], char plain[8])
{
	char cipherBit[64];
	char rightHalf[48];
	int count;

	CharToBit(cipher, cipherBit);
	//Initial permutation
	DES_IP(cipherBit);

	//16 rounds of iteration
	for(count = 15; count >= 0; count--)
	{
		//Expand the right half of the cipertext
		memcpy(rightHalf, cipherBit + 32, 32);
		DES_Exp(rightHalf);
		//Right half XOR with key
		DES_XOR(rightHalf, subKey[count], 48);
		//Put the result into the S-Box, then we will get a 32-bit result
		DES_SBox(rightHalf);
		//Permutation within the round function
		DES_Per(rightHalf);
		//Left half of the plain bits XOR with the right half
		DES_XOR(cipherBit, rightHalf, 32);
		//Swap the two sides of data
		if(count != 0)
		{
			DES_Swap(cipherBit, cipherBit + 32);
		}
	}
	//Do the inversed Initial Permutation
	DES_IP_inv(cipherBit);
	BitToChar(cipherBit, plain);
	return 0;
}

//Encrypt the file
int DES_Encrypt(char *pFile, char *key, char *cFile)
{
	FILE *cipher, *plain;
	int count;
	char plainBlock[8], cipherBlock[8], keyBlock[8];
	char binKey[64];
	char subKey[16][48];
	if((plain = fopen(pFile,"rb")) == NULL)
	{
		return -1;
	}
	if((cipher = fopen(cFile, "wb")) == NULL)
	{
		return -3;
	}
	//Setup the key
	memcpy(keyBlock, key, 8);
	//Convert the key into binary bits
	CharToBit(keyBlock, binKey);
	//Generate the sub-key
	GenSubKey(binKey, subKey);

	while(!feof(plain))
	{
		//Read 8 bytes everytime, and return the number of the bytes had been read
		if((count = fread(plainBlock, sizeof(char), 8, plain)) == 8)
		{
			DES_EncryptBlock(plainBlock, subKey, cipherBlock);
			fwrite(cipherBlock, sizeof(char), 8, cipher);
		}
	}

	if(count)
	{
		//Fill the blanks
		memset(plainBlock + count, '\0', 7 - count);
		//Save the last char include the number of chars filled
		plainBlock[7] = 8 - count;
		DES_EncryptBlock(plainBlock, subKey, cipherBlock);
		fwrite(cipherBlock, sizeof(char), 8, cipher);
	}

	fclose(plain);
	fclose(cipher);
	return 1;
}

//Decrypt the file
int DES_Decrypt(char *cipher, char *key, char *plain);
