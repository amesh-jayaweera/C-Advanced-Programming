#include <stdio.h>
#include <stdlib.h>

unsigned *intToBinary(unsigned num,unsigned inputBits);
unsigned binaryToInt(unsigned *ptr,unsigned bits);

int main(int argc, char** argv)
{
    if(argc < 5) {
        printf("Required command line args :- input bits, output bits, addend 1, addend 2\n");
        return 0;
    }

    unsigned inputBits = atoi(argv[1]);
    unsigned outputBits = atoi(argv[2]);
    unsigned addend1 = atoi(argv[3]);
    unsigned addend2 = atoi(argv[4]);

    // printf("%d and %d and %d and %d\n",inputBits, outputBits, addend1, addend2);

    unsigned* addend1InBinary = intToBinary(addend1,inputBits);
    unsigned* addend2InBinary = intToBinary(addend2,inputBits);
    unsigned addednd1AfterTurnCat = binaryToInt(addend1InBinary,inputBits);
    unsigned addednd2AfterTurnCat = binaryToInt(addend2InBinary,inputBits);
    unsigned originalSum = addednd1AfterTurnCat + addednd2AfterTurnCat;
    unsigned *output = intToBinary(originalSum,outputBits);
    unsigned turncatedSum = binaryToInt(output,outputBits);

    if(originalSum != turncatedSum) {
        printf("OVERFLOW\n");
    } else {
        // print output
        for(int i=0;i<outputBits;i++) {
            printf("%u",output[i]);
        }
        printf("\n");
    }

    return 0;
}

unsigned* intToBinary(unsigned num,unsigned bits) {
    unsigned i;
    unsigned *ptr = malloc(bits * sizeof *ptr);
    unsigned index = 0;

    for (i = 1 << (bits-1); i > 0; i = i / 2) {
        if((num & i)) {
            ptr[index] = 1;
        } else {
            ptr[index] = 0;
        }
        index++;
    }
    return ptr;
}

unsigned binaryToInt(unsigned *ptr,unsigned bits) {
    unsigned p = 1;
    unsigned sum = 0;
    for(int i=bits-1;i>=0;i--) {
        sum += ptr[i] * p;
        p = p * 2;
    }
    return sum;
}