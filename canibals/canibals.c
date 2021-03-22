/*
*
* Gabriel Crespo de Souza
* 140139982
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

pthread_mutex_t lock_servir = PTHREAD_MUTEX_INITIALIZER; // lock que garante que haja somente um canibal se servindo 
pthread_cond_t cond_canibal = PTHREAD_COND_INITIALIZER; // Variável de condição para os canibais
pthread_cond_t cond_cozinheiro = PTHREAD_COND_INITIALIZER; // Variável de condição para o cozinheiro

void *canibal(void*meuid);
void *cozinheiro(int m);

int qtd_comida = 0; // Variável que controla a quantidade de comida disponível

void main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if(argc != 3){
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }
  
  n = atoi (argv[1]); //número de canibais
  m = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
  printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

  if(n > MAXCANIBAIS){
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }
  
  for (i = 0; i < n; i++)  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cozinheiro(m);
}

void * canibal (void* pi){
  
  while(1) {
    
    pthread_mutex_lock(&lock_servir); // permite que somente um canibal se sirva
      while (!qtd_comida) 
      {
        printf("As porções acabaram, então será necessário avisar ao cozinheiro\n");
        pthread_cond_wait(&cond_canibal, &lock_servir); // Canibal deve esperar enquanto a comida é preparada pelo cozinheiro
      }
      qtd_comida--; // Porções vão diminuindo a medida que os canibais vão se servindo
      if(qtd_comida == 0){
        pthread_cond_signal(&cond_cozinheiro); // Avisar ao cozinherio que a comida acabou
      }
    pthread_mutex_unlock(&lock_servir); // liberar o lock pois os canibais estão comendo
    printf("%d: vou comer a porcao que peguei\n", *(int *)(pi));
    sleep(1);
  }
  
}

void *cozinheiro (int m){
 
  while(1){
     
    pthread_mutex_lock(&lock_servir); // permite que nenhum um canibal se sirva enquanto o cozinheiro está em ação
      while (qtd_comida) 
      {
        printf("Cozinheiro dormindo...\n");
        pthread_cond_wait(&cond_cozinheiro, &lock_servir); // O cozinheiro descansa enquanto houver comida disponível para os canibais
      }
      printf("cozinheiro: vou cozinhar\n");
      qtd_comida = m; // Cozinheiro preparou todas as porções que consegue
      sleep(2);
      pthread_cond_broadcast(&cond_canibal); // Avise aos canibais que as porções estão prontas
    pthread_mutex_unlock(&lock_servir); // Libere o caldeirão para que os canibais possam se servir
   }

}