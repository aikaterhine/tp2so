#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>//TODO: Remover esta linha


///////////////////////////////////////////////////////////////
//                         tabela                           //
/////////////////////////////////////////////////////////////

typedef struct pagina{
	char presente;//TODO: Retirar
	unsigned int moldura;//indice do array de quadros que corresponde a esta pagina. TODO: Renomear para "quadro"
  unsigned int numero_pagina;//numero da pagina, de acordo com os bits extraidos de um endereco do .log
  unsigned int ultimo_endereco_acessado;//contem o ultimo endereco acessado desta pagina
  bool suja;//indica se a pagina esta suja ou nao
} pagina;

//esta tabela nao sera usada para o fifo ou para o lru
typedef struct tabela{
	unsigned int num_entradas;
	pagina *paginas;
} tabela;

typedef struct moldura{//TODO: Renomear este struct para "quadro"
	pagina *pagina;//TODO: Retirar isto
	unsigned int ultimo_acesso; //Os clocks podem ser considerados como a colocação da intrução lida
	unsigned int _carregamento; //O instante de carregamento para a memória
  bool esta_na_memoria;//indica se o quadro esta na memoria ou nao
	char pagina_modificada;//TODO: Retirar isto
} moldura;

typedef struct memoria_processo{//TODO: Remover este struct
	unsigned int num_entradas;
	moldura *molduras;
} memoria_processo;



void criaTabela(tabela *tabela, int tamPagina){//TODO: Remover esta funcao
	tabela->num_entradas = 4194304 / tamPagina; //os enderecos fornecidos sao de 32 bits, logo temos 544288kb mapeados
    tabela->paginas = (pagina*) malloc(sizeof(pagina) * tabela->num_entradas);
    for(int i = 0; i < tabela->num_entradas; i++){
    	tabela->paginas[i].presente = 0;
		tabela->paginas[i].moldura = 0;
    }
}

void criaMemoriaProcesso(memoria_processo *memoria_processo, int tamMemoriaF){//TODO: Remover esta funcao
	memoria_processo->num_entradas = tamMemoriaF;
	memoria_processo->molduras = (moldura*) malloc(sizeof(moldura)*tamMemoriaF);
	for(int i=0;i<tamMemoriaF;i++){
		memoria_processo->molduras[i].pagina = NULL;
		memoria_processo->molduras[i].pagina_modificada = 0;
	}
}

///////////////////////////////////////////////////////////////
//                         algs                             //
/////////////////////////////////////////////////////////////

//TODO: Remover esta secao

int fifo_escolha(memoria_processo *memoria_processo, unsigned int clock){
	int escolha;
	int primeiro_acesso = clock;
	for(int i=0; i<memoria_processo->num_entradas; i++){
		if(memoria_processo->molduras[i]._carregamento < primeiro_acesso){
			escolha = i;
			primeiro_acesso = memoria_processo->molduras[i]._carregamento;
		}
	}
	return escolha;
}

int lru_escolha(memoria_processo *memoria_processo, unsigned int clock){
	int escolha;
	int ultimo_acesso = clock;
	for(int i=0; i<memoria_processo->num_entradas; i++){
		if(memoria_processo->molduras[i].ultimo_acesso < ultimo_acesso){
			escolha = i;
			ultimo_acesso = memoria_processo->molduras[i].ultimo_acesso;
		}
	}
	return escolha;
}

int random_escolha(unsigned int num_entradas){
	return rand() % num_entradas;
}

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
  novo_elemento->page.moldura = page.moldura;
          
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
//                           PILHA                           //
///////////////////////////////////////////////////////////////

struct Pilha {

	int topo; /* posicao elemento topo */
	int capa;
	pagina *pElem;

};

void criarpilha( struct Pilha *p, int capa){

   p->topo = -1;
   p->capa = capa;
   p->pElem = (pagina*) malloc (capa * sizeof(pagina));
}
int estavazia ( struct Pilha *p ){

   if( p-> topo == -1 )
      return 1;   
   else
      return 0;   

}

int estacheia ( struct Pilha *p ){

	if (p->topo == p->capa - 1)
		return 1;
	else
		return 0;

}

void empilhar ( struct Pilha *p, pagina page){

	p->topo++;
	p->pElem[p->topo].numero_pagina = page.numero_pagina;
  p->pElem[p->topo].moldura = page.moldura;
}

