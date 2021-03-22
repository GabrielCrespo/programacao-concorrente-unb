#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 2 //macacos que andam de A para B
#define MB 2 //macacos que andam de B para A

pthread_mutex_t lock_A = PTHREAD_MUTEX_INITIALIZER; // lock que controla os acessos dos macacos A para B
pthread_mutex_t lock_B = PTHREAD_MUTEX_INITIALIZER; // lock que controla os acessos dos macacos B para A
pthread_mutex_t lock_corda = PTHREAD_MUTEX_INITIALIZER; // lock que controla a ida dos macacos em somente uma direção
pthread_mutex_t lock_turno = PTHREAD_MUTEX_INITIALIZER; // lock que garante que todos os macacos tenham o direito de atravessar de um lado para o outro
pthread_mutex_t lock_gorila = PTHREAD_MUTEX_INITIALIZER; // lock que garante somente o acesso do gorila à corda

int macacos_na_corda_a = 0;
int macacos_na_corda_b = 0;


void * macacoAB(void * a) {
    int i = *((int *) a);    
    while(1){
		pthread_mutex_lock(&lock_gorila); // gorila garante acesso somente a ele à corda
			pthread_mutex_lock(&lock_turno); // lock que garante a vez dos macacos saindo somente do lado A
				pthread_mutex_lock(&lock_A); // lock que controla o acesso à variavél que controla a quantidade de macacos na corda e indo no sentido AB
					macacos_na_corda_a++; // quantidade de macacos na corda e indo no sentindo AB
					if(macacos_na_corda_a == 1){ // o primeiro macaco a entrar na corda do lado A, irá bloquear o acesso à corda pelos macacos do lado B
						pthread_mutex_lock(&lock_corda); // lock que garante o bloqueio da corda para uso dos macacos do lado A
					}
				pthread_mutex_unlock(&lock_A); // libera o acesso à variável que controla a quantidade de macacos na corda
			pthread_mutex_unlock(&lock_turno); // libere a vez de uso da corda para os macacos do lado B
		pthread_mutex_unlock(&lock_gorila); // gorila libera o acesso à corda
		
		printf("Macaco %d passado de A para B\n", i); // região crítica
		sleep(1); // região crítica
		
		pthread_mutex_lock(&lock_A); // lock que controla o acesso à variavél que controla a quantidade de macacos saindo da corda no sentido AB
			macacos_na_corda_a--; // quantidade de macacos deixando a corda no sentido AB
			if(macacos_na_corda_a == 0){ // o último macaco a passar pela corda deverá liberar a corda para que os macacos do lado B possam atravessar
				pthread_mutex_unlock(&lock_corda); // a corda está liberada
			}
		pthread_mutex_unlock(&lock_A); // libera o acesso à variável que controla a quantidade de macacos saindo da corda
		}
	pthread_exit(0);
}

void * macacoBA(void * a) {
    int i = *((int *) a);    
    while(1){
	pthread_mutex_lock(&lock_gorila); // gorila garante acesso somente a ele à corda
		pthread_mutex_lock(&lock_turno); // lock que garante a vez dos macacos saindo somente do lado B
			pthread_mutex_lock(&lock_B); // lock que controla o acesso à variavél que controla a quantidade de macacos na corda e indo no sentido BA
				macacos_na_corda_b++; // quantidade de macacos na corda e indo no sentindo BA
				if(macacos_na_corda_b == 1){ // o primeiro macaco a entrar na corda do lado B, irá bloquear o acesso à corda pelos macacos do lado A
					pthread_mutex_lock(&lock_corda); // lock que garante o bloqueio da corda para uso dos macacos do lado B
				}
			pthread_mutex_unlock(&lock_B); // libera o acesso à variável que controla a quantidade de macacos na corda
		pthread_mutex_unlock(&lock_turno); // libere a vez de uso da corda para os macacos do lado A
	pthread_mutex_unlock(&lock_gorila);	// gorila libera o acesso à corda
	
	printf("Macaco %d passado de B para A\n", i); // região crítica
	sleep(1); // região crítica
	
	pthread_mutex_lock(&lock_B); // lock que controla o acesso à variavél que controla a quantidade de macacos saindo da corda no sentido BA
		macacos_na_corda_b--; // quantidade de macacos deixando a corda no sentido BA
		if(macacos_na_corda_b == 0){ // o último macaco a passar pela corda deverá liberar a corda para que os macacos do lado A possam atravessar
			pthread_mutex_unlock(&lock_corda); // a corda está liberada
		}
	pthread_mutex_unlock(&lock_B); // libera o acesso à variável que controla a quantidade de macacos saindo da corda
		
    }
    pthread_exit(0);
}

void * gorila(void * a){
    while(1){
	pthread_mutex_lock(&lock_gorila); // gorila garante sua vez de acessar à acorda
		pthread_mutex_lock(&lock_corda); // gorila bloqueia o acesso à corda
		printf("Gorila passado de A para B - Quantidade de macacos na corda: %d\n", macacos_na_corda_a + macacos_na_corda_b);
		sleep(5);
		pthread_mutex_unlock(&lock_corda); // gorila libera o acesso à corda
    pthread_mutex_unlock(&lock_gorila); // gorila libera definitivamento o acesso à corda
     } 
    pthread_exit(0);
}


int main(int argc, char * argv[])
{
    pthread_t macacos[MA+MB];
    int *id;
    int i = 0;

    for(i = 0; i < MA+MB; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        if(i%2 == 0){
          if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }else{
          if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }
    }
    pthread_t g;
    pthread_create(&g, NULL, &gorila, NULL);

    pthread_join(macacos[0], NULL);
    return 0;
}
