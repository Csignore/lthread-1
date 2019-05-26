#include "lthread.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

void a(lthread_t *lt, void *x);

int* getPrime(int * numbers){
    int size = 12;
    printf("size is %d\n", size);
    int firstNotZero = 0;
    int firstPrime;

    for (int i = 0; i<size; i++){
        if (numbers[i] != 0){
            firstNotZero = i;
            firstPrime = numbers[i];
            break;
        }
    }
    
    printf("Take %d as the prime number\n", firstPrime);
    printf("return integer array as [");
    for (int i = firstNotZero+1; i<size; i++){
        if (i % firstPrime == 0){
            numbers[i] == 0;
        } else{
            printf("%d,", numbers[i]);
        }
    }
    printf("]\n");
    
    return *(&numbers+firstNotZero*sizeof(int));
}

union void_cast {
    void* ptr;
    int value;
};

int VOID_TO_INT(void* ptr) {
    union void_cast u;
    u.ptr = ptr;
    return u.value;
}

void
a(lthread_t *lt ,void *arg)
{
	struct timeval t1 = {0, 0};
	struct timeval t2 = {0, 0};
    DEFINE_LTHREAD;
    lthread_detach();

    int numbers[12];
    printf("this is the integer array generated at first: [");
    for(int i = 0; i <= 11; i++){
      numbers[i] = i+2;
      printf("%d, ", numbers[i]);
    }
    printf("]\n");


    lthread_compute_begin();
        int * bxArr = getPrime(numbers);
        printf("I am A in a compute thread \n");
    lthread_compute_end();
    lthread_set_data(&bxArr);
    printf("A going for a nap\n");
    lthread_sleep(3000);


	printf("a is exiting\n");
}

void
b(lthread_t *lt ,void *x)
{
	struct timeval t1 = {0, 0};
	struct timeval t2 = {0, 0};
    DEFINE_LTHREAD;
    lthread_detach();

    lthread_sleep(1000);

        printf(" I am B before begin\n");
       
        int * xArr = * (int *) lthread_get_data();
        printf("%d\n", xArr);
        lthread_compute_begin();
        getPrime(xArr);
            printf("I am B in a compute thread \n");
            sleep(10);
        lthread_compute_end();
        printf("B after  sleep\n");

	printf("b is exiting\n");
}

int
main(int argc, char **argv)
{
	lthread_t *lt = NULL;

	lthread_create(&lt, a, NULL);
	lthread_create(&lt, b, NULL);
	lthread_run();

	return 0;
}
