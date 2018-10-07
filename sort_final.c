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

typedef struct{

    char date[2048];
    char season[5];
    int neutral;
    char playoff[6];
    char team1[5];
    char team2[5];
    float elo1_pre;
    float elo2_pre;
    float elo_prob1;
    float elo_prob2;
    float elo1_post;
    float elo2_post;
    float rating1_pre;
    float rating2_pre;
    char pitcher1 [2048];
    char pitcher2 [2048];
    float pitcher1_rgs;
    float pitcher2_rgs;
    float pitcher1_adj;
    float pitcher2_adj;
    float rating_prob1;
    float rating_prob2;
    float rating1_post;
    float rating2_post;
    int score1;
    int score2;

}entry;

void* create_shared_mem(size_t size){

  int protection = PROT_READ | PROT_WRITE;

  int visibility = MAP_ANONYMOUS | MAP_SHARED;

  return mmap(NULL, size, protection, visibility, 0, 0);
}

void merge_arr(entry *res_arr, entry *arr1, entry *arr2, int len1, int len2){

    int total = len1 + len2;

    int l_ptr = 0, r_ptr = 0;

    for (int i = 0; i < total; i++)
    {
        if(l_ptr < len1 && r_ptr < len2){
            // printf("-----------\n");
            // printf("Round %d\n", i);
            if(strcmp(arr1[l_ptr].team1, arr2[r_ptr].team1) <= 0){
                // printf("cond 1\n");
                // printf("%s comes before %s and %d < %d\n", arr1[l_ptr].team1, arr2[r_ptr].team1, l_ptr, len1);

                // printf("putting %s in slot %d\n", arr1[l_ptr].team1, i);
                res_arr[i] = arr1[l_ptr];

                l_ptr++;

            }
            else{
                // printf("cond 2\n");
                // printf("%s comes after %s and %d < %d\n", arr1[l_ptr].team1, arr2[r_ptr].team1, r_ptr, len2);

                // printf("putting %s in slot %d\n", arr2[r_ptr].team1, i);
                res_arr[i] = arr2[r_ptr];

                r_ptr++;

            }

        }
        else{
            if(l_ptr == len1){
                for (int j = r_ptr; j < len2; ++j)
                {
                    // printf("dumping %s into slot %d\n", arr2[j].team1, i);
                    res_arr[i++] = arr2[j];
                }
            }
            else if(r_ptr == len2){
                for (int j = l_ptr; j < len1; ++j)
                {
                    // printf("dumping %s into slot %d\n", arr1[j].team1, i);
                    res_arr[i++] = arr1[j];
                }
            }
        }

    }
}

void swap_elem(entry *a, entry *b){
    entry temp = *a;
    *a = *b;
    *b = temp;
}

void split(entry *arr, entry *arr1, entry *arr2, int arr1_len, int arr_2len){
    for (int i = 0; i < arr1_len; ++i)
    {
        arr1[i] = arr[i];
    }
    for (int i = 0; i < arr_2len; ++i)
    {
        arr2[i] = arr[arr1_len + i];
    }
}

