#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct directive {
    char gate[17];
    int n;
    int s;
    int *inputs;
    int *outputs;
    int *selectors;
};

struct output {
    char input[100];
    char output[100];
};

int incrementInputs(int *arr, int icount) {
    int i;
    for (i = icount + 1; i >= 2; i--) {
        arr[i] = !arr[i];
        if (arr[i] == 1) {
            return 1;
        }
    }
    return 0;
}

void NOT(int *values, int iindex, int oindex) {
    values[oindex] = !values[iindex];
}

void AND(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = values[iindex1] && values[iindex2];
}

void OR(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = values[iindex1] || values[iindex2];
}

void NAND(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = !(values[iindex1] && values[iindex2]);
}

void NOR(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = !(values[iindex1] || values[iindex2]);
}

void XOR(int *values, int iindex1, int iindex2, int oindex) {
    values[oindex] = values[iindex1] ^ values[iindex2];
}

void PASS(int *values, int iindex, int oindex) {
    values[oindex] = values[iindex];
}

void DECODER(int *values, int n, int *iindex, int *oindex) {
    int s = 0;
    int i;
    for (i = 0; i < pow(2, n); i++) {
        values[oindex[i]] = 0;
    }
    for (i = 0; i < n; i++) {
        s += values[iindex[i]] * pow(2, n - i - 1);
    }
    values[oindex[s]] = 1;
}

void MUX(int *values, int n, int *iindex, int *sindex, int oindex) {
    int s = 0;
    int i;

    for (i = 0; i < n; i++) {
        if(sindex[i] != -1) {
            s += values[sindex[i]] * pow(2, n - i - 1);
        }
    }
    values[oindex] = values[iindex[s]];
}

void printNames(int size, char **arr) {
    int i;
    for (i = 0; i < size; i++) {
        printf("%s\n", arr[i]);
    }
}

int indexOf(int size, char **arr, char *var) {
    int i;
    for (i = 0; i < size; i++) {
        if (strcmp(arr[i], var) == 0) {
            return i;
        }
    }
    return -1;
}

void resetValues(int size, int *arr) {
    int i;
    for (i = 0; i < size; i++) {
        arr[i] = 0;
    }
    arr[1] = 1;
}

