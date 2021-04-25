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

pthread_cond_t cond_pombo = PTHREAD_COND_INITIALIZER; // variável de condição do pombo
pthread_cond_t cond_usuario = PTHREAD_COND_INITIALIZER; // variável de condição dos usuários
pthread_mutex_t lock_bolsa = PTHREAD_MUTEX_INITIALIZER; // lock para garantir exclusão mútua à bolsa do pombo

int qtd_cartas_na_bolsa; // contador de cartas que a bolsa do pombo contém
int pombo_esta_em_A; // variável que verifica se o pombo está em A ou B

void *f_usuario(void *arg);
void *f_pombo(void *arg);

int main(int argc, char **argv)
{
    int i;

    qtd_cartas_na_bolsa = 0; // Inicialmente não há carta alguma na bolsa do pombo
    pombo_esta_em_A = 1; // Inicialmente o pombo está em A

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
        pthread_mutex_lock(&lock_bolsa); // Não permite nenhum usuário postar uma carta na bolsa do pombo enquanto ele estiver em ação.
            while (qtd_cartas_na_bolsa != CARTAS)
            {
                pthread_cond_wait(&cond_pombo, &lock_bolsa); // Pombo deve agurdar enquanto a sua mochila não estiver cheia de cartas
            }
            pombo_esta_em_A = 0; // Pombo não está mais na base A.
            
            printf("Pombo: indo para a base B...\n");
            qtd_cartas_na_bolsa = 0; // Pombo realizou as entregas de todas as cartas na base B.
            printf("Pombo: Cartas entregues a base B. Estou voltando para a base A...\n");
            printf("Pombo: cheguei na base A, voltem a escrever cartas e postar na minha bolsa...\n");
            pombo_esta_em_A = 1; // Pombo retornou à base A.
            pthread_cond_broadcast(&cond_usuario); // Pombo sinaliza a todos os usuários o seu retorno e que podem voltar a escrever cartas.
        pthread_mutex_unlock(&lock_bolsa); // libera o acesso a bolsa a todos os usuários.

    }
}

void *f_usuario(void *arg)
{
    int id_usuario = *((int *) arg);

    while (1)
    {
        printf("Usuário %d está escrevendo uma carta...\n", id_usuario);
        pthread_mutex_lock(&lock_bolsa); // Permite que um usuário de cada vez tenha acesso a bolsa do pombo.
            while (!pombo_esta_em_A || qtd_cartas_na_bolsa == CARTAS)
            {
                printf("Usuário %d está aguardando o pombo ficar disponível...\n", id_usuario);
                pthread_cond_wait(&cond_usuario, &lock_bolsa); // Usuários agurdam enquanto o pombo não está em A ou sua mochila atingiu a quantidade máxima de cartas.
            }
            qtd_cartas_na_bolsa++; // Incrementa a quantidade cartas na mochila do pombo.
            printf("Usuário %d terminou de escrever a carta...\n", id_usuario);
            printf("Quantidade de cartas na mochila: %d \n", qtd_cartas_na_bolsa);

            if(qtd_cartas_na_bolsa == CARTAS){
                printf("A mochila encheu!!\n");
                printf("Usuário %d acordou o pombo.\n", id_usuario);
                pthread_cond_signal(&cond_pombo); // O usuário que postar a última carta deve acordar o pombo para que entre em ação.
            }
        pthread_mutex_unlock(&lock_bolsa); // libera o acesso a bolsa do pombo.
        sleep(5);
    }
}
