#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>

void* create_shared_mem(size_t size){

  int protection = PROT_READ | PROT_WRITE;

  int visibility = MAP_ANONYMOUS | MAP_SHARED;

  return mmap(NULL, size, protection, visibility, 0, 0);
}

void merge_arr(int *res_arr, int *arr1, int *arr2, int len1, int len2){

    int total = len1 + len2;

    int l_ptr = 0, r_ptr = 0;

    for (int i = 0; i < total; i++)
    {
        if(l_ptr < len1 && r_ptr < len2){
            // printf("-----------\n");
            // printf("Round %d\n", i);
            if(arr1[l_ptr] < arr2[r_ptr]){
                // printf("%d < %d and %d < %d\n", arr1[l_ptr], arr2[r_ptr], l_ptr, len1);

                // printf("putting %d in slot %d\n", arr1[l_ptr], i);
                res_arr[i] = arr1[l_ptr];

                l_ptr++;

            }
            else{
                // printf("%d >= %d and %d < %d\n", arr1[l_ptr], arr2[r_ptr], r_ptr, len2);

                // printf("putting %d in slot %d\n", arr2[r_ptr], i);
                res_arr[i] = arr2[r_ptr];

                r_ptr++;

            }

        }
        else{
            if(l_ptr == len1){
                for (int j = r_ptr; j < len2; ++j)
                {
                    // printf("dumping %d into slot %d\n", arr2[j], i);
                    res_arr[i++] = arr2[j];
                }
            }
            else if(r_ptr == len2){
                for (int j = l_ptr; j < len1; ++j)
                {
                    // printf("dumping %d into slot %d\n", arr1[j], i);
                    res_arr[i++] = arr1[j];
                }
            }
        }

    }
}

void swap_elem(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

void split(int *arr, int *arr1, int *arr2, int arr1_len, int arr_2len){
    for (int i = 0; i < arr1_len; ++i)
    {
        arr1[i] = arr[i];
    }
    for (int i = 0; i < arr_2len; ++i)
    {
        arr2[i] = arr[arr1_len + i];
    }
}

void puke(int *arr, int end){
    for (int i = 0; i < end; ++i)
    {
        printf("%d \n", arr[i]);
    }
    printf("\n");
}

void bubbly(int *arr, int end){
    printf("entered bubbly\n");
    for (int i = 0; i < end - 1; ++i)
    {
        for (int j = 0; j < (end - i - 1); ++j)
        {
            if(arr[j] > arr[j + 1]){
                // printf("SWAPPING arr[%d] with arr[%d]\n", j, j+1);
                swap_elem(&arr[j], &arr[j + 1]);

            }
        }
    }
}

const int size = 13;

int main(int argc, char const *argv[])
{

    int arr[] = {9, 2, 1, 8, 8, 5, 55, 3, 5, 1, 5, 7, 11, 99};

    printf("How many processes should be run (1, 2, 4, or 8?)\n");

    int choice = 0;

    while(choice != 1 && choice != 2 && choice != 4 && choice != 8){
        scanf("%d", &choice);
    }

    printf("choice = %d\n", choice);
    exit(1);

    if(choice == 2 || choice == 4){
        //split original array
        int len = sizeof(arr) / sizeof(int);
        int mid = len / 2;
        int rem = len - mid;

        int *arr1 = create_shared_mem(mid * sizeof(int));
        int *arr2 = create_shared_mem(rem * sizeof(int));

        split(arr, arr1, arr2, mid, rem);

        // puke(arr1, mid);
        // puke(arr2, rem);

        printf("-------------------\n");
    }

    /*----------------------------------------------*/

    if(choice == 4){

        //split first half of new array
        int len2 = mid;
        // printf("len is %d\n", len);
        int mid2 = len2 / 2;
        // printf("mid is %d\n", mid);
        int rem2 = len2 - mid2;
        // printf("rem is %d\n", rem);

        int *arr3 = create_shared_mem(mid2 * sizeof(int));
        int *arr4 = create_shared_mem(rem2 * sizeof(int));

        split(arr1, arr3, arr4, mid2, rem2);

        printf("array 3 is:\n");
        puke(arr3, mid2);
        printf("array 4 is:\n");
        puke(arr4, rem2);

        /*----------------------------------------------*/

         //split second half of new array
        int len3 = rem;
        int mid3 = len3 / 2;
        int rem3 = len3 - mid3;

        int *arr5 = create_shared_mem(mid3 * sizeof(int));
        int *arr6 = create_shared_mem(rem3 * sizeof(int));

        split(arr2, arr5, arr6, mid3, rem3);

        printf("array 5 is:\n");
        puke(arr5, mid3);
        printf("array 6 is:\n");
        puke(arr6, rem3);

        printf("---------------\n");

    }

    // puke(arr1, mid);
    // puke(arr2, rem);

    int pid, pid1;

    // bubbly(arr1, mid);
    // bubbly(arr2, rem);

    if(choice == 4){

        for (int i = 0; i < choice; ++i)
        {
            pid = fork();

            if(pid < 0){
                perror("fork failed");
                exit(-1);
            }

            else if(pid == 0){

                    if(i == 0){
                        bubbly(arr3, mid2);
                    }
                    else if (i == 1){
                        bubbly(arr4, rem2);
                    }
                    else if (i == 2){
                        bubbly(arr5, mid3);
                    }
                    else{
                        bubbly(arr6, rem3);
                    }

                exit(1);
            }

            else if(pid > 0){
                if(choice == 2){
                    bubbly(arr2, rem);
                }
                wait(NULL);
            }
        }
    }

    if(choice == 2){

        if(pid < 0){
            perror("fork failed");
            exit(-1);
        }

        else if(pid == 0){

            printf("sorting in child\n");

                bubbly(arr3, mid2);

                pid1 = fork();

                if(pid < 0){
                    perror("fork failed");
                    exit(-1);
                }

                else if(pid == 0){
                    printf("sorting in second child\n");

                    bubbly(arr4, rem2);
                }

                else if(pid > 0){
                    wait(NULL);
                }

            bubbly(arr5, mid3);


            exit(1);
        }

        else if(pid > 0){

            printf("sorting in parent\n");

            bubbly(arr6, rem3);

            wait(NULL);
        }
    }

    printf("first array after sort:\n");
    puke(arr3, mid2);

    printf("second array after sort:\n");
    puke(arr4, rem2);

    printf("third array after sort:\n");
    puke(arr5, mid3);

    printf("fourth array after sort:\n");
    puke(arr6, rem3);

    //mid is length of arr1, rem is length of arr2

    /*int *res_arr = malloc(len * sizeof(int));

    merge_arr(res_arr, arr1, arr2, mid, rem);

    puke(res_arr, len);

    free(res_arr);*/

    int *res_arr1 = malloc((mid2 + rem2) * sizeof(int));

    merge_arr(res_arr1, arr3, arr4, mid2, rem2);

    int *res_arr2 = malloc((mid3 + rem3) * sizeof(int));

    merge_arr(res_arr2, arr5, arr6, mid3, rem3);

    int *fin_arr = malloc((mid2 + rem2 + mid3 + rem3) * sizeof(int));

    merge_arr(fin_arr, res_arr1, res_arr2, (mid2 + rem2), (mid3 + rem3));

    printf("final array\n");
    puke(fin_arr, (mid2 + rem2 + mid3 + rem3));

    return 0;
}