int main(int argc, char** argv) {

    if (argc - 1 != 1) {
        printf("Invalid number of arguments\n");
        return 0;
    }

    FILE *fp2;

    fp2 = fopen(argv[1],"r");

    int NN_1;
    char NN_temp[200];
    char NN_temp1[200];
    char NN_temp2[200];
    fscanf(fp2, "%d\n", &NN_1);

    for(int i=0;i<NN_1;i++) {
        fscanf(fp2, "%[^\n]%*c\n", NN_temp);
    }

    int NN_X;
    int NN_COUNT = 0;
    int NN_INPUTS_COUNT = 0;
    while (fscanf(fp2, "%d\n", &NN_X) == 1)
    {
        NN_COUNT++;
    }

    // create array with outputs
    int NN_OUTPUTS[NN_COUNT];
    rewind(fp2);

    fscanf(fp2, "%d\n", &NN_1);

    for(int i=0;i<NN_1;i++) {
        fscanf(fp2, "%s %s %[^\n]%*c\n",NN_temp1, NN_temp2, NN_temp);
        if(strcmp(NN_temp1, "INPUTVAR\0") == 0) {
            NN_INPUTS_COUNT = atoi(NN_temp2);
        }
    }

    int NN_index = 0;
    while (fscanf(fp2, "%d\n", &NN_X) == 1) {
        NN_OUTPUTS[NN_index] = NN_X;
        NN_index++;
    }
    
    rewind(fp2);

    int NN_OUTPUTS_LEN = sizeof(NN_OUTPUTS) / sizeof(NN_OUTPUTS[0]);

    FILE *file;
    remove("testtemp.txt");
    file = fopen("testtemp.txt" ,"a");

    if(!file){
        printf("cannot read tempory created file");
        return 0;
    }

    // write INPUTVAR
    fscanf(fp2, "%d\n", &NN_1);
    for(int i=0;i<NN_1;i++) {
        fscanf(fp2, "%s %s %[^\n]%*c\n",NN_temp1, NN_temp2, NN_temp);
        if(strcmp(NN_temp1, "INPUTVAR\0") == 0) {
            fprintf(file,"%s %s %s\n",NN_temp1,NN_temp2,NN_temp);
            break;
        }
    }
    while (fscanf(fp2, "%d\n", &NN_X) == 1);
    rewind(fp2);

    // write OUTPUTVAR
    fscanf(fp2, "%d\n", &NN_1);
    for(int i=0;i<NN_1;i++) {
        fscanf(fp2, "%s %s %[^\n]%*c\n",NN_temp1, NN_temp2, NN_temp);
        if(strcmp(NN_temp1, "OUTPUTVAR\0") == 0) {
            fprintf(file,"%s %s %s\n",NN_temp1,NN_temp2,NN_temp);
            break;
        }
    }
    while (fscanf(fp2, "%d\n", &NN_X) == 1);
    rewind(fp2);

    // write other commands
    fscanf(fp2, "%d\n", &NN_1);
    for(int i=0;i<NN_1;i++) {
        fscanf(fp2, "%s %s %[^\n]%*c\n",NN_temp1, NN_temp2, NN_temp);
        if(strcmp(NN_temp1, "OUTPUTVAR\0") != 0 && strcmp(NN_temp1, "INPUTVAR\0") != 0) {
            fprintf(file,"%s %s %s\n",NN_temp1,NN_temp2,NN_temp);
        }
    }
    while (fscanf(fp2, "%d\n", &NN_X) == 1);
    fclose(fp2);
    fclose(file);

    file = fopen("testtemp.txt", "r");

    //make steps of circuit with struct directive
    int scount = 0;
    struct directive* steps = NULL;
    int size = 2;
    int icount = 0;
    int ocount = 0;
    int tcount = 0;
    char dir[17];
    char **names;
    int *values = malloc(size * sizeof(int));

    //get INPUT info
    fscanf(file, " %s", dir);
    fscanf(file, "%d", &icount);

    size += icount;
    names = malloc(size * sizeof(char *));
    names[0] = malloc(2 * sizeof(char));
    names[0] = "0\0";
    names[1] = malloc(2 * sizeof(char));
    names[1] = "1\0";

    int i;
    for (i = 0; i < icount; i++) {
        names[i + 2] = malloc(17 * sizeof(char));
        fscanf(file, "%*[: ]%16s", names[i + 2]);
    }

    //get OUTPUT info
    fscanf(file, " %s", dir);
    fscanf(file, "%d", &ocount);
    size += ocount;
    names = realloc(names, size * sizeof(char *));
    for (i = 0; i < ocount; i++) {
        names[i + icount + 2] = malloc(17 * sizeof(char));
        fscanf(file, "%*[: ]%16s", names[i + icount + 2]);
    }


    // outputs
    int OUTPUTS_COUNT = pow(2,NN_INPUTS_COUNT);
    struct output outputs[OUTPUTS_COUNT];
    int output_index = 0;

    //get STEPS
    while (!feof(file)) {
        int numInputs = 2, numOutputs = 1;
        struct directive step;
        int sc = fscanf(file, " %s", dir); 
        if (sc != 1) {
            break;
        }
        scount++;
        step.n = 0;
        step.s = 0;
        strcpy(step.gate, dir);

        if (strcmp(dir, "NOT") == 0) {
            numInputs = 1;
        }
        if (strcmp(dir, "PASS") == 0) {
            numInputs = 1;
        }
        if (strcmp(dir, "DECODER") == 0) {
            fscanf(file, "%d", &numInputs);
            step.n = numInputs;
            numOutputs = pow(2, numInputs);
        }
        if (strcmp(dir, "MULTIPLEXER") == 0) {
            fscanf(file, "%d", &numInputs);
            step.n = numInputs;
            step.s = log(numInputs)/ log(2);
            numOutputs = 1;
        }

        step.inputs = malloc(numInputs * sizeof(int));
        step.outputs = malloc(numOutputs * sizeof(int));
        step.selectors = malloc(step.s * sizeof(int));

        char v[17];
        for (i = 0; i < numInputs; i++) {
            fscanf(file, "%*[: ]%16s", v);
            step.inputs[i] = indexOf(size, names, v);
            // if (strcmp(dir, "MULTIPLEXER") == 0) {
            //     printf("INPUT : %s\n", v);
            // }
        }

        for (i = 0; i < step.s; i++) {
            fscanf(file, "%*[: ]%16s", v);
            step.selectors[i] = indexOf(size, names, v);
            //printf("SELECTOR : %s\n", v);
        }
 
        for (i = 0; i < numOutputs; i++) {
            fscanf(file, "%*[: ]%16s", v);
            // if (strcmp(dir, "MULTIPLEXER") == 0) {
            //     printf("OUTPUT : %s\n", v);
            // }
            int idx = indexOf(size, names, v);
            if (idx == -1) {
                size++;
                tcount++;
                names = realloc(names, size * sizeof(char *));
                names[size - 1] = malloc(17 * sizeof(char));
                strcpy(names[size - 1], v);
                step.outputs[i] = size - 1;
            }
            else {
                step.outputs[i] = idx;
            }
        }
        
        //add step to list of steps
        if (!steps) {
            steps = malloc(sizeof(struct directive));
        } else {
            steps = realloc(steps, scount * sizeof(struct directive));
        }
        steps[scount - 1] = step;
    }

    // exit(0);

    // initialize values array
    values = malloc(size * sizeof(int));
    resetValues(size, values);

    while(1 < 2) {
        //print inputs
        char temp_input[100];
        for (i = 0; i < icount; i++) {
           temp_input[i] = (char) values[i + 2] + 48;
            // printf("%d", values[i + 2]);
        }

        temp_input[i] = '\0';

        //run through steps, calculate outputs
        for (i = 0; i < scount; i++) {
            struct directive step = steps[i];
            if (strcmp(step.gate, "NOT") == 0) {
                NOT(values, step.inputs[0], step.outputs[0]);
            }
            if (strcmp(step.gate, "AND") == 0) {
                AND(values, step.inputs[0], step.inputs[1], step.outputs[0]);
            }
            if (strcmp(step.gate, "OR") == 0) {
                OR(values, step.inputs[0], step.inputs[1], step.outputs[0]);
            }
            if (strcmp(step.gate, "XOR") == 0) {
                XOR(values, step.inputs[0], step.inputs[1], step.outputs[0]);
            }
            if (strcmp(step.gate, "DECODER") == 0) {
                DECODER(values, step.n, step.inputs, step.outputs);
            }
            if (strcmp(step.gate, "MULTIPLEXER") == 0) {
                MUX(values, step.s, step.inputs, step.selectors, step.outputs[0]);
            }
        }

        char temp_output[200];
        int test_i = 0;
        //print outputs
        for (i = 0; i < ocount; i++) {
            // printf("%d", values[icount + i + 2]);
            temp_output[test_i] = (char) values[icount + i + 2] + 48;
            test_i++;
            temp_output[test_i] = '\t';
            test_i++;
        }
        // printf("\n");
        temp_output[test_i] = '\0';

        strcpy(outputs[output_index].input, temp_input);
        strcpy(outputs[output_index].output, temp_output);

        // printf("%d == %d\n", output_index, icount);

        output_index++;

        if (!incrementInputs(values, icount) || output_index == OUTPUTS_COUNT) {
            break;
        }
    }

    // end of all calculations

    for(int i=0;i<NN_OUTPUTS_LEN;i++) {
        char ptr[100];
        unsigned index = 0;
        int num = NN_OUTPUTS[i];

        for (int i = 1 << (NN_INPUTS_COUNT-1); i > 0; i = i / 2) {
            if((num & i)) {
                ptr[index] = '1';
            } else {
                ptr[index] = '0';
            }
            index++;
        }
        ptr[index] = '\0';

        for(int j=0;j<OUTPUTS_COUNT;j++) {
            if(strcmp(outputs[j].input,ptr) == 0) {
                printf("%s\n", outputs[j].output);
                break;
            }
        }
    }

    return 0;
}
