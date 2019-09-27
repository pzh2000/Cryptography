#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//PC-1 transforms the key from 64 bits to 56 bits by dropping 8 bits and permuting it 
//Drop bits: 8, 16, 24, 32, 40, 48, 56, 64
static const int key_pc1[56] = {
    57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
    10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
    14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4 
};

//Define the number of rotations for each position in the input key. Done to compute subkeys
static const int rotationPositions[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

//PC-2 for computing 48 bit subkey: drop 8 bits (from 56 bits) and permute the rest 
static const int key_pc2[48] = {
    14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

//Initial permutation for 64 bit plain text
static const int initialPerm[64] = {
 
   58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12,  4,
   62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16,  8,
   57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11,  3,
   61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15,  7
 
};

//E-box permutation: Expansion of bits from 32 to 48 / DIFFUSION ELEMENT
static const int dataExpansionPerm[48] = { 
   32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
    8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
   16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
   24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
};

//S box permutations. These are fixed! / CONFUSION ELEMENT
static const int desSbox[8][4][16] = {
 
   {
   {14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
   { 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
   { 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
   {15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13},
   },
 
   {
   {15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
   { 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
   { 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
   {13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9},
   },
 
   {
   {10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
   {13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
   {13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
   { 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12},
   },
 
   {
   { 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
   {13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
   {10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
   { 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14},
   },
 
   {
   { 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
   {14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
   { 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
   {11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3},
   },
 
   {
   {12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
   {10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
   { 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
   { 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13},
   },
 
   {
   { 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
   {13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
   { 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
   { 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12},
   },
 
   {
   {13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
   { 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
   { 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
   { 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11},
   },
 
};

// The permutation for P-Box 
static const int pBoxPermutation[32] = { 
   16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10,
    2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25 
};

//Final permutation for 64 bit encrypted text
static const int finalPerm[64] = {
 
   40,  8, 48, 16, 56, 24, 64, 32, 39,  7, 47, 15, 55, 23, 63, 31,
   38,  6, 46, 14, 54, 22, 62, 30, 37,  5, 45, 13, 53, 21, 61, 29,
   36,  4, 44, 12, 52, 20, 60, 28, 35,  3, 43, 11, 51, 19, 59, 27,
   34,  2, 42, 10, 50, 18, 58, 26, 33,  1, 41,  9, 49, 17, 57, 25
 
};

static char *permuted_subkeys[16];

char *asciiToBinary(char *text)
{
    unsigned char strH[200];
    int i,j;
     
    /*set strH with nulls*/
    memset(strH,0,sizeof(strH));
     
    /*converting str character into Hex and adding into strH*/
    for(i=0,j=0;i<strlen(text);i++,j+=2)
    { 
        sprintf((char*)strH+j,"%02X",text[i]);
    }
    strH[j]='\0'; /*adding NULL in the end*/
     
    // printf("Hexadecimal converted string is: \n");
    // printf("%s\n",strH);

    char *bin_str = (char *)malloc(1000*sizeof(char));
    int k = 0;

    /* Extract first digit and find bin_strary of each hex digit */
    for(k=0; strH[k]!='\0'; k++)
    {
        switch(strH[k])
        {
            case '0':
                strcat(bin_str, "0000");
                break;
            case '1':
                strcat(bin_str, "0001");
                break;
            case '2':
                strcat(bin_str, "0010");
                break;
            case '3':
                strcat(bin_str, "0011");
                break;
            case '4':
                strcat(bin_str, "0100");
                break;
            case '5':
                strcat(bin_str, "0101");
                break;
            case '6':
                strcat(bin_str, "0110");
                break;
            case '7':
                strcat(bin_str, "0111");
                break;
            case '8':
                strcat(bin_str, "1000");
                break;
            case '9':
                strcat(bin_str, "1001");
                break;
            case 'a':
            case 'A':
                strcat(bin_str, "1010");
                break;
            case 'b':
            case 'B':
                strcat(bin_str, "1011");
                break;
            case 'c':
            case 'C':
                strcat(bin_str, "1100");
                break;
            case 'd':
            case 'D':
                strcat(bin_str, "1101");
                break;
            case 'e':
            case 'E':
                strcat(bin_str, "1110");
                break;
            case 'f':
            case 'F':
                strcat(bin_str, "1111");
                break;
            default:
                printf("Invalid hexadecimal input.");
        }
    }

    // printf("Hexademial number = %s\n", strH);
    // printf("binary in func = %s\n", bin_str);
     
    return bin_str;
}



void getArgs (int argc, char **argv){
   int idx = 0;

   for (idx = 1; idx < argc;  idx++) {
        printf("index: %d, %s \n", idx, argv[idx]);
    // index: 1, -e 
    // index: 2, -i 
    // index: 3, plain.txt 
    // index: 4, -o 
    // index: 5, enc.txt 
    // index: 6, -k 
    // index: 7, key.txt 

    }
}

char *loadData(char *fileName)
{
    printf("filename is: %s\n", fileName);
    FILE *file;
    size_t n = 0;
    int c;
    file = fopen(fileName, "r");
    if(file == NULL)
        return NULL;
    char *data = malloc(1000);

    while ((c = fgetc(file)) != EOF)
    {
        data[n++] = (char) c;
    }

    // don't forget to terminate with the null character
    data[n] = '\0';        
    return data;
}

void writeFile(char *fileName, char *text) {
    printf("filename is: %s\n", fileName);
    FILE *f = fopen(fileName, "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    /* print some text */
    // const char *temp = "Write this to the file";
    fprintf(f, "%s", text);
    fclose(f);
}

void rotate_left(char bits_array[], int arr_len, int pos) {
    char extra;
    int i,j;
    for(i=0;i<pos;i++) {
        extra = bits_array[0];
        for(j=0;j<arr_len-1;j++) {
            bits_array[j] = bits_array[j+1];
        }
        bits_array[arr_len-1] = extra;
    }
}

void binary_xor(char *bin1, char *bin2, char *xorred, int size) {
    int i, res;
    int len1 = strlen(bin1);
    int len2 = strlen(bin2);

    for(i=0;i<size;i++) {
        char tmp1 = bin1[i];
        char tmp2 = bin2[i];
        res = (tmp1 - '0') ^ (tmp2 - '0');
        xorred[i] = (res + '0');
    }
    xorred[size] = '\0';

}

char *decToBinary(int n)
{
    char *binary;
    // array to store binary number
    binary = (char *)calloc(5, sizeof(char));
    binary[0]= binary[1] = binary[2] = binary[3] ='0';
    binary[4]='\0';
    // counter for binary array
    int i = 3;
    while (n > 0) {
 
        // storing remainder in binary array
        binary[i] = n % 2 + '0';
        n = n / 2;
        i--;
    }
 
    // printing binary array in reverse order
    // printf("printing binary array\n");
    // printf("%s\n", binary);
    return binary; 
}


char *computePermutation(char *bits, const int *mapping, int size) {
    char *result = (char *)calloc(size+1, sizeof(char));
    result[size] = '\0';
    // printf("res is initialized as: %s\n", result);
    int i=0;
    for( i=0;i<size;i++) {
        result[i] = bits[mapping[i]-1];
    }
    
    return result;
}

void createSubkeys(char *mainKey) {
    static unsigned char subkeys[16][60];
    
    //The bits in key is reduced to 56 using the PC-1 permutation
    char *permutedKey = computePermutation(mainKey, key_pc1, 56);
    // printf("Final returned permuted string: %s\n", permutedKey);

    //Divide the key into two halves: left_key and right_key
    char left_key[30], right_key[30];
    memset(left_key, '0', 28*sizeof(char));
    memset(right_key, '0', 28*sizeof(char));
    int i=0;
    for(i=0; i<28 ;i++) {
        left_key[i] = permutedKey[i];
        right_key[i] = permutedKey[i+28];
    }
    left_key[28] = '\0'; 
    right_key[28] = '\0'; 
    // printf("Left key is: %s\n", left_key);
    // printf("Right key is: %s\n", right_key);
    int j;

    //Generate 16 subkeys for the feistel network
    for( j =0;j<16;j++) {
        rotate_left(left_key, 28, rotationPositions[j]);
        rotate_left(right_key, 28, rotationPositions[j]);
        int k;
        for (k = 0; k < 28; k++) {
            subkeys[j][k]=left_key[k];
            subkeys[j][k+28] = right_key[k];
        }
        subkeys[j][56]='\0';
        char *str = computePermutation(subkeys[j], key_pc2, 48);
        permuted_subkeys[j] = (char *)malloc((strlen(str) + 1)*sizeof(char));
        strcpy(permuted_subkeys[j], str);
        // printf("Subkey No.: %d, Subkey is: %s \n", j+1, permuted_subkeys[j]);
         
    }
    printf("----------------------------------------------------------------\n");
    for(j=0;j<16;j++) {
        printf("Subkey No.: %d, Subkey is: %s \n", j+1, permuted_subkeys[j]);
    }
}

char *des_util(char *plainText, char flag) {
    char *target;
    char *permutedText;
    char *encrypted;
    char *leftText = (char *)calloc(33, sizeof(char));
    char *rightText= (char *)calloc(33, sizeof(char));
    char *f_rightText; //This the rightText after function f is applied on it
    
    // printf("plainText is: %s\n", plainText);
    permutedText = computePermutation(plainText, initialPerm, 64);
    // printf("permutedText is: %s\n", permutedText);
    // Copy the left 32 bits of 4 bytes of text into leftText and right 32 bits into rightText
    int x=0;
    for(x=0; x<32 ;x++) {
        leftText[x] = permutedText[x];
        rightText[x] = permutedText[x+32];
    }
    leftText[32] = '\0'; 
    rightText[32] = '\0'; 

    // printf("Left 32 bits of data: %s\n", leftText);
    // printf("Ryt 32 bits of data: %s\n", rightText);

    // 16 Encryption rounds using s boxes:
    int i;
    for(i=0;i<16;i++) {
        //f function is implemented only on the rightText:

        //First, apply the expansion permutation on the right side of the data
        f_rightText=computePermutation(rightText, dataExpansionPerm, 48);
        char *xor_result =(char *)calloc(49, sizeof(char));
        xor_result[48] = '\0';
        if(flag =='e') {
            //For encryption, subkeys are applied in increasing order
            // printf("f_rightText: %s\n", f_rightText);
            // printf("subkey: %s\n", permuted_subkeys[i]);
            // printf("xor result:   %s\n", xor_result);
            binary_xor(f_rightText, permuted_subkeys[i], xor_result, 48);
            strcpy(f_rightText, xor_result);
            // printf("after xor: %s\n", xor_result);
            // printf("f_rightText after overwrite: %s\n", f_rightText);
            
        }
        else if(flag == 'd') {
            //For decrytpion, subkeys are applied in decreasing order
            // printf("f_rightText: %s\n", f_rightText);
            // printf("subkey: %s\n", permuted_subkeys[i]);
            // printf("xor result:   %s\n", xor_result);
            binary_xor(f_rightText, permuted_subkeys[15 - i], xor_result, 48);
            strcpy(f_rightText, xor_result);
            // printf("after xor: %s\n", xor_result);
            // printf("f_rightText after overwrite: %s\n", f_rightText);

        }
        

        //Now, we perform the 8 s-box substitutions
        int q = 0;
        int sb, row, col;
        int s_block;
        char *rightText_final = (char *)calloc(33, sizeof(char));
        char *permuted_rightText;
        for(sb=0; sb<8; sb++) {
            char *temp_4bits;
            // temp_4bits = (char *)calloc(5, sizeof(char));
            //Bits for row from positions: 0 and 5
            //Bits for column from positions: 1, 2, 3, 4
            row = ((f_rightText[(sb*6) + 0] - '0')*2) + ((f_rightText[(sb*6) + 5] - '0')*1);
            col = ((f_rightText[(sb*6) + 1] - '0')*8) + ((f_rightText[(sb*6) + 2] - '0')*4) + 
                    ((f_rightText[(sb*6) + 3] - '0')*2) + ((f_rightText[(sb*6) + 4] - '0')*1);
            // printf("Row is: %d, Col is: %d\n", row, col);

            //S-box substitution for the 6 bit block in f_rightText
            s_block = desSbox[sb][row][col];
            // printf("sblock is: %d\n", s_block);
            temp_4bits = decToBinary(s_block);
            // printf("Binary In main: %s\n", temp_4bits); 
            int ind;
            for(ind = 0; ind<4;ind++) {
                int l = (sb*4) + ind;
                // printf("ind: %d\n", l);
                rightText_final[(sb*4) + ind] = temp_4bits[ind];
            }
            // printf("final right text block: %s\n", rightText_final);

        }
        //P-Box permutation: last step of f
        permuted_rightText=computePermutation(rightText_final, pBoxPermutation, 32);    
        char *xor_result2 =(char *)calloc(33, sizeof(char));
        binary_xor(leftText, permuted_rightText, xor_result2, 32);
        // printf("Final xor_result2 is: %s\n", xor_result2);
        //Copy the rightText into leftText and xor_result2 into rightText   
        strcpy(leftText, rightText);
        strcpy(rightText, xor_result2);
        // printf("Final leftText is: %s\n", leftText);
        // printf("Final rightText is: %s\n", rightText);

    } //The 16 Encryption rounds are over------------------------
    //Now, the final text will be the combination of rightText and leftText
    int c;
    char *intermediate_target = (char *)calloc(65, sizeof(char));
    for(c=0;c<32;c++) {
        intermediate_target[c] = rightText[c];
        intermediate_target[c+32] = leftText[c];
    }
    intermediate_target[64] = '\0';
    // printf("intermediate_target: %s\n", intermediate_target);
    //Final permutation: IP inverse
    target=computePermutation(intermediate_target, finalPerm, 64); 
    // printf("target: %s\n", target);
    return target;
}

unsigned long binaryToDecimal(char *binary, int length)
{
    int i;
    unsigned long decimal = 0;
    unsigned long weight = 1;
    int l = length -1;
    binary += l;
    weight = 1;
    for(i = 0; i < length; ++i, --binary)
    {
        if(*binary == '1')
            decimal += weight;
        weight *= 2;
    }
    return decimal;
}


void binaryToText(char *binary, int binaryLength, char *text, int symbolCount)
{
    int i;
    for(i = 0; i < binaryLength; i+=8, binary += 8)
    {
        char *byte = binary;
        byte[8] = '\0';
        *text++ = binaryToDecimal(byte, 8);
        // printf("part string in func: %s\n", text);
    }
    text -= symbolCount;
    // printf("---------------------Text string in func: %s\n", text);
}

int main(int argc, char **argv) {
    getArgs(argc, argv);
    //Read the key.txt file here
    char *key = loadData(argv[7]);
    printf("Key is: %s", key);
    printf("Flag is: %s\n", argv[1]);
    createSubkeys(key);
    if(strcmp(argv[1], "-e") == 0) //ENCRYPTION
    {
        char *plainText = loadData(argv[3]);
        char *binaryData = asciiToBinary(plainText);
        int bin_size = strlen(binaryData);
        int left;
        if(bin_size%64 != 0) {
            left = (64- (bin_size%64))/8;
            int l = 0;
            for(l=0;l<left;l++) {
                strcat(binaryData, "00100000");
            } 
        }
        printf("binary in main = %s\n", binaryData);
        int new_size = strlen(binaryData);
        // char *part=(char *)malloc(65*sizeof(char));
        char *part=(char *)calloc(65, sizeof(char));
        char *encryptedPart;
        char *encryptedText = (char *)malloc((new_size+1)*sizeof(char));
        int chunk_size =64;
        int chunks = new_size / chunk_size;
        int c=0, i=0;
        while(c<chunks){
            char *part=(char *)calloc(chunk_size+1, sizeof(char));
            i=0;
            while(i<chunk_size) {
                part[i] = binaryData[c*chunk_size + i];

                i++;

            }
            part[chunk_size]='\0';
            // printf("chunk: %d, part: %s\n", c, part);
            encryptedPart = des_util(part, 'e');
            strcat(encryptedText,encryptedPart);
            c++;
        }
        
        printf("EncryptedText in main: %s\n", encryptedText);
        writeFile(argv[5], encryptedText);
    }
    else if(strcmp(argv[1], "-d") == 0) //DECRYPTION
    {
        char *bin_encryptedText = loadData(argv[3]);
        int encrypted_size = strlen(bin_encryptedText);
        char *decryptedPart;
        char *decryptedText = (char *)malloc((encrypted_size+1)*sizeof(char));
        int chunk_size =64;
        int chunks = encrypted_size / chunk_size;
        int c=0, i=0;
        while(c<chunks){
            char *part=(char *)calloc(chunk_size+1, sizeof(char));
            i=0;
            while(i<chunk_size) {
                part[i] = bin_encryptedText[c*chunk_size + i];
                i++;

            }
            part[chunk_size]='\0';
            // printf("chunk: %d, part: %s\n", c, part);
            decryptedPart = des_util(part, 'd');
            strcat(decryptedText,decryptedPart);
            c++;
        }
        printf("DecryptedText in main: %s\n", decryptedText);
        int length = strlen(decryptedText);
        int symbolCount = length / 8 + 1;
        char *res_text = (char *)malloc((symbolCount)*sizeof(char));
        binaryToText(decryptedText, length, res_text, symbolCount);
        printf("Decrypted string: %s\n", res_text);
       
        writeFile(argv[5], res_text);
    }
    else
    {
        printf("The flag should either be -e or -d!");
    }
}
