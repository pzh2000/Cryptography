//des.h
//Header file for implementing DES
#ifndef DES_H_
#define DES_H_

//Convert Byte into binary bits
int ByteToBit(char ch, char bin[8]);

//Convert binary bits into Byte
int BitToByte(char bin[8], char *ch);

//Convert 8 Byte string into 64 bit binary numbers
int CharToBit(char str[8], char bin[64]);

//Convert 64 bit binary numbers into 8 Byte string
int BitToChar(char bin[64], char str[8]);

//Generate sub-keys
int GenSubKey(char key[64], char subkey[16][48]);

//Implementation of Permuted Choice 1
int DES_PC1(char key[64], char key_pc1[56]);

//Implementaton of Permuted Choice 2
int DES_PC2(char key[56], char key_pc2[48]);

//Left circular shift
int Left_Cir_Shift(char key[56], int t);

//Initial permutation
int DES_IP(char data[64]);

//Inverse of inital permutation
int DES_IP_inv(char data[64]);

//Expansion
int DES_Exp(char data[32]);

//Permutation in the F function
int DES_Per(char data[32]);

//XOR
int DES_XOR(char R[48], char L[48], int count);

//S-Box permutation
int DES_SBox(char data[48]);

//Swap function
int DES_Swap(char L[32], char R[32]);

//Encrypt a single block
int DES_EncryptBlock(char plain[8], char subKey[16][48], char cipher[8]);

//Decrypt a single block
int DES_DecryptBlock(char cipher[8], char subKey[16][48], char plain[8]);

//Encrypt the file
int DES_Encrypt(char *plain, char *key, char *cipher);

//Decrypt the file
int DES_Decrypt(char *cipher, char *key, char *plain);

#endif