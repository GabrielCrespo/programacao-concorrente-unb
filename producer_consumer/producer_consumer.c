/*
* Gabriel Crespo de Souza
* 140139982
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer

pthread_mutex_t lock_buffer = PTHREAD_MUTEX_INITIALIZER; // lock que controla o acesso ao buffer e outras variáveis
pthread_cond_t cond_produtor = PTHREAD_COND_INITIALIZER; // variável de condição do produtor
pthread_cond_t cond_consumidor = PTHREAD_COND_INITIALIZER; // variável de condição do consumidor

void * produtor(void * meuid);
void * consumidor (void * meuid);

int buffer[N] = {0, 0, 0, 0, 0}; // buffer, inicialmente vazio
int posicao_insere_produtor = 0; // controla a posição do buffer a obter uma produção
int posicao_remove_consumidor = 0; // controla a posição do buffer a obter um consumo
int contador = 0; // quantidade de itens no buffer

void main(argc, argv)
int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[PR];
   
  for (i = 0; i < PR; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumidor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
 
  pthread_join(tid[0],NULL);

}

void * produtor (void* pi)
{
  while(1)
  {
   pthread_mutex_lock(&lock_buffer); // lock para acesso do produtor ao buffer e outras variáveis
      while(contador == N){
        pthread_cond_wait(&cond_produtor, &lock_buffer); // espere enquanto o buffer estiver cheio
      }
      buffer[posicao_insere_produtor] = 1; // produtor inseriu "item" no buffer
      posicao_insere_produtor++; // incrementa a posição do buffer para produzir na próxima posição
      contador++; // incrementa a quantidade de itens no buffer
      printf("Produtor inseriu item na posicao %d\tQtd itens no buffer: %d\n", posicao_insere_produtor, contador);
      pthread_cond_signal(&cond_consumidor);  // acorda o consumidor para consumir o item inserido
    pthread_mutex_unlock(&lock_buffer); // libera o acesso ao buffer e outras variáveis
    sleep(1);

  }
  pthread_exit(0);
}

void * consumidor (void* pi)
{
 while(1)
  {
    pthread_mutex_lock(&lock_buffer); // lock para acesso do consumidor ao buffer e outras variáveis
      while(contador == 0){
        pthread_cond_wait(&cond_consumidor, &lock_buffer); // espere enquanto o buffer estiver vazio
      }
      buffer[posicao_remove_consumidor] = 0; // consumidor removeu "item" do buffer
      posicao_remove_consumidor++; // incrementa a posição do buffer para consumir na próxima posição
      contador--; // decrementa a quantidade de itens no buffer
      printf("Consumidor removeu item na posicao %d\tQtd itens no buffer: %d\n", posicao_remove_consumidor, contador);
      if(contador < N){
        pthread_cond_signal(&cond_produtor); // Se há espaço livre no buffer, então acorde o produtor
      }
    pthread_mutex_unlock(&lock_buffer); // libera o acesso ao buffer e outras variáveis
    sleep(1);
  }
  pthread_exit(0);
}
