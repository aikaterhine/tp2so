#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


///////////////////////////////////////////////////////////////
//                         tabela                           //
/////////////////////////////////////////////////////////////

typedef struct pagina{
	char presente;
	unsigned int moldura;
} pagina;

typedef struct tabela{
	unsigned int num_entradas;
	pagina *paginas;
} tabela;

typedef struct moldura{
	pagina *pagina;
	unsigned int ultimo_acesso; //Os clocks podem ser considerados como a colocação da intrução lida
	unsigned int _carregamento; //O instante de carregamento para a memória
	char pagina_modificada;
} moldura;

typedef struct memoria_processo{
	unsigned int num_entradas;
	moldura *molduras;
} memoria_processo;



void criaTabela(tabela *tabela, int tamPagina){
	tabela->num_entradas = 4194304 / tamPagina; //os enderecos fornecidos sao de 32 bits, logo temos 544288kb mapeados
    tabela->paginas = (pagina*) malloc(sizeof(pagina) * tabela->num_entradas);
    for(int i = 0; i < tabela->num_entradas; i++){
    	tabela->paginas[i].presente = 0;
		tabela->paginas[i].moldura = 0;
    }
}

void criaMemoriaProcesso(memoria_processo *memoria_processo, int tamMemoriaF){
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
char *dado;         
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
int inserir (Fila * sequencia, Elemento * atual, char *dado){         
  Elemento *novo_elemento;         
  if ((novo_elemento = (Elemento *) malloc (sizeof (Elemento))) == NULL)
  return -1;         
  if ((novo_elemento->dado = (char *) malloc (50 * sizeof (char))) == NULL)
  return -1;         
  strcpy (novo_elemento->dado, dado);         
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
  atual-> seguinte = novo_elemento;         }         
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
  free (remov_elemento->dado);         
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
  printf(" %s ", atual->dado);           
  atual = atual->seguinte;         
  }       
}



///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//                           PILHA                           //
///////////////////////////////////////////////////////////////

struct Pilha {

	int topo; /* posicao elemento topo */
	int capa;
	float *pElem;

};

void criarpilha( struct Pilha *p, int c ){

   p->topo = -1;
   p->capa = c;
   p->pElem = (float*) malloc (c * sizeof(float));

}
int estavazia ( struct Pilha *p ){

   if( p-> topo == -1 )

      return 1;   // true

   else

      return 0;   // false

}

int estacheia ( struct Pilha *p ){

	if (p->topo == p->capa - 1)

		return 1;

	else

		return 0;

}

void empilhar ( struct Pilha *p, float v){

	p->topo++;
	p->pElem [p->topo] = v;

}

float desempilhar ( struct Pilha *p ){

   float aux = p->pElem [p->topo];
   p->topo--;
   return aux;

}

float retornatopo ( struct Pilha *p ){

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
  char *nome;         
  if ((sequencia = (Fila *) malloc (sizeof (Fila))) == NULL)           
  return -1;         
  if ((nome = (char *) malloc (50 * sizeof (char))) == NULL)           
  return -1;         
  inicializacao(sequencia);         
  printf ("Inserir uma palavra:");         
  scanf ("%s", nome);         
  inserir (sequencia, sequencia->fim, nome);         
  printf ("A fila (%de elementos)\n",sequencia->tamanho);         
  printf("\nInício de la fila [ ");         
  exibir (sequencia);     

  //primeiro elemento inserido será exibido         
  printf(" ] fim de la fila\n\n");         
  printf ("Inserir uma palavra:");         
  scanf ("%s", nome);         
  inserir(sequencia, sequencia->fim, nome);         
  printf ("A fila (%de elementos)\n",sequencia->tamanho);         
  printf("\nInício da fila [ ");         
  exibir (sequencia);      

  //primeiro elemento inserido será exibido       
  printf(" ] fim da fila\n\n");         
  printf ("Inserir uma palavra:");         
  scanf ("%s", nome);         
  inserir (sequencia, sequencia->fim, nome);         
  printf ("A fila (%de elementos)\n",sequencia->tamanho);         
  printf("\nInício de la fila [ ");         
  exibir (sequencia);      

  //primeiro elemento inserido será exibido       
  printf(" ] fim da fila\n\n");         
        
  printf ("\nO primeiro elemento inserido é removido\n");         
  remover (sequencia);              

  //remoção do primeiro elemento inserido        
  printf ("A fila (%d elementos): \n",sequencia->tamanho);         
  printf("\nInício da fila [ ");         
  exibir (sequencia);         
  printf(" ] fim da fila\n\n");         
  return 0; */


///////////////////////////////////////////////////////////////
/////////////////////////TESTE DA PILHA///////////////////////
/////////////////////////////////////////////////////////////

	/*struct Pilha minhapilha;
	int capacidade, op;
	float valor;

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

					printf("\nVALOR? ");
					scanf("%f", &valor);
					empilhar (&minhapilha, valor);

				}
				break;

			case 2: //pop
				if ( estavazia(&minhapilha) == 1 )

					printf( "\nPILHA VAZIA! \n" );

				else{

					valor = desempilhar (&minhapilha);
					printf ( "\n%.1f DESEMPILHADO!\n", valor );

				}
				break;

			case 3: // mostrar o topo
				if ( estavazia (&minhapilha) == 1 )

					printf( "\nPILHA VAZIA!\n" );

				else {

					valor = retornatopo (&minhapilha);
					printf ( "\nTOPO: %.1f\n", valor );

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

  if(tamMemoriaF < 128 || tamMemoriaF > 16384){
  	printf("Erro ao identificar tamanho total da memória física: valor informado menor que 128 ou maior que 16MB");
  	return 0;
  }

  /*
  Criar a tabela de paginas de acordo com o tamanho especificado.
  */
  tabela tabela;
  criaTabela(&tabela, tamPagina);

  /*
  Aloca memória para o processo
  */
  memoria_processo memoria_processo;
  criaMemoriaProcesso(&memoria_processo, tamMemoriaF);

  /*
  leitura dos acessos.
  */
  unsigned int endereco;
  char operacao;
  int tamPaginaBytes = tamPagina * pow(2, 10); //tamanho da página em bytes

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
    //incrementa o número de instruções lidas (semelhante aos pulsos de clock)
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
    }
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
