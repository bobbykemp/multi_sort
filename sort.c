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

//entry struct represents one line from the mlb csv file
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

//helper function that returns a void pointer to a new shared memory location
void* create_shared_mem(size_t size){

  int protection = PROT_READ | PROT_WRITE;

  int visibility = MAP_ANONYMOUS | MAP_SHARED;

  return mmap(NULL, size, protection, visibility, 0, 0);
}

//takes three array arguments:
//res_arr (combined array), arr1 (first array to be merged), arr2 (second array to be merged)
//merges arr1 and arr2 into res_arr
//len1 is length of first array, len2 is length of second array
void merge_arr(entry *res_arr, entry *arr1, entry *arr2, int len1, int len2){

    int total = len1 + len2;

    //l_ptr -> index into arr1
    //r_ptr -> index into arr2
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

        //if we get to the end of either arr1 or arr2
        //dump the remaining values of the other array into the res_arr
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

//simple swap procedure using pointers
void swap_elem(entry *a, entry *b){
    entry temp = *a;
    *a = *b;
    *b = temp;
}

//takes three array parameters:
//arr (the array to be split in half), arr1 (the first half of the split array)
//arr2 (the second half of the split array)
//arr1_len is length of first array, arr_2len is length of second array
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

//helper function for producing *len* child processes
//theses processes do not exit until explicitly told to outside
//of this function
//takes param arr that stores the pid of all created children
//so that they may be referenced outside of this function
void forker(pid_t *arr, int len){

    arr[0] = getpid();
    printf("parent process id is %d\n", arr[0]);

    for (int i = 0; i < len; ++i)
    {
        /* code */
        int pid;
        if( (i == 1) || ((i > 1) && (getpid() == arr[i - 1])) ){
            // if(i > 1){
            //     printf("I am previous process %d and I am forking a new child\n", arr[i - 1]);
            // }
            pid = fork();
        }

        if (pid < 0){perror("fork failed"); exit(-1);}

        else if(pid == 0){
            pid_t p2 = getpid();
            // printf("pid is %d\n", p2);
            arr[i] = getpid();
            printf("arr[i] is %d\n", arr[i]);
            // exit(1);
        }

        else{
            wait(NULL);
        }

    }

}

//will print every value in an array of type entry
//most is commented out now because we only care about sorting team1
//but the other data can be uncommented if desired
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

//simple bubble sort routine to sort by team1
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

int main(int argc, char const *argv[])
{

    FILE *file = fopen(argv[1], "r");
    char *value_buffer;

    //make sure we get exactly one file argument
    if(argv[1] == "" || argv[1] == NULL || argc > 1){
        printf("You need to supply exactly one argument - file to be sorted. Exiting\n");
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

    printf("There are %d lines in file %s\n", num_lines, argv[1]);

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

    while(choice != 1 && choice != 2 && choice != 4 && choice != 8){
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

        pid_t *process = create_shared_mem(4 * sizeof(pid_t));

        forker(process, 4);

        printf("---------------------\n");

        if(process[0] == getpid()){
            // printf("I am process p1 and my pid is %d\n", getpid());
            printf("sorting in pid %d ; actual pid = %d\n" ,process[0], getpid());
            bubbly(arr1, mid1);
        }
        if(process[1] == getpid()){
            // printf("I am process p2 and my pid is %d\n", getpid());
            printf("sorting in pid %d ; actual pid = %d\n" ,process[1], getpid());
            bubbly(arr2, rem1);

            exit(1);
        }
        if(process[2] == getpid()){
            // printf("I am process p3 and my pid is %d\n", getpid());
            printf("sorting in pid %d ; actual pid = %d\n" ,process[2], getpid());
            bubbly(arr3, mid2);

            exit(1);
        }
        if(process[3] == getpid()){
            // printf("I am process p4 and my pid is %d\n", getpid());
            printf("sorting in pid %d ; actual pid = %d\n" ,process[3], getpid());
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

    }

    if(choice == 8){

        time_t start = time(NULL);

        //how many data points per group
        int len = num_lines / 8;

        //how many data points are in the remaining group
        int rem = num_lines % 8;

        //willl split the original array into two halves
        //each half is stored in temp1 and temp2
        entry *temp1 = create_shared_mem((num_lines / 2) * sizeof(entry));
        entry *temp2 = create_shared_mem(((num_lines % 2) + (num_lines / 2)) * sizeof(entry));

        entry *temp3 = create_shared_mem((num_lines / 4) * sizeof(entry));
        entry *temp4 = create_shared_mem((num_lines / 4) * sizeof(entry));
        entry *temp5 = create_shared_mem((num_lines / 4) * sizeof(entry));
        entry *temp6 = create_shared_mem(((num_lines % 4) + (num_lines / 4)) * sizeof(entry));

        entry *arr1 = create_shared_mem(len * sizeof(entry));
        entry *arr2 = create_shared_mem(len * sizeof(entry));
        entry *arr3 = create_shared_mem(len * sizeof(entry));
        entry *arr4 = create_shared_mem(len * sizeof(entry));
        entry *arr5 = create_shared_mem(len * sizeof(entry));
        entry *arr6 = create_shared_mem(len * sizeof(entry));
        entry *arr7 = create_shared_mem(len * sizeof(entry));
        entry *arr8 = create_shared_mem((len + rem) * sizeof(entry));

        //split original array
        split(entries, temp1, temp2, (num_lines / 2), ((num_lines / 2) + (num_lines % 2)));

        //split first half of original array
        split(temp1, temp3, temp4, (num_lines / 4), (num_lines / 4));
        //split seccond half of original array
        split(temp2, temp5, temp6, (num_lines / 4), ((num_lines / 4) + (num_lines % 4)));

        //split quarters into eighths
        split(temp3, arr1, arr2, len, len);
        split(temp4, arr3, arr4, len, len);
        split(temp5, arr5, arr6, len, len);
        split(temp6, arr7, arr8, len, (len + rem));

        //by now we have eight arrays

        pid_t *process = create_shared_mem(8 * sizeof(pid_t));

        forker(process, 8);

        printf("---------------------\n");

        if(process[0] == getpid()){
            // printf("I am process p1 and my pid is %d\n", getpid());
            printf("sorting in pid %d ; actual pid = %d\n" ,process[0], getpid());
            bubbly(arr1, len);
        }
        if(process[1] == getpid()){
            // printf("I am process p2 and my pid is %d\n", getpid());
            printf("sorting in pid %d ; actual pid = %d\n" ,process[1], getpid());
            bubbly(arr2, len);

            exit(1);
        }
        if(process[2] == getpid()){
            // printf("I am process p3 and my pid is %d\n", getpid());
            printf("sorting in pid %d ; actual pid = %d\n" ,process[2], getpid());
            bubbly(arr3, len);

            exit(1);
        }
        if(process[3] == getpid()){
            // printf("I am process p4 and my pid is %d\n", getpid());
            printf("sorting in pid %d ; actual pid = %d\n" ,process[3], getpid());
            bubbly(arr4, len);

            exit(1);
        }
        if(process[4] == getpid()){
            // printf("I am process p4 and my pid is %d\n", getpid());
            printf("sorting in pid %d ; actual pid = %d\n" ,process[4], getpid());
            bubbly(arr5, len);

            exit(1);
        }
        if(process[5] == getpid()){
            // printf("I am process p4 and my pid is %d\n", getpid());
            printf("sorting in pid %d ; actual pid = %d\n" ,process[5], getpid());
            bubbly(arr6, len);

            exit(1);
        }
        if(process[6] == getpid()){
            // printf("I am process p4 and my pid is %d\n", getpid());
            printf("sorting in pid %d ; actual pid = %d\n" ,process[6], getpid());
            bubbly(arr7, len);

            exit(1);
        }
        if(process[7] == getpid()){
            // printf("I am process p4 and my pid is %d\n", getpid());
            printf("sorting in pid %d ; actual pid = %d\n" ,process[7], getpid());
            bubbly(arr8, len + rem);

            exit(1);
        }

        //split quarters into eighths
        merge_arr(temp3, arr1, arr2, len, len);
        merge_arr(temp4, arr3, arr4, len, len);
        merge_arr(temp5, arr5, arr6, len, len);
        merge_arr(temp6, arr7, arr8, len, len + rem);

        //split first half of original array
        merge_arr(temp1, temp3, temp4, (num_lines / 4), (num_lines / 4));
        //split seccond half of original array
        merge_arr(temp2, temp5, temp6, (num_lines / 4), ((num_lines / 4) + (num_lines % 4)));

        //split original array
        merge_arr(entries, temp1, temp2, (num_lines / 2), ((num_lines / 2) + (num_lines % 2)));

        printf("final sorted list:\n");
        puke(entries, num_lines);

        time_t end = time(NULL);

        time_t delta = end - start;

        printf("Sort took %ld seconds\n", delta);

    }

    return 0;
}