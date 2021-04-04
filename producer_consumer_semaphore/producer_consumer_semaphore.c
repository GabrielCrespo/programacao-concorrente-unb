/*
 * 
 * Gabriel Crespo de Souza
 * 14/0139982 
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer

pthread_mutex_t lock_buffer = PTHREAD_MUTEX_INITIALIZER; // lock para controlar os acessos do leitor e escritor a suas regiões críticas
sem_t buffer_pos_vazia; // semáforo que controla a quantidade de posições vazias existentes no buffer
sem_t buffer_pos_cheia; // semáforo que controla a quantidade de posições ocupadas existentes no buffer

int buffer[N]; // representação do buffer
int posicao_insere_produtor = 0; // controla a posição do buffer a obter uma produção
int posicao_remove_consumidor = 0; // controla a posição do buffer a obter um consumo
int qtd_items_buffer = 0; // variável que controla a quantidade de itens no buffer

void *produtor(void *meuid);
void *consumidor(void *meuid);

void main(argc, argv) 
  int argc;
  char *argv[];
{

  int erro;
  int i, n, m;
  int *id;
  
  sem_init(&buffer_pos_vazia, 0, N); // o semáforo de posições vazias é inicializado com N posições vazias que poderão receber um item
  sem_init(&buffer_pos_cheia, 0, 0); // o semáforo de posições ocupadas é inicializado com zero posições ocupadas.

  pthread_t tid[PR];

  // laço utilizado para iniciar todas as posições do buffer com zero.
  for(i = 0; i < N; i++){
    buffer[i] = 0;
  }

  for (i = 0; i < PR; i++)
  {
    id = (int *)malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, produtor, (void *)(id));

    if (erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++)
  {
    id = (int *)malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumidor, (void *)(id));

    if (erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_join(tid[0], NULL);
}

void *produtor(void *pi)
{

  int n; // variável que conterá um inteiro aleatório a ser inserido em alguma posição do buffer

  while (1)
  {
    n = (rand() % (N + 1)); // gerando o valor inteiro entre 0 e o tamanho do buffer + 1
    sem_wait(&buffer_pos_vazia); // semáforo que permite o produtor controlar e manipular uma posição vazia no buffer 
      pthread_mutex_lock(&lock_buffer); // garantia de que somente o produtor esteja acessando o buffer
        buffer[posicao_insere_produtor] = n; // guardando o valor produzido pelo produtor no buffer 
        posicao_insere_produtor = (posicao_insere_produtor + 1) % N; // buscando outra posição do buffer para alocar outro item produzido
        qtd_items_buffer++; // incrementando a quantidade de itens no buffer após produzir um item
      pthread_mutex_unlock(&lock_buffer); // liberando o acesso ao buffer
    sem_post(&buffer_pos_cheia); // após controlar e manipular a posição vazia, o produtor a devolve com o status de ocupada
    printf("Produzindo o valor: %d\tQuantidade de itens no buffer: %d\n", n, qtd_items_buffer); // imprime o item produzido e a quantidade de itens no buffer
    sleep(3);

  }
  pthread_exit(0);
}

void *consumidor(void *pi)
{
  int n;

  while (1)
  {
    sem_wait(&buffer_pos_cheia); // semáforo que permite o consumidor controlar e manipular uma posição ocupada no buffer
      pthread_mutex_lock(&lock_buffer); // garantia de que somente o consumidor esteja acessando o buffer
        n = buffer[posicao_remove_consumidor]; // guardando o valor a ser consumido pelo consumidor
        posicao_remove_consumidor = (posicao_remove_consumidor + 1) % N; // buscando outra posição do buffer para consumir outro item
        qtd_items_buffer--; // decrementando a quantidade de itens no buffer após consumir um item
      pthread_mutex_unlock(&lock_buffer); // liberando o acesso ao buffer
    sem_post(&buffer_pos_vazia); // após controlar e manipular a posição ocupada, o consumidor a devolve com o status de vazia
    printf("Consumindo o valor: %d\tQuantidade de itens no buffer: %d\n", n, qtd_items_buffer); // imprime o item consumido e a quantidade de itens no buffer
    sleep(4);
  }
  pthread_exit(0);
}
