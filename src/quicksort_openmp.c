#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>

#define MAX_NUMBER 1000

unsigned int seed;
int threads;
int size_arr;

void * print_array(int * arr, int * size_arr){
    printf("Array: ");
    for(int index = 0; index < *size_arr; index++)
        printf("%d ", arr[index]);
    printf("\n");
    return NULL;
}

void * change_place(int * a, int * b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
    return NULL;
}

int partition(int * arr, int ini, int end){
    int pivot = arr[end];
    
    int smaller_pivot = ini;
    int more_pivot = ini;
    
    for(; more_pivot < end; more_pivot++)
        if(arr[more_pivot] <= pivot){
            change_place(&arr[more_pivot], &arr[smaller_pivot]);
            smaller_pivot++;
        }
    
    change_place(&arr[smaller_pivot], &arr[end]);

    return smaller_pivot;
}

void quicksort(int * arr, int ini, int end){
 
    if(ini < end){
        int pivot = partition(arr, ini, end);
        #pragma omp task shared(arr) firstprivate(ini, pivot)
        { 
        	quicksort(arr, ini, pivot-1); 
        }
        #pragma omp task shared(arr) firstprivate(end, pivot)
        { 
        	quicksort(arr, pivot+1, end); 
        }

        #pragma omp taskwait
    }
}

void read_vector_in_file(int * vector, int n){
	FILE * file;
	
	char file_name[30];
	sprintf(file_name, "data/%d.bin", n);
	
	file = fopen(file_name, "rb");

	if(file != NULL){
		
		int i = 0;

		while(1){
			int value;

			size_t re = fread(&value, sizeof(int), 1, file);

			if(re<1)
				break;
			else
				vector[i++] = value; 
		}
		fclose(file);
	}else{
		printf("Erro ao abrir arquivo.\n");
		exit(1);
	}
}

int main(int argc, char **argv){
    struct timeval start, stop;
    int size_arr = atoi(argv[1]);
    
    threads = atoi(argv[2]);

    seed = time(NULL);

    int * arr = malloc(sizeof(int)*size_arr);
    read_vector_in_file(arr, size_arr);
    //print_array(arr, &size_arr);
    
    gettimeofday(&start, 0);
    #pragma omp parallel default(none) shared(size_arr,arr) num_threads(threads)
    {
        #pragma omp single nowait
        {
            quicksort(arr, 0, size_arr-1);
        }
    }
    
    gettimeofday(&stop, 0);
    printf("%d\n", (int)(((double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec))*100));

    //print_array(arr, &size_arr);
    return 0;
}