#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define N 2
#define X 10000

int contador = 0;

void * incrementar(void * arg){
	int id = *((int *) arg);
	printf("Criou um pthread com id = %d\n", id);
	int i = 0;
	for(i = 0; i < X; i++){
		contador++;
	}
	pthread_exit(0);
}

int main(){
	pthread_t a[N];
	int i;
	int *id;
	for(i = 0; i < N; i++){
		pthread_create(&a[i], NULL, incrementar, (void *) id);
	}
	
	for(i = 0; i < N; i++){
		pthread_join(a[i], NULL);
	}
	
	printf("Valor final do contador = %d\n", contador);
	printf("FIM!!!\n");
	return 0;
}