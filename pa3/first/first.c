#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <regex.h>

/*
    Struct to repreent Cache
*/
struct Cache
{
    char **tag;
    char *block;
    int valid;
    int replacement;
};

// method declarations
char *substr(char const *input, size_t start, size_t len); 
char *intToBinary(unsigned num,unsigned bits);
char *intToBinaryIn48BitFormat(unsigned long num);

int main(int argc, char* argv[]) 
{
    // check for all command line arguments are present
    if (argc < 5)
    {
        printf("Error : Command line argument should be in -> ./first <cache size><associativity><block size><trace file>\n");
        return 0;
    }

    // regex to check the set associativity 
    regex_t regex;
    int regex_test = regcomp(&regex, "^assoc:.*$", 0);
    // calculate basic params
    unsigned int cacheSize = atoi(argv[1]);
    char* associativity = argv[2];
    int ass_n = 1; // init ass_n with 1 

    regex_test = regexec(&regex, associativity ,0, NULL, 0);

    // set number set associatives if it exits
    if(regex_test == 0) {
        char* num = substr(associativity,6, strlen(associativity)-5);
        ass_n = atoi(num);
        free(num);
    }

    unsigned int blockSize = atoi(argv[3]);
    int offsetBits = log(blockSize) / log(2);
    int setsCount = (cacheSize / blockSize) / ass_n;
    int indexBits = log(setsCount) / log(2);
    int tagBits;
    if(strcmp(associativity,"assoc\0") == 0) {
        tagBits = 48 - offsetBits;
    } else {
        tagBits = 48 - offsetBits - indexBits;
    }
    FILE *tracefile;
    // open tracefile for read
    tracefile = fopen(argv[4], "r");

    // check the file is exists
    if(tracefile == NULL)
    {
        printf("Error: Cannot find file\n");
        return -1;
    }

    // init cache
    struct Cache cache[setsCount];

    if(strcmp(associativity,"direct\0") == 0 || regex_test == 0 || strcmp(associativity,"assoc\0") == 0) {
        for(int i=0;i<setsCount;i++) {
            cache[i].block = intToBinary(i,indexBits);
            cache[i].valid = 0;
            cache[i].replacement = 0;
            cache[i].tag = malloc(ass_n * sizeof(char *));
            for(int j=0; j < ass_n ;j++)
            {
                cache[i].tag[j] = malloc(100 * sizeof(char));
                for(int k=0;k < 48 ; k++) {
                    cache[i].tag[j][k] = '0';
                }
                cache[i].tag[j][48] = '\0';
            }
        }
    } else {
        printf("Unknown Algorithm!\n");
        return 0;
    }

    // params need to be counted
    int reads = 0, writes = 0, hits = 0, misses = 0;

    char mode;
    char hex[50];
    unsigned long n;
    int replacement = 0;
    // read file

    if(strcmp(associativity,"direct\0") == 0 || regex_test == 0) { // direct and assoc:n
        while(fscanf(tracefile, "%c %s\n", &mode, hex) == 2) {
            sscanf(hex,"%lx", &n);
            char *nInBin = intToBinaryIn48BitFormat(n);
            char *tag = substr(nInBin, 0, tagBits);
            int end;
            if(indexBits == 0){
                end = strlen(nInBin);
            } else {
                end = indexBits;
            }
            char *set = substr(nInBin, tagBits, end);
            
            for(int i=0;i<setsCount;i++) {
                // found target block
                if(strcmp(cache[i].block, set) == 0 || indexBits == 0) {
                    
                    int flag = 0;
                    for(int j=0;j<ass_n;j++) {
                        if(cache[i].valid == 1 && strcmp(cache[i].tag[j],tag) == 0) {
                            hits++;
                            if(mode == 'W') {
                                writes++;
                            }
                            flag = 1;
                            break;
                        }                         
                    }

                    if(flag == 0) {
                        cache[i].valid = 1;
                        misses++;

                        if(mode == 'W') {
                            reads++;
                            writes++;
                        }

                        if(mode == 'R') {
                            reads++;
                        }

                        strcpy(cache[i].tag[cache[i].replacement], tag);
                        cache[i].replacement++;
                        if(cache[i].replacement == ass_n) {
                            cache[i].replacement = 0;
                        }
                    }
                    break;
                }
            }

            free(tag);
            free(set);
            free(nInBin);
        }
    } else if(strcmp(associativity,"assoc\0") == 0) { //  assoc
        while(fscanf(tracefile, "%c %s\n", &mode, hex) == 2) {
            sscanf(hex,"%lx", &n);
            char *nInBin = intToBinaryIn48BitFormat(n);
            char *tag = substr(nInBin, 0, tagBits);
            int flag = 0;
            for(int i=0;i<setsCount;i++) {
                if(cache[i].valid == 1 && strcmp(cache[i].tag[0],tag) == 0) {
                    hits++;
                    if(mode == 'W') {
                        writes++;
                    }
                    flag = 1;
                    break;
                }     
            }

            if(flag == 0) {
                cache[replacement].valid = 1;
                misses++;

                if(mode == 'W') {
                    reads++;
                    writes++;
                }

                if(mode == 'R') {
                    reads++;
                }

                strcpy(cache[replacement].tag[0], tag);
                replacement++;
                if(replacement == setsCount) {
                    replacement = 0;
                }
            }

            free(tag);
            free(nInBin);
        }
    }

    fclose(tracefile);

    // display output
    printf("memread:%d\n", reads);
    printf("memwrite:%d\n", writes);
    printf("cachehit:%d\n", hits);
    printf("cachemiss:%d\n", misses);

    return 0;
}

char *intToBinary(unsigned num,unsigned bits) {
    unsigned i;
    char *ptr = malloc((bits + 1) * sizeof *ptr);
    unsigned index = 0;

    for (i = 1 << (bits-1); i > 0; i = i / 2) {
        if((num & i)) {
            ptr[index] = '1';
        } else {
            ptr[index] = '0';
        }
        index++;
    }
    ptr[index] = '\0';
    return ptr;
}

char *substr(char const *input, size_t start, size_t len) { 
    char *ret = malloc(len+1);
    memcpy(ret, input+start, len);
    ret[len]  = '\0';
    return ret;
}

char *intToBinaryIn48BitFormat(unsigned long num) {
    unsigned long i;
    char *ptr = malloc(49 * sizeof *ptr);
    unsigned index = 0;
    for (i = 1UL << 47UL; i > 0; i = i / 2) {
        if((num & i)) {
            ptr[index] = '1';
        } else {
            ptr[index] = '0';
        }
        index++;
    }
    ptr[index] = '\0';
    return ptr;
}