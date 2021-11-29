#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define GRID_SIZE 9

void printGrid(int arr[GRID_SIZE][GRID_SIZE]);
int hasSolution(int grid[GRID_SIZE][GRID_SIZE], int row, int col, int num);
int sudukoSolver(int grid[GRID_SIZE][GRID_SIZE], int row, int col);
 
int main(int argc, char** argv)
{

    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int grid[GRID_SIZE][GRID_SIZE];

    if(argc < 2) {
        printf("Input text file required from command line");
        return 0;
    }

    FILE *fp;
    fp = fopen (argv[1], "r"); // open file in read mode

    if (fp == NULL) {
      perror("Failed to read file");
      return 0;
    }

    // read from input file
    int k = 0;
    int j = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        j = 0;
        for(int i=0;i<strlen(line);i++) {
            if(line[i] == '-') {
                grid[k][j] = 0;
                j++;
            }else if(line[i] == '\t' || line[i] == '\n' || line[i] == '\r' || line[i] == ' ') {
                continue;
            }else {
                grid[k][j] = (int) (line[i]) - 48;
                j++;
            }
            if(j == 9) {
                break;
            }
        }
        k++;
        if(k == 9){
            break;
        }
    }
    
    fclose (fp); // close file
    if (line)
        free(line);

    // printGrid(grid);

    if (sudukoSolver(grid, 0, 0) == 1)
        printGrid(grid);
    else
        printf("no-solution\n");
 
    return 0;
}

void printGrid(int arr[GRID_SIZE][GRID_SIZE])
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
         for (int j = 0; j < GRID_SIZE; j++)
            printf("%d\t",arr[i][j]);
         printf("\n");
    }
}
 
int hasSolution(int grid[GRID_SIZE][GRID_SIZE], int row, int col, int num)
{
    for (int x = 0; x <= 8; x++)
        if (grid[row][x] == num)
            return 0;
    for (int x = 0; x <= 8; x++)
        if (grid[x][col] == num)
            return 0;
    int startRow = row - row % 3,
                 startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[i + startRow][j +
                          startCol] == num)
                return 0;
    return 1;
}
 
int sudukoSolver(int grid[GRID_SIZE][GRID_SIZE], int row, int col)
{
    if (row == GRID_SIZE - 1 && col == GRID_SIZE)
        return 1;
    if (col == GRID_SIZE)
    {
        row++;
        col = 0;
    }
    if (grid[row][col] > 0)
        return sudukoSolver(grid, row, col + 1);
 
    for (int num = 1; num <= GRID_SIZE; num++)
    {
        if (hasSolution(grid, row, col, num)==1)
        {
            grid[row][col] = num;
            if (sudukoSolver(grid, row, col + 1)==1)
                return 1;
        }
        grid[row][col] = 0;
    }
    return 0;
}