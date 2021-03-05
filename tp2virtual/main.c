#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>


///////////////////////////////////////////////////////////////
//                         tabela                           //
/////////////////////////////////////////////////////////////

typedef struct pagina{
	int quadro;//indice do array de quadros que corresponde a esta pagina.
  unsigned int numero_pagina;//numero da pagina, de acordo com os bits extraidos de um endereco do .log
  unsigned int ultimo_endereco_acessado;//contem o ultimo endereco acessado desta pagina
  int ultimo_acesso;//indica o instante do ultimo acesso a essa pagina
  bool suja;//indica se a pagina esta suja ou nao
  bool segunda_chance;//indica se a pagina tem mais uma chance de ficar na tabela(apenas para o algoritmo "second chance")
} pagina;

//esta tabela nao sera usada para o fifo ou para o lru
typedef struct tabela{
	unsigned int num_entradas;
	pagina *paginas;
} tabela;

typedef struct quadro{
	unsigned int ultimo_acesso; //Os clocks podem ser considerados como a colocação da intrução lida
	unsigned int _carregamento; //O instante de carregamento para a memória
  bool esta_na_memoria;//indica se o quadro esta na memoria ou nao
} quadro;

///////////////////////////////////////////////////////////////
//                            Lista                         //
/////////////////////////////////////////////////////////////

typedef struct elementoLista{         
  pagina page;         
  struct elementoLista *seguinte;       
} Elemento;       
typedef struct ListaDetectada{         
Elemento *inicio;  Elemento *fim;  int tamanho;       
} Fila;

void inicializacao (Fila * sequencia){         
  sequencia->inicio = NULL;         
  sequencia->fim = NULL;         
  sequencia->tamanho = 0;       
}

/* inserir (adicionar) um elemento na fila */       
int inserir (Fila * sequencia, Elemento * atual, pagina page){         
  Elemento *novo_elemento;         
  if ((novo_elemento = (Elemento *) malloc (sizeof (Elemento))) == NULL)
    return -1;         

  novo_elemento->page.numero_pagina = page.numero_pagina;
  novo_elemento->page.quadro = page.quadro;
  novo_elemento->page.ultimo_endereco_acessado = page.ultimo_endereco_acessado;
  novo_elemento->page.suja = page.suja;
          
  if(atual == NULL){           
    if(sequencia->tamanho == 0)             
      sequencia->fim = novo_elemento;           
    novo_elemento->seguinte = sequencia->inicio;           
    sequencia-> inicio = novo_elemento;         
  }
  else 
  {           
    if(atual->seguinte == NULL)             
      sequencia->fim = novo_elemento;           
    novo_elemento->seguinte = atual->seguinte;           
    atual-> seguinte = novo_elemento;         
  }         
  sequencia->tamanho++;         
  return 0;       
}

/* remover (eliminar) elemento da fila */       
int remover (Fila * sequencia){         
  Elemento *remov_elemento;         
  if (sequencia->tamanho == 0)           
    return -1;         
  remov_elemento = sequencia->inicio;         
  sequencia-> inicio = sequencia->inicio->seguinte;
  if(remov_elemento == sequencia->fim)
    sequencia->fim = NULL;                   
  free (remov_elemento);         
  sequencia->tamanho--;         
  return 0;       
}

/* exibição da fila */       
void exibir (Fila *sequencia){         
  Elemento *atual;         
  int i;         
  atual = sequencia->inicio;         
  for(i=0;i<sequencia->tamanho;++i){           
    printf("numero da página: %u \n", atual->page.numero_pagina);           
    atual = atual->seguinte;         
  }       
}

///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

//Função para validação do algorítmo recebido por parametro em main()
int valida_entrada(char* alg){

  if ((strcmp(alg, "lru") == 0) || (strcmp(alg, "2a") == 0) || (strcmp(alg, "fifo") == 0) || (strcmp(alg, "new") == 0)){
    return 1;
  }
  else
    return 0;
}

