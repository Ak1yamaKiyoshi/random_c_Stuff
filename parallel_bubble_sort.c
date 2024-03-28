#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 12
pthread_t threads[NUM_THREADS];

struct SortAlorythmInnerArgs {
    int *array; int end; int start;
};


void bubble_sort(int *array, int len) {
    char flag;
    int tmp;
    for (int i = 0; i < len; i++) {
        flag = 0;
        for (int j = i + 1; j < len; j++) {
            if (array[j] < array[i]) {
                flag = 1;
                tmp = array[j];
                array[j] = array[i];
                array[i] = tmp;
            }
        }
    }
}


void print_array(int *array, int len) {
    printf("array(");
    for (int i = 0; i < len; i++) {
        if (i != len-1) {
            printf("%2d ", array[i]);
        } else if (i == len - 1) {
            printf("%2d)", array[i]);
        }
    }
}


void fill_random(int *array, const int len, const int limit, const int offset) {
    for (int i = 0; i < len; i++) 
        array[i] = offset + rand() % limit;
}


void *bubble_sort_chunk(void *arguments) {
    struct SortAlorythmInnerArgs *args = (struct ThreadArgs*)arguments;
    
    int* array = args->array;
    int start = args->start;
    int end = args->end;

    char flag;
    int tmp;
    for (int i = start; i < end; i++) {
        flag = 0;
        for (int j = i + 1; j < end; j++) {
            if (array[j] < array[i]) {
                flag = 1;
                tmp = array[j];
                array[j] = array[i];
                array[i] = tmp;
            }
        }
    }

    pthread_exit(NULL);
}


void p_bubble_sort(int *array, int len) {
    pthread_t threads[NUM_THREADS];
    void *ret;
  
    struct SortAlorythmInnerArgs args = {array, len, 0};
    args.array = array;
    args.start = 0;
    int chunk_size = len/NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        args.end = args.start + chunk_size;
        
        if (pthread_create(&threads[i], NULL, bubble_sort_chunk, &args) != 0) {
            perror("pthread_create() error");
            exit(1);
        }   
        args.start = args.end;  
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    args.start = 0;
    args.end = len;
    bubble_sort(array, len);
}


long long current_time_ns() {
    struct timespec currentTime;
    clock_gettime(CLOCK_REALTIME, &currentTime);
    return currentTime.tv_nsec;
}


double current_time_ms() {
    return (double)clock() / CLOCKS_PER_SEC * 1000;
}


void benchmark(void (*sort_algorythm)(int *, int)) {

    int iterations = 10;
    int lenght = 10000;
    int *arr = (int*)malloc(sizeof(int) * lenght);
    double total_time_ms = 0.;
    float average = 0;
    
    
    for (int i = 0; i < iterations; i++) {
        fill_random(arr, lenght, 100, 0);
        long start_time_ms = current_time_ms();
        sort_algorythm(arr, lenght);
        
        long end_time_ms = current_time_ms();
        long time_taken_ms = end_time_ms - start_time_ms;
        total_time_ms += time_taken_ms;
    }

    average = (float)total_time_ms / iterations;
    printf("Average: %10ld miliseconds", (long)average);

    free(arr);
    arr = NULL;
}


void run_once(void (*sort_algorythm)(int *, int)) {
    int lenght = 100;
    int *arr = (int*)malloc(sizeof(int) * lenght);
    fill_random(arr, lenght, 100, 0);
    print_array(arr, lenght);
    printf("\n");
    sort_algorythm(arr, lenght);
    print_array(arr, lenght);
}


int main() {
    srand(time(NULL));

    printf(" > parallel bubble sort \n");
    for (int i = 0; i < 3; i++) {
        benchmark(&p_bubble_sort);
        printf("\n");
    }

    printf("\n > bubble sort \n");
    for (int i = 0; i < 3; i++) {
        benchmark(&bubble_sort);
        printf("\n");
    }

    
    return 0;
}
