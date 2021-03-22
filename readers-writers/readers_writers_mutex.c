#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define TRUE 1

#define NE 3 //numero de escritores
#define NL 10 //numero de leitores

pthread_mutex_t lock_bd = PTHREAD_MUTEX_INITIALIZER; /* lock para controlar o acesso ao banco de dados */
pthread_mutex_t lock_nl = PTHREAD_MUTEX_INITIALIZER; /* lock para controlar o acesso a varável num_leitores */
pthread_mutex_t lock_tn = PTHREAD_MUTEX_INITIALIZER; /* lock para controlar o acesso ao turno e garantir que os escritores tenham a possibilidade de escrita */

int num_leitores = 0;

void * reader(void *arg);
void * writer(void *arg);
void read_data_base();
void use_data_read();
void think_up_data();
void write_data_base();

int main() {

	pthread_t r[NL], w[NE];
	int i;
        int *id;
        /* criando leitores */
    	for (i = 0; i < NL ; i++) {
	   id = (int *) malloc(sizeof(int));
           *id = i;
		 pthread_create(&r[i], NULL, reader, (void *) (id));
	}
	 /* criando escritores */
	for (i = 0; i< NE; i++) {
	   id = (int *) malloc(sizeof(int));
           *id = i;
		 pthread_create(&w[i], NULL, writer, (void *) (id));
	}
	pthread_join(r[0],NULL);
	return 0;
}

void * reader(void *arg) {
	int i = *((int *) arg);
	while(TRUE) { /* repete para sempre */
		pthread_mutex_lock(&lock_tn); /* lock que garante que outros leitores não acessem o banco de dados */
            pthread_mutex_lock(&lock_nl); /* lock que garante que somente um leitor, por vez, incremente a variável num_leitores */
				num_leitores++; /* variável que controla o acesso e incrementa o número de leitores entrando no banco de dados */
		        if(num_leitores == 1){ /* o primeiro leitor deve garantir que o banco dados esteja bloqueado para escrita */
		             pthread_mutex_lock(&lock_bd); /* lock que garante o acesso ao banco de dados por parte dos leitores */
		        }
            pthread_mutex_unlock(&lock_nl); /* libera o acesso a variável num_leitores */
		pthread_mutex_unlock(&lock_tn); /* libera o acesso aos outros leitores para que tenham acesso ao banco de dados */

        read_data_base(i);       /* acesso aos dados */

		pthread_mutex_lock(&lock_nl); /* lock que garante que somente um leitor decremente a variável num_leitores */
			num_leitores--; /* variável que controla o acesso e incrementa o número de leitores saindo do banco de dados */
			if(num_leitores == 0){ /* o último leitor deve garantir que o banco dados esteja liberado para escrita */
				pthread_mutex_unlock(&lock_bd); /* leitores liberam acesso ao banco de dados */
			}
		pthread_mutex_unlock(&lock_nl); /* libera o acesso a variável num_leitores */
        use_data_read(i);     /* região não crítica */
	}
        pthread_exit(0);
}

void * writer(void *arg) {
	int i = *((int *) arg);
	while(TRUE) { /* repete para sempre */
		think_up_data(i); /* região não crítica */
		pthread_mutex_lock(&lock_tn); /* lock que garante que outros leitores não acessem o banco de dados, após um escritor desejar escrever no BD */
            pthread_mutex_lock(&lock_bd); /* bloqueia o banco de dados no momento em que estiver ocorrendo uma escrita */
            write_data_base(i); /* atualiza os dados */
			pthread_mutex_unlock(&lock_bd); /* escritor libera o banco de dados e informa que finalizou sua escrita */
		pthread_mutex_unlock(&lock_tn); /* libera definitivamente para que outros escritores escrevam ou outros leitores leiam */
        }
        pthread_exit(0);
}

void read_data_base(int i) {
	printf("Leitor %d está lendo os dados! Número de leitores: %d\n", i+1,num_leitores);
	sleep(rand() % 5);
}

void use_data_read(int i) {
	printf("Leitor %d está usando os dados lidos! Número de leitores: %d\n", i+1,num_leitores);
	sleep(rand() % 5);
}

void think_up_data(int i) {
	printf("Escritor %d está pensando no que escrever!\n", i+1);
	sleep(rand() % 5);
}

void write_data_base(int i) {
	printf("Escritor %d está escrevendo os dados! Número de leitores: %d\n", i+1,num_leitores);
	sleep( rand() % 5 + 15);
}