int main (int argc, char *argv[]){

  char *alg = argv[1];
  char *arq = argv[2];
  int tamPagina = atoi(argv[3]);
  int tamMemoriaF = atoi(argv[4]);
  int contador_clock = 0;

  int i;
  /*
  Abertura e controle dos parametros de entrada
  */
  FILE *arquivo = fopen(arq,"r");

  if(arquivo == NULL){
      printf("Erro ao ler arquivo.\n");
      return 1;
  }

  //Verificando se a política de substituição é válida
  if(!valida_entrada(alg)){
  	printf("Erro ao identificar política de substituição.\n");
  	return 0;
  }

  int total_paginas = tamMemoriaF / tamPagina; //numero total de paginas/quadros na tabela e na memoria
  printf("Numero total de paginas na memoria e na tabela: %d\n", total_paginas);

  //inicializando estruturas para executar os algoritmos de substituicao
  Fila *tabela_fifo;
  tabela tabela_nao_fifo; 

  if(strcmp(alg, "fifo") == 0){
    if ((tabela_fifo = (Fila *) malloc(sizeof(Fila))) == NULL)           
      return -1;         
    inicializacao(tabela_fifo); 
  }
  else
  if(strcmp(alg, "lru") == 0 || strcmp(alg, "2a") == 0 || strcmp(alg, "new") == 0){

    tabela_nao_fifo.paginas = (pagina *) malloc(total_paginas * sizeof(pagina));
    for(i = 0; i < total_paginas; i++){
      tabela_nao_fifo.paginas[i].quadro = -1;
      tabela_nao_fifo.paginas[i].segunda_chance = false;
    }
  }
        

  quadro quadros_memoria[total_paginas];//esta sera a estrutura responsavel por simular os quadros que estarao na memoria
  //inicializo atributos dos quadros
  
  for(i = 0; i < total_paginas; i++){
    quadros_memoria[i].esta_na_memoria = false;
  }

  unsigned int endereco;//endereco lido em uma linha do arquivo
  char operacao;//operacao descrita em uma linha do arquivo
  int tamPaginaBytes = tamPagina * pow(2, 10); //tamanho da pagina em bytes

  //Pego o numero maximo de bits que podem ser usados para identificar as paginas(algoritmo retirado da especificacao deste trabalho) 
  unsigned s, tmp;
  tmp = tamPaginaBytes;
  s = 0;
  while (tmp>1) {
    tmp = tmp>>1;
    s++;
  }
  s = 32u - s;

  printf("valor de s: %u\n", s);

  unsigned int indice;
  unsigned int miss = 0;
  unsigned int hit = 0;
  unsigned int leituras = 0;
  unsigned int escritas = 0;

  printf("Executando simulador ...\n");

  clock_t inicio = clock();

  while(fscanf(arquivo,"%x %c\n", &endereco, &operacao) != EOF){
    contador_clock++;
    leituras += operacao == 'R' ? 1 : 0;
    escritas += operacao == 'W' ? 1 : 0;

    unsigned int numero_pagina_acessada = endereco >> s;

    if(strcmp(alg, "fifo") == 0){
      //verifica se pagina esta na fila
      Elemento* i_elemento;
      i_elemento = tabela_fifo->inicio;
      bool pagina_esta_na_fila = false;
      while(i_elemento != NULL){
        //se a pagina acessada esta na tabela de paginas
        if(i_elemento->page.numero_pagina == numero_pagina_acessada){
          hit++;

          //atualiza dados da tabela, referentes a pagina acessada
          i_elemento->page.ultimo_endereco_acessado = endereco;
          i_elemento->page.suja = (operacao == 'W');

          pagina_esta_na_fila = true;
          break;
        }

        i_elemento = i_elemento->seguinte;
      }

      //se a pagina nao esta na fila
      if(!pagina_esta_na_fila){
        miss++;

        //pega o indice, no array de quadros na memoria, que recebera o novo quadro ou detecta que todos os quadros na memoria estao ocupados
        int indice_quadro_a_inserir = -1;
        for(i = 0; i < total_paginas; i++){
          if(!quadros_memoria[i].esta_na_memoria){
            indice_quadro_a_inserir = i;
            break;
          }
        }

        //se a memoria ja estiver lotada de quadros
        if(indice_quadro_a_inserir == -1){
          indice_quadro_a_inserir = tabela_fifo->inicio->page.quadro;
          remover(tabela_fifo);
        }

        //insere uma nova pagina na fila
        pagina* page = (pagina*) malloc(sizeof(pagina));
        page->numero_pagina = numero_pagina_acessada;
        page->quadro = indice_quadro_a_inserir;
        page->suja = (operacao == 'W');
        page->ultimo_endereco_acessado = endereco;
        inserir(tabela_fifo, tabela_fifo->fim, *page);

        //atualiza um atributo do quadro que estara na memoria
        quadros_memoria[indice_quadro_a_inserir].esta_na_memoria = true;
      }
    }
    else
    if(strcmp(alg, "lru") == 0){
      //verifica se pagina esta na tabela
      int i_pagina;
      bool pagina_esta_na_tabela = false;
      for(i_pagina = 0; i_pagina < total_paginas; i_pagina++){
        //se a pagina acessada esta na tabela de paginas
        if(tabela_nao_fifo.paginas[i_pagina].numero_pagina == numero_pagina_acessada){
          hit++;

          //atualiza dados da tabela, referentes a pagina acessada
          tabela_nao_fifo.paginas[i_pagina].ultimo_endereco_acessado = endereco;
          tabela_nao_fifo.paginas[i_pagina].suja = (operacao == 'W');
          tabela_nao_fifo.paginas[i_pagina].ultimo_acesso = contador_clock;

          pagina_esta_na_tabela = true;
          break;
        }
      }

      //se a pagina nao esta na tabela
      if(!pagina_esta_na_tabela){
        miss++;

        //pega o indice, no array de quadros na memoria, que recebera o novo quadro ou detecta que todos os quadros na memoria estao ocupados
        int indice_quadro_a_inserir = -1;
        for(i = 0; i < total_paginas; i++){
          if(!quadros_memoria[i].esta_na_memoria){
            indice_quadro_a_inserir = i;
            break;
          }
        }

        //se a memoria ja estiver lotada de quadros
        if(indice_quadro_a_inserir == -1){
          //pego o indice da pagina least recently used
          int menor_ultimo_acesso = -1;
          for(i_pagina = 0; i_pagina < total_paginas; i_pagina++){
            if(tabela_nao_fifo.paginas[i_pagina].ultimo_acesso < menor_ultimo_acesso){
              menor_ultimo_acesso = tabela_nao_fifo.paginas[i_pagina].ultimo_acesso;
              indice_quadro_a_inserir = i_pagina;
            }
          }
        }

        //escreve(ou sobrescreve) uma nova pagina na tabela
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].numero_pagina = numero_pagina_acessada;
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].quadro = indice_quadro_a_inserir;
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].suja = (operacao == 'W');
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].ultimo_endereco_acessado = endereco;
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].ultimo_acesso = contador_clock;

        //atualiza um atributo do quadro que estara na memoria
        quadros_memoria[indice_quadro_a_inserir].esta_na_memoria = true;
      }
    }
    else
    if(strcmp(alg, "2a") == 0){
      //verifica se pagina esta na tabela
      int i_pagina;
      bool pagina_esta_na_tabela = false;
      for(i_pagina = 0; i_pagina < total_paginas; i_pagina++){
        //se a pagina acessada esta na tabela de paginas
        if(tabela_nao_fifo.paginas[i_pagina].numero_pagina == numero_pagina_acessada){
          hit++;

          //atualiza dados da tabela, referentes a pagina acessada
          tabela_nao_fifo.paginas[i_pagina].ultimo_endereco_acessado = endereco;
          tabela_nao_fifo.paginas[i_pagina].suja = (operacao == 'W');
          tabela_nao_fifo.paginas[i_pagina].ultimo_acesso = contador_clock;
          tabela_nao_fifo.paginas[i_pagina].segunda_chance = true;

          pagina_esta_na_tabela = true;
          break;
        }
      }

      //se a pagina nao esta na tabela
      if(!pagina_esta_na_tabela){
        miss++;

        //pega o indice, no array de quadros na memoria, que recebera o novo quadro ou detecta que todos os quadros na memoria estao ocupados
        int indice_quadro_a_inserir = -1;
        for(i = 0; i < total_paginas; i++){
          if(!quadros_memoria[i].esta_na_memoria){
            indice_quadro_a_inserir = i;
            break;
          }
        }

        //se a memoria ja estiver lotada de quadros
        if(indice_quadro_a_inserir == -1){
          //pego o indice da pagina least recently used
          int menor_ultimo_acesso = -1;
          for(i_pagina = 0; i_pagina < total_paginas; i_pagina++){

            if(tabela_nao_fifo.paginas[i_pagina].ultimo_acesso < menor_ultimo_acesso){
              menor_ultimo_acesso = tabela_nao_fifo.paginas[i_pagina].ultimo_acesso;
              indice_quadro_a_inserir = i_pagina;
            }
            
            //se a pagina LRU tem uma segunda chance, remove esta segunda chance e reavalia a tabela de paginas
            if((i_pagina == total_paginas - 1) && (tabela_nao_fifo.paginas[indice_quadro_a_inserir].segunda_chance)){
              tabela_nao_fifo.paginas[indice_quadro_a_inserir].segunda_chance = false;
              i_pagina = 0; 
            }
          }
        }

        //escreve(ou sobrescreve) uma nova pagina na tabela
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].numero_pagina = numero_pagina_acessada;
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].quadro = indice_quadro_a_inserir;
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].suja = (operacao == 'W');
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].ultimo_endereco_acessado = endereco;
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].ultimo_acesso = contador_clock;
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].segunda_chance = false; 

        //atualiza um atributo do quadro que estara na memoria
        quadros_memoria[indice_quadro_a_inserir].esta_na_memoria = true;
      }
    }
    else
    if(strcmp(alg, "new") == 0){//algoritmo que escolhe aleatoriamente a pagina a ser removida
      //verifica se pagina esta na tabela
      int i_pagina;
      bool pagina_esta_na_tabela = false;
      for(i_pagina = 0; i_pagina < total_paginas; i_pagina++){
        //se a pagina acessada esta na tabela de paginas
        if(tabela_nao_fifo.paginas[i_pagina].numero_pagina == numero_pagina_acessada){
          hit++;

          //atualiza dados da tabela, referentes a pagina acessada
          tabela_nao_fifo.paginas[i_pagina].ultimo_endereco_acessado = endereco;
          tabela_nao_fifo.paginas[i_pagina].suja = (operacao == 'W');

          pagina_esta_na_tabela = true;
          break;
        }
      }

      //se a pagina nao esta na tabela
      if(!pagina_esta_na_tabela){
        miss++;

        //pega o indice, no array de quadros na memoria, que recebera o novo quadro ou detecta que todos os quadros na memoria estao ocupados
        int indice_quadro_a_inserir = -1;
        for(i = 0; i < total_paginas; i++){
          if(!quadros_memoria[i].esta_na_memoria){
            indice_quadro_a_inserir = i;
            break;
          }
        }

        //se a memoria ja estiver lotada de quadros
        if(indice_quadro_a_inserir == -1){
          //pego o indice de uma pagina aleatoriamente
          int menor_ultimo_acesso = -1;
          indice_quadro_a_inserir = (rand() % total_paginas);
        }

        //escreve(ou sobrescreve) uma nova pagina na tabela
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].numero_pagina = numero_pagina_acessada;
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].quadro = indice_quadro_a_inserir;
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].suja = (operacao == 'W');
        tabela_nao_fifo.paginas[indice_quadro_a_inserir].ultimo_endereco_acessado = endereco;

        //atualiza um atributo do quadro que estara na memoria
        quadros_memoria[indice_quadro_a_inserir].esta_na_memoria = true;
      }
    }
  }

  clock_t fim = clock();

  double tempoExecucao = (double)(fim - inicio) / CLOCKS_PER_SEC;

  printf("Arquivo de entrada: %s\n", arq);
  printf("Tamanho da memoria: %d KB\n", tamMemoriaF);
  printf("Tamanho das paginas: %d KB\n", tamPagina);
  printf("Tecnica de reposicao: %s\n", alg);
  printf("Paginas lidas: %d\n", leituras);
  printf("Paginas escritas: %d\n", escritas);
  printf("Tempo de execucao: %f\n\n", tempoExecucao);
  printf("Misses de pagina: %d\n", miss);
  printf("Hits de pagina: %d\n\n", hit);
  printf("Tabela: \n\n");

  if(strcmp(alg, "fifo") == 0){
    Elemento* i_elemento;
    i_elemento = tabela_fifo->inicio;

    while(i_elemento != NULL){
      printf("Numero da pagina: %u | Ultimo endereco acessado: %u | bit de controle(pagina suja): %d\n",
            i_elemento->page.numero_pagina, 
            i_elemento->page.ultimo_endereco_acessado, 
            i_elemento->page.suja ? 1 : 0);

      i_elemento = i_elemento->seguinte;
    }
  }
  else
  if(strcmp(alg, "lru") == 0 || strcmp(alg, "2a") == 0 || strcmp(alg, "new") == 0){
    int i_pagina;
    for(i_pagina = 0; i_pagina < total_paginas; i_pagina++){
      //se a pagina acessada tem um quadro na memoria
      if(tabela_nao_fifo.paginas[i_pagina].quadro != -1){
        printf("Numero da pagina: %u | Ultimo endereco acessado: %u | bit de controle(pagina suja): %d\n",
              tabela_nao_fifo.paginas[i_pagina].numero_pagina, 
              tabela_nao_fifo.paginas[i_pagina].ultimo_endereco_acessado, 
              tabela_nao_fifo.paginas[i_pagina].suja ? 1 : 0);
      }
    }
  }

  return 0;

}
