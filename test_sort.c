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
            printf("-----------\n");
            printf("Round %d\n", i);
            if(arr1[l_ptr] < arr2[r_ptr]){
                printf("%d < %d and %d < %d\n", arr1[l_ptr], arr2[r_ptr], l_ptr, len1);

                printf("putting %d in slot %d\n", arr1[l_ptr], i);
                res_arr[i] = arr1[l_ptr];

                l_ptr++;

            }
            else{
                printf("%d >= %d and %d < %d\n", arr1[l_ptr], arr2[r_ptr], r_ptr, len2);

                printf("putting %d in slot %d\n", arr2[r_ptr], i);
                res_arr[i] = arr2[r_ptr];

                r_ptr++;

            }

        }
        else{
            if(l_ptr == len1){
                for (int j = r_ptr; j < len2; ++j)
                {
                    printf("dumping %d into slot %d\n", arr2[j], i);
                    res_arr[i++] = arr2[j];
                }
            }
            else if(r_ptr == len2){
                for (int j = l_ptr; j < len1; ++j)
                {
                    printf("dumping %d into slot %d\n", arr1[j], i);
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

    int arr[] = {9, 2, 1, 8, 8, 5, 3, 5, 1, 5, 7, 11, 99};

    int len = sizeof(arr) / sizeof(int);

    int mid = len / 2;
    int rem = len - mid;

    printf("mid = %d\n", mid);
    printf("rem = %d\n", rem);

    int *arr1 = malloc(mid * sizeof(int));
    int *arr2 = malloc(rem * sizeof(int));

    // int *arr2 = arr + mid;

    // printf("arr2[0] = %d\n", arr2[0]);

    //mid = len(arr1); rem = len(arr2)

    // void* shmem = create_shared_mem(mid * sizeof(arr));

    // memcpy(shmem, arr, sizeof(arr));

    split(arr, arr1, arr2, mid, len);

    // printf("result is %d\n", mid);
    // printf("remainder is %d\n", rem);
    // printf("value of arr + mid = %d\n", *arr2);

    puke(arr1, mid);
    puke(arr2, rem);

    int pid;

    bubbly(arr1, mid);
    bubbly(arr2, rem);

    /*pid = fork();

    if(pid < 0){
        perror("fork failed");
        exit(1);
    }

    else if(pid == 0){

        printf("sorting in child\n");

        // bubbly(arr1, mid);
        // memcpy(shmem, arr, sizeof(arr));

        exit(1);
    }

    else if(pid > 0){

        printf("sorting in parent\n");

        // bubbly(arr2, rem);

        wait(NULL);
    }*/

    printf("first array after sort:\n");
    puke(arr1, mid);

    printf("second array after sort:\n");
    puke(arr2, rem);

    //mid is length of arr1, rem is length of arr2

    int *res_arr = malloc(len * sizeof(int));

    merge_arr(res_arr, arr1, arr2, mid, rem);

    puke(res_arr, len);

    free(res_arr);

    return 0;
}