void puke(entry entries[], int num_lines){
    for (int i = 0; i < num_lines; ++i)
    {
        // printf("index is %d\n", i);
        // printf("---------------------------------\n");
        // printf("date is %s\n", entries[i].date);
        // printf("season is %s\n", entries[i].season);
        // printf("neutral is %d\n", entries[i].neutral);
        // printf("playoff is %s\n", entries[i].playoff);
        printf("%d: %s\n", i, entries[i].team1);
        // printf("team2 is %s\n", entries[i].team2);
        // printf("elo1_pre is %f\n", entries[i].elo1_pre);
        // printf("elo2_pre is %f\n", entries[i].elo2_pre);
        // printf("elo_prob1 is %f\n", entries[i].elo_prob1);
        // printf("elo_prob2 is %f\n", entries[i].elo_prob2);
        // printf("elo1_post is %f\n", entries[i].elo1_post);
        // printf("elo2_post is %f\n", entries[i].elo2_post);
        // printf("rating1_pre is %f\n", entries[i].rating1_pre);
        // printf("rating2_pre is %f\n", entries[i].rating2_pre);
        // printf("pitcher1 is %s\n", entries[i].pitcher1);
        // printf("pitcher2 is %s\n", entries[i].pitcher2);
        // printf("pitcher1_rgs is %f\n", entries[i].pitcher1_rgs);
        // printf("pitcher2_rgs is %f\n", entries[i].pitcher2_rgs);
        // printf("pitcher1_adj is %f\n", entries[i].pitcher1_adj);
        // printf("pitcher2_adj is %f\n", entries[i].pitcher2_adj);
        // printf("rating_prob1 is %f\n", entries[i].rating_prob1);
        // printf("rating_prob2 is %f\n", entries[i].rating_prob2);
        // printf("rating1_post is %f\n", entries[i].rating1_post);
        // printf("rating2_post is %f\n", entries[i].rating2_post);
        // printf("score1 is %d\n", entries[i].score1);
        // printf("score2 is %d\n", entries[i].score2);
    }
}

void bubbly(entry *arr, int end){
    printf("entered bubbly\n");
    for (int i = 0; i < end - 1; ++i)
    {
        for (int j = 0; j < (end - i - 1); ++j)
        {
            if(strcmp(arr[j].team1, arr[j + 1].team1) >= 0){

                swap_elem(&arr[j], &arr[j + 1]);

            }
        }
    }
}

const int size = 13;