pagina desempilhar ( struct Pilha *p ){

   pagina aux = p->pElem [p->topo];
   p->topo--;
   return aux;

}

pagina retornatopo ( struct Pilha *p ){

   return p->pElem [p->topo];

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

///////////////////////////////////////////////////////////////
/////////////////////////TESTE DA LISTA///////////////////////
/////////////////////////////////////////////////////////////

  /*Fila *sequencia;         
  pagina page;         
  if ((sequencia = (Fila *) malloc (sizeof (Fila))) == NULL)           
    return -1;         
          
  inicializacao(sequencia);         
  printf ("Inserir um numero:");         
  scanf ("%u", &page.numero_pagina);         
  inserir (sequencia, sequencia->fim, page);         
  printf ("A fila (%de elementos)\n",sequencia->tamanho);         
  printf("---Início da fila\n");         
  exibir (sequencia);     

  //primeiro elemento inserido será exibido         
  printf("---fim da fila\n\n");         
  printf ("Inserir um numero:");         
  scanf ("%u", &page.numero_pagina);         
  inserir(sequencia, sequencia->fim, page);         
  printf ("A fila (%de elementos)\n",sequencia->tamanho);         
  printf("---Início da fila\n");         
  exibir (sequencia);      

  //primeiro elemento inserido será exibido       
  printf("---fim da fila\n\n");          
  printf ("Inserir um numero:");         
  scanf ("%u", &page.numero_pagina);         
  inserir (sequencia, sequencia->fim, page);         
  printf ("A fila (%de elementos)\n",sequencia->tamanho);         
  printf("---Início da fila\n");          
  exibir (sequencia);      

  //primeiro elemento inserido será exibido       
  printf("---fim da fila\n\n");        
        
  printf ("\nO primeiro elemento inserido é removido\n");         
  remover (sequencia);              

  //remoção do primeiro elemento inserido        
  printf ("A fila (%d elementos): \n",sequencia->tamanho);         
  printf("---Início da fila\n");         
  exibir (sequencia);         
  printf("---fim da fila\n\n");         
  return 0;*/


///////////////////////////////////////////////////////////////
/////////////////////////TESTE DA PILHA///////////////////////
/////////////////////////////////////////////////////////////

	/*struct Pilha minhapilha;
	int capacidade, op;
	pagina page;

	printf( "\nCapacidade da pilha? " );
	scanf( "%d", &capacidade );

	criarpilha (&minhapilha, capacidade);

	while( 1 ){ 

		printf("\n1- empilhar (push)\n");
		printf("2- desempilhar (POP)\n");
		printf("3- Mostrar o topo \n");
		printf("4- sair\n");
		printf("\nopcao? ");
		scanf("%d", &op);

		switch (op){

			case 1: //push

				if( estacheia( &minhapilha ) == 1 )

					printf("\nPILHA CHEIA! \n");

				else {

					printf("\nNumero da pagina? ");
					scanf("%u", &page.numero_pagina);
					empilhar(&minhapilha, page);

				}
				break;

			case 2: //pop
				if ( estavazia(&minhapilha) == 1 )

					printf( "\nPILHA VAZIA! \n" );

				else{

					page = desempilhar (&minhapilha);
					printf ( "\nPagina: %u DESEMPILHADA!\n", page.numero_pagina );

				}
				break;

			case 3: // mostrar o topo
				if ( estavazia (&minhapilha) == 1 )

					printf( "\nPILHA VAZIA!\n" );

				else {

					page = retornatopo (&minhapilha);
					printf ( "\nTOPO: %u\n", page.numero_pagina );

				}
				break;

			case 4: 
				exit(0);

			default: printf( "\nOPCAO INVALIDA! \n" );
		}
	}*/
	


///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////



  char *alg = argv[1];
  char *arq = argv[2];
  int tamPagina = atoi(argv[3]);
  int tamMemoriaF = atoi(argv[4]);
  int contador_clock = 0;

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

  int total_paginas = tamMemoriaF / tamPagina; //numero total de paginas/quadros(a tabela de paginas tera no maximo esta quantidade de registros)

  /*
  Criar a tabela de paginas de acordo com o tamanho especificado.
  */
  tabela tabela;
  criaTabela(&tabela, tamPagina);

  Fila *tabela_fifo;              
  if ((tabela_fifo = (Fila *) malloc(sizeof(Fila))) == NULL)           
    return -1;         
          
  inicializacao(tabela_fifo); 


  /*
  Aloca memória para o processo
  */
  memoria_processo memoria_processo;
  criaMemoriaProcesso(&memoria_processo, tamMemoriaF);

  moldura quadros_memoria[total_paginas];//esta sera a estrutura responsavel por simular os quadros que estarao na memoria

  int i;
  for(i = 0; i < total_paginas; i++){
    quadros_memoria[i].esta_na_memoria = false;
  }

  /*
  leitura dos acessos.
  */
  unsigned int endereco;
  char operacao;
  int tamPaginaBytes = tamPagina * pow(2, 10); //tamanho da página em bytes

  //Pego o numero maximo de bits que podem ser usados para identificar as paginas 
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

  // imprimir a tabela de paginas aqui antes da execucao dos algoritmos

  clock_t inicio = clock();

  while(fscanf(arquivo,"%x %c\n", &endereco, &operacao) != EOF){
    contador_clock++;

    printf("----------------Numero da pagina acessada: %d\n", endereco >> s);

    /*Elemento* i_elemento;
    i_elemento = tabela_fifo->inicio;
    while(1){
      printf("Numero da pagina na fila: %u\n", i_elemento->page.numero_pagina);

      if(i_elemento == tabela_fifo->fim)
        break;
    }*/
    
    
    /*//incrementa o número de instruções lidas (semelhante aos pulsos de clock)
    indice = endereco % tabela.num_entradas;
    //achei a pagina, agora vou acessar o conteudo dela
    leituras += operacao == 'R' ? 1 : 0;
    escritas += operacao == 'W' ? 1 : 0;
    if(tabela.paginas[indice].presente){
      hit++;
      //pagina esta na memoria principal e seu endereço é a moldura
      //redefinindo último acesso
      memoria_processo.molduras[tabela.paginas[indice].moldura].ultimo_acesso = contador_clock;
      //página modificada
      memoria_processo.molduras[tabela.paginas[indice].moldura].pagina_modificada = operacao == 'W' ? 1 : 0;
    }else{
      //pagina nao esta na memoria principal
      miss++;
      unsigned int indice_moldura;
      if(!strcmp("fifo", alg) || !strcmp("FIFO", alg)){
        //fifo, me dê a posição da moldura que eu possa fazer a substituição
        indice_moldura = fifo_escolha(&memoria_processo, contador_clock);
      }else if(!strcmp("lru", alg) || !strcmp("LRU", alg)){
        //lru, me dê a posição da moldura que eu possa fazer a substituição
        indice_moldura = lru_escolha(&memoria_processo, contador_clock);
      }else if(!strcmp("random", alg) || !strcmp("RANDOM", alg)){
        //random, me dê a posição da moldura que eu possa fazer a substituição
        indice_moldura = random_escolha(memoria_processo.num_entradas);
      }
      //a página que ocupava esta moldura não está mais presente
      if(memoria_processo.molduras[indice_moldura].pagina){
        memoria_processo.molduras[indice_moldura].pagina->presente = 0;
        memoria_processo.molduras[indice_moldura].pagina_modificada = 0;
      }
      //a página referenciada pelo endereço agora esta presente
      tabela.paginas[indice].presente = 1;
      //a moldura recebe a página e atualiza seus temporizadores
      memoria_processo.molduras[indice_moldura].pagina = &tabela.paginas[indice];
      memoria_processo.molduras[indice_moldura]._carregamento = contador_clock;
      memoria_processo.molduras[indice_moldura].ultimo_acesso = contador_clock;
    }*/
  }

  clock_t fim = clock();

  double tempoExecucao = (double)(fim - inicio) / CLOCKS_PER_SEC;

  // imprimir a tabela de paginas aqui depois da execucao dos algoritmos

  printf("Arquivo de entrada: %s\n", arq);
  printf("Tamanho da memoria: %d KB\n", tamMemoriaF);
  printf("Tamanho das paginas: %d KB\n", tamPagina);
  printf("Tecnica de reposicao: %s\n", alg);
  printf("Paginas lidas: %d\n", leituras);
  printf("Paginas escritas: %d\n", escritas);
  printf("Tempo de execucao: %f\n", tempoExecucao);
  printf("Misses de pagina: %d\n\n", miss);
  printf("Hits de pagina: %d\n\n", hit);
  printf("Tabela: %s\n\n", "imprimir a tabela aqui");
  return 0;

}
