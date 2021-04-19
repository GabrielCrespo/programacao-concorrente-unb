/*
 *
 * Jantar dos Filósofos
 * Gabriel Crespo de Souza
 * 14/0139982
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5
#define ESQ(id) (id)
#define DIR(id) (id + 1) % N

// Numeração que irá representar os estados dos filósofos
enum estados_filosofo
{
	PENSANDO,
	FAMINTO,
	COMENDO
};

// Lock que irá garantir a reserva dos talheres ao filósofo que estiver com fome e quer comer
pthread_mutex_t reservar_talher = PTHREAD_MUTEX_INITIALIZER;

sem_t s[N];
int filosofo[N] = {PENSANDO}; // Vetor para controlar o estado atual de um filósofo

void *filosofos(void *arg);

void pega_talher(int n);
void devolve_talher(int n);

int main()
{
	int i;
	int *id;
	//semaforo
	for (i = 0; i < N; i++)
	{
		sem_init(&s[i], 0, 1);
	}
	pthread_t r[N];

	//criacao das threads de filosofos
	for (i = 0; i < N; i++)
	{
		id = (int *)malloc(sizeof(int));
		*id = i;
		pthread_create(&r[i], NULL, filosofos, (void *)(id));
	}

	pthread_join(r[0], NULL);
	return 0;
}

void *filosofos(void *arg)
{
	int n = *((int *)arg);
	while (1)
	{
		//pensar
		printf("Filosofo %d pensando ...\n", n);
		pega_talher(n); // Filósofo tenta pegar o talher para comer
		sleep(2);
		//comer
		printf("\tFilosofo %d comendo ...\n", n);
		printf("\tFilosofo %d acabou de comer ...\n", n);
		devolve_talher(n); // Filósofo devolve o talher logo após comer
		sleep(2);
	}
}

/*
 * Função que irá verificar se um filósofo que está faminto  
 * poderá ter acesso aos talheres caso os seus vizinhos não estejam comendo.
 * 
 */
void verificar_filosofos_vizinhos(int num_filosofo)
{
	int filosofo_a_esquerda = (num_filosofo - 1) % 5; // Filósofo à esquerda do filósofo atual
	int filosofo_a_direita = (num_filosofo + 1) % 5; // Filósofo à direita do filósofo atual

	if ((filosofo[filosofo_a_esquerda] != COMENDO) && (filosofo[num_filosofo] == FAMINTO) && (filosofo[filosofo_a_direita] != COMENDO))
	{
		filosofo[num_filosofo] = COMENDO; // O filósofo atual está comendo
		//printf("Filosofo %d está comendo...\n", num_filosofo);
		sem_post(&s[N]); // Filósofo sinaliza a liberação dos talheres
	}
}

// Função para reservar os talheres para o filósofo atual
void pega_talher(int n)
{
	pthread_mutex_lock(&reservar_talher); // Filósofo está com fome e quer ter acesso aos talheres
		filosofo[n] = FAMINTO; // O filósofo passa a ter o estado de faminto
		verificar_filosofos_vizinhos(n); // Antes de tudo é necessário verificar se seus vizinhos estão comendo ou não
	pthread_mutex_unlock(&reservar_talher); // Filósofo está com fome e quer ter acesso aos talheres
	sem_wait(&s[N]); // Filósofo sinaliza para pegar o talher
}

// Função para o filósofo atual liberar os talheres após comer 
void devolve_talher(int n)
{
	pthread_mutex_lock(&reservar_talher); // Filósofo terminou de comer e quer devolver os talheres
		filosofo[n] = PENSANDO; // Filósofo volta a pensar
		verificar_filosofos_vizinhos((n - 1) % 5); // Verificar o vizinho a esquerda para devolver o talher, se possível
		verificar_filosofos_vizinhos((n + 1) % 5); // Verificar o vizinho a direita para devolver o talher, se possível
	pthread_mutex_unlock(&reservar_talher); // Talheres devolvidos
}