int main(int argc, char const *argv[])
{

    FILE *file = fopen(argv[1], "r");
    char *value_buffer;

    if(argv[1] == "" || argv[1] == NULL){
        printf("You need to supply at least one argument. Exiting\n");
        exit(-1);
    }

    char c;
    entry *entries; //dynamic array of entries
    int num_lines = 0;

    char *buf;

    size_t size = 99999;

    c = getc(file);

    while(c != EOF){
        if(c == '\n'){
            num_lines++;
        }
        c = getc(file);
    }

    printf("There are %d lines\n", num_lines);

    entries = malloc(num_lines * sizeof(entry));
    buf = malloc(size * sizeof(char));

    if(fseek(file, 0, SEEK_SET)){printf("Failed to seek, exiting\n"); exit(-1);}

    const char delim = ',';

    for (int i = 0; i < num_lines; ++i)
    {
        //put a line in the buffer
        buf = NULL;
        // printf("At index %d\n", i);
        getline(&buf, &size, file);

        strcpy(entries[i].date, strsep(&buf, &delim));
        strcpy(entries[i].season, strsep(&buf, &delim));
        entries[i].neutral = atoi(strsep(&buf, &delim));
        strcpy(entries[i].playoff, strsep(&buf, &delim));
        strcpy(entries[i].team1, strsep(&buf, &delim));
        strcpy(entries[i].team2, strsep(&buf, &delim));
        entries[i].elo1_pre = atof(strsep(&buf, &delim));
        entries[i].elo2_pre = atof(strsep(&buf, &delim));
        entries[i].elo_prob1 = atof(strsep(&buf, &delim));
        entries[i].elo_prob2 = atof(strsep(&buf, &delim));
        entries[i].elo1_post = atof(strsep(&buf, &delim));
        entries[i].elo2_post = atof(strsep(&buf, &delim));
        entries[i].rating1_pre = atof(strsep(&buf, &delim));
        entries[i].rating2_pre = atof(strsep(&buf, &delim));
        strcpy(entries[i].pitcher1, strsep(&buf, &delim));
        strcpy(entries[i].pitcher2, strsep(&buf, &delim));
        entries[i].pitcher1_rgs = atof(strsep(&buf, &delim));
        entries[i].pitcher2_rgs = atof(strsep(&buf, &delim));
        entries[i].pitcher1_adj = atof(strsep(&buf, &delim));
        entries[i].pitcher2_adj = atof(strsep(&buf, &delim));
        entries[i].rating_prob1 = atof(strsep(&buf, &delim));
        entries[i].rating_prob2 = atof(strsep(&buf, &delim));
        entries[i].rating1_post = atof(strsep(&buf, &delim));
        entries[i].rating2_post = atof(strsep(&buf, &delim));
        entries[i].score1 = atoi(strsep(&buf, &delim));
        entries[i].score2 = atoi(strsep(&buf, &delim));
    }

    printf("How many processes should be run (1, 2, 4, or 8?)\n");

    int choice = 0;

    while(choice != 1 && choice != 2 && choice != 4 /*&& choice != 8*/){
        scanf("%d", &choice);
    }

    if (choice == 1){

        time_t start = time(NULL);

        printf("array pre-sort:\n");
        puke(entries, num_lines);

        bubbly(entries, num_lines);

        printf("array post-sort:\n");
        puke(entries, num_lines);

        time_t end = time(NULL);

        time_t delta = end - start;

        printf("Sort took %ld seconds\n", delta);
    }

    if(choice == 2){

        time_t start = time(NULL);

        //split original array
        int len = num_lines;
        int mid = len / 2;
        int rem = len - mid;

        entry *arr1 = create_shared_mem(mid * sizeof(entry));
        entry *arr2 = create_shared_mem(rem * sizeof(entry));

        split(entries, arr1, arr2, mid, rem);

        printf("array 1 is\n");
        puke(arr1, mid);
        printf("array 2 is\n");
        puke(arr2, rem);

        // puke(arr1, mid);
        // puke(arr2, rem);

        printf("-------------------\n");

        int pid = fork();

        if(pid < 0){
            perror("fork failed");
            exit(-1);
        }

        else if(pid == 0){

            printf("sorting in child\n");

            bubbly(arr1, mid);

            exit(1);
        }

        else if(pid > 0){

            printf("sorting in parent\n");

            bubbly(arr2, rem);

            wait(NULL);
        }

        printf("array 1 post sort:\n");
        puke(arr1, mid);
        printf("array 2 post sort:\n");
        puke(arr2, rem);

        entry *res_arr = malloc(num_lines * sizeof(entry));

        printf("mid before merge is : %d\nrem before merge is: %d\n", mid, rem);

        merge_arr(res_arr, arr1, arr2, mid, rem);

        time_t end = time(NULL);

        time_t delta = end - start;

        puke(res_arr, num_lines);

        printf("Sort took %ld seconds\n", delta);

        free(res_arr);
    }

    /*----------------------------------------------*/

    if(choice == 4){
        time_t start = time(NULL);

        //split original array
        int len1 = num_lines / 2;
        int mid1 = len1 / 2;
        int rem1 = len1 - mid1;

        int len2 = num_lines - len1;
        int mid2 = len2 / 2;
        int rem2 = len2 - mid2;

        //willl split the original array into two halves
        //each half is stored in temp1 and temp2
        entry *temp1 = create_shared_mem(len1 * sizeof(entry));
        entry *temp2 = create_shared_mem(len2 * sizeof(entry));

        //the two halves are then split into four quarters
        //which are stored in theses variables
        entry *arr1 = create_shared_mem(mid1 * sizeof(entry));
        entry *arr2 = create_shared_mem(rem1 * sizeof(entry));
        entry *arr3 = create_shared_mem(mid2 * sizeof(entry));
        entry *arr4 = create_shared_mem(rem2 * sizeof(entry));

        //split original array
        split(entries, temp1, temp2, len1, len2);

        //split first half of original array
        split(temp1, arr1, arr2, mid1, rem1);

        //split seccond half of original array
        split(temp2, arr3, arr4, mid2, rem2);

        //by now we have four arrays

        // pid_t *p1 = create_shared_mem(sizeof(pid_t));
        pid_t p1, p2, p3, p4;

        int pid = fork();
        //now we have two processes

        if(pid < 0){
            perror("fork failed");
            exit(-1);
        }

        else if(pid == 0){
            //child process
            p2 = getpid();
            printf("p2 pid is %d\n", p2);

            /*-------------------------------*/

                pid = fork();

                if(pid < 0){
                    perror("fork failed");
                    exit(-1);
                }

                else if (pid == 0){
                    p4 = getpid();
                    printf("p4 pid is %d\n", p4);

                    // exit(1);
                }

                else if(pid > 0){
                    printf("p4 parent id is %d\n", getpid());
                }

            /*-------------------------------*/

            // exit(1);
        }

        else if (pid > 0){
            p1 = getpid();
            printf("p1 pid is %d\n", p1);

            /*-------------------------------*/

                pid = fork();

                if(pid < 0){
                    perror("fork failed");
                    exit(-1);
                }

                else if (pid == 0){
                    p3 = getpid();
                    printf("p3 pid is %d\n", p3);

                    // exit(1);
                }

                else if(pid > 0){
                    printf("p3 parent id is %d\n", getpid());
                }

            /*-------------------------------*/

            wait(NULL);
        }

        printf("---------------------\n");

        if(p1 == getpid()){
            // printf("I am process p1 and my pid is %d\n", getpid());
            printf("sorting in p1\n");
            bubbly(arr1, mid1);
        }
        if(p2 == getpid()){
            // printf("I am process p2 and my pid is %d\n", getpid());
            printf("sorting in p2\n");
            bubbly(arr2, rem1);

            exit(1);
        }
        if(p3 == getpid()){
            // printf("I am process p3 and my pid is %d\n", getpid());
            printf("sorting in p3\n");
            bubbly(arr3, mid2);

            exit(1);
        }
        if(p4 == getpid()){
            // printf("I am process p4 and my pid is %d\n", getpid());
            printf("sorting in p4\n");
            bubbly(arr4, rem2);

            exit(1);
        }

        merge_arr(temp1, arr1, arr2, mid1, rem1);

        merge_arr(temp2, arr3, arr4, mid2, rem2);

        merge_arr(entries, temp1, temp2, len1, len2);

        printf("final sorted list:\n");
        puke(entries, num_lines);

        time_t end = time(NULL);

        time_t delta = end - start;

        printf("Sort took %ld seconds\n", delta);

        /*for (int i = 0; i < 2; ++i)
        {
            int pid = fork();

            if(pid < 0){
                perror("fork failed");
                exit(-1);
            }

            else if(pid == 0){

                if(i == 0){
                    bubbly(arr1, mid1);
                }
                else if (i == 1){
                    bubbly(arr4, rem2);
                }

                exit(1);
            }

            else if(pid > 0){

                if(i == 0){
                    bubbly(arr1, mid1);
                }
                else if (i == 1){
                    bubbly(arr4, rem2);
                }

                wait(NULL);
            }
        }*/

    }

    /*if(choice == 4){

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
        puke(arr4, rem2);*/

        /*----------------------------------------------*/

         //split second half of new array
       /* int len3 = rem;
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

    }*/

    // puke(arr1, mid);
    // puke(arr2, rem);

    // bubbly(arr1, mid);
    // bubbly(arr2, rem);

    /*if(choice == 4){

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
    }*/


        

    /*printf("first array after sort:\n");
    puke(arr3, mid2);

    printf("second array after sort:\n");
    puke(arr4, rem2);

    printf("third array after sort:\n");
    puke(arr5, mid3);

    printf("fourth array after sort:\n");
    puke(arr6, rem3);*/

    //mid is length of arr1, rem is length of arr2

    /*int *res_arr1 = malloc((mid2 + rem2) * sizeof(int));

    merge_arr(res_arr1, arr3, arr4, mid2, rem2);

    int *res_arr2 = malloc((mid3 + rem3) * sizeof(int));

    merge_arr(res_arr2, arr5, arr6, mid3, rem3);

    int *fin_arr = malloc((mid2 + rem2 + mid3 + rem3) * sizeof(int));

    merge_arr(fin_arr, res_arr1, res_arr2, (mid2 + rem2), (mid3 + rem3));

    printf("final array\n");
    puke(fin_arr, (mid2 + rem2 + mid3 + rem3));*/

    return 0;
}