/*
 * 
 *  Gabriel Crespo de Souza
 *  14/0139982 
 * 
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila

sem_t mochila, pombo, usuario;

int qtd_cartas_na_bolsa; // contador de cartas que a bolsa do pombo contém

void *f_usuario(void *arg);
void *f_pombo(void *arg);

int main(int argc, char **argv)
{
    int i;

    qtd_cartas_na_bolsa = 0;
    sem_init(&mochila, 0, 1);
    sem_init(&pombo, 0, CARTAS);
    sem_init(&usuario, 0, 0);

    pthread_t usuario[N];
    int *id;
    for (i = 0; i < N; i++)
    {
        id = (int *)malloc(sizeof(int));
        *id = i;
        pthread_create(&(usuario[i]), NULL, f_usuario, (void *)(id));
    }
    pthread_t pombo;
    id = (int *)malloc(sizeof(int));
    *id = 0;
    pthread_create(&(pombo), NULL, f_pombo, (void *)(id));

    pthread_join(pombo, NULL);
}

void *f_pombo(void *arg)
{

    printf("Pombo: até o momento, me encontro na base A e com a mochila não cheia...\n");
    while (1)
    {
        sem_wait(&usuario); // Usuários devem aguardar enquanto o pombo estiver em ação.
        printf("Pombo: indo para a base B...\n");
        sem_wait(&mochila);      // Só o pombo tem acesso à mochila pois está ação.
        qtd_cartas_na_bolsa = 0; // Pombo realizou as entregas das cartas na base B.
        sleep(7);
        printf("Pombo: Cartas entregues a base B. Estou voltando para a base A...\n");
        sem_post(&mochila); // Acesso à mochila foi liberado pelo pombo aos usuários.
        printf("Pombo: cheguei na base A, voltem a escrever cartas e postar na minha bolsa...\n");
        for (int i = 0; i < CARTAS; i++)
        {
            sem_post(&pombo); // Liberar a capacidade da mochila do pombo para que novas cartas possam ocupar.
        }
    }
}

void *f_usuario(void *arg)
{
    int id_usuario = *((int *)arg);

    printf("Usuário %d está escrevendo uma carta...\n", id_usuario);
    while (1)
    {
        // Usuário deve aguardar enquanto o pombo não estiver em A ou sua mochila estiver cheia.
        if (sem_trywait(&pombo) != 0)
        {
            printf("Usuário %d está aguardando o pombo ficar disponível...\n", id_usuario);
            sem_wait(&pombo); // A capacidade da mochila do pombo fica indisponível devido lotação
        }
        sem_wait(&mochila); // Acesso à mochila deve ser feito por um usuário de cada vez
        qtd_cartas_na_bolsa++; // Usuário incrementa a quantidade de cartas após postar a sua na mochila do pombo
        printf("Usuário %d terminou de escrever a carta...\n", id_usuario);
        printf("Quantidade de cartas na mochila: %d \n", qtd_cartas_na_bolsa);

        // Verificar se a mochila encheu para que o pombo seja avisado.
        if (qtd_cartas_na_bolsa == CARTAS)
        {
            printf("A mochila encheu!!\n");
            printf("Usuário %d acordou o pombo.\n", id_usuario);
            sem_post(&usuario); // O usuário que postar a última carta deve acordar o pombo para que entre em ação.
        }
        sem_post(&mochila); // liberar acesso à mochila para o próximo usuário.
        sleep(3);
    }
}
