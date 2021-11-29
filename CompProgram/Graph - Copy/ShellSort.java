import java.util.Random;

public class ShellSort {

    int sort(int arr[])
    {
        int n = arr.length;
        for (int gap = n/2; gap > 0; gap /= 2)
        {
            for (int i = gap; i < n; i += 1)
            {
                int temp = arr[i];
                int j;
                for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                    arr[j] = arr[j - gap];
                }
                arr[j] = temp;
            }
        }
        return 0;
    }

    void printArray(int arr[])
    {
        int n = arr.length;
        System.out.print("| ");
        for (int i=0; i<n; ++i) {
            System.out.print(arr[i] + " | ");
        }
        System.out.println();
    }

    public static void main(String[] args) {
        Random rng = new Random();
        int min = 1;
        int max = 1000;
        int upperBound = max - min + 1;
        int[] array = new int[20];
        for(int i=0;i<20;i++) {
            int num = min + rng.nextInt(upperBound);
            array[i] = num;
        }

        ShellSort shell = new ShellSort();
        System.out.println("Before Sorting");
        shell.printArray(array);
        shell.sort(array);
        System.out.println("After Sorting");
        shell.printArray(array);

        for(int i=0;i<19;i++) {
            if(array[i] <= array[i+1]) {
                continue;
            } else {
                throw new RuntimeException("Not sorted in ascending order correctly.");
            }
        }
    }
}