#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela.c" // RENOMEAR
#include "algs.c" // RENOMEAR

///////////////////////////////////////////////////////////////
//                           PILHA                           //
///////////////////////////////////////////////////////////////

struct Pilha {

	int topo; /* posiÃ§Ã£o elemento topo */
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
  int val;

  switch (*alg) {
    case ("lru"):
      val = 0;
      break;
    case ("2a"):
      val = 0;
      break;
    case ("fifo"):
      val = 0;
      break;
    case ("new"):
      val = 0;
      break;
    default:
      val = 1;
  }
  return val;
}

int main (int argc, char *argv[]){

///////////////////////////////////////////////////////////////
/////////////////////////TESTE DA PILHA///////////////////////
/////////////////////////////////////////////////////////////

/*	struct Pilha minhapilha;
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
	}
	
*/

///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////



    char *alg = argv[1];
    char *arq = argv[2];
    int tamPagina = atoi(argv[3]);
    int tamMemoriaF = atoi(argv[4]);
    int clock = 0;

    /*
    Abertura e controle dos parametros de entrada
    */
    FILE *arquivo = fopen(arq,"r");

    if(arquivo == NULL){
        printf("Erro ao ler arquivo.\n");
        return 1;
    }

  	//Verificando se a política de substituição é válida
  	if(!valida_entrada()){
  			printf("Erro ao identificar política de substituição.\n");
  			return 0;
  	}

    //Verificando se os tamanhos de pagina e memória são válidos
  	if((tamPagina % 2 || tamPagina <= 0)){
  		printf("Erro ao identificar tamanho da página.\n");
  		return 0;
  	}
  	if(tamPagina < 2 || tamPagina > 64){
  		printf("Erro ao identificar tamanho da página: valor informado menor que 2 ou maior que 64.");
  		return 0;
  	}
    	if((tamMemoriaF % 2 || tamMemoriaF <= 0)){
    		printf("Erro ao identificar tamanho total da memória física.\n");
    		return 0;
    	}
  	if(tamMemoriaF < 128 || tamMemoriaF > 16384){
  		printf("Erro ao identificar tamanho total da memória física: valor informado menor que 128 ou maior que 16MB");
  		return 0;
  	}


    /*
    Criar a tabela de páginas de acordo com o tamanho especificado.
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
    unsigned int tmpPOS;
    char tmpOP;
    int tamPagina_real = tamPagina * pow(2, 10); //tamanho da página em bytes

    unsigned int indice;
    unsigned int pageFault = 0;
    unsigned int paginas_lidas = 0;
    unsigned int paginas_escritas = 0;

    printf("Executando simulador ...\n");

    while(fscanf(arquivo,"%x %c\n", &tmpPOS, &tmpOP) != EOF){
        clock++;
        //incrementa o número de instruções lidas (semelhante aos pulsos de clock)
        indice = tmpPOS % tabela.num_entradas;
        //achei a pagina, agora vou acessar o conteudo dela
        paginas_lidas += tmpOP == 'R' ? 1 : 0;
        paginas_escritas += tmpOP == 'W' ? 1 : 0;
        if(tabela.paginas[indice].presente){
            //pagina esta na memoria principal e seu endereço é a moldura
            //redefinindo último acesso
            memoria_processo.molduras[tabela.paginas[indice].moldura].ultimo_acesso = clock;
            //página modificada
            memoria_processo.molduras[tabela.paginas[indice].moldura].pagina_modificada = tmpOP == 'W' ? 1 : 0;
        }else{
            //pagina nao esta na memoria principal
            pageFault++;
            unsigned int indice_moldura;
            if(!strcmp("fifo", alg) || !strcmp("FIFO", alg)){
                //fifo, me dê a posição da moldura que eu possa fazer a substituição
                indice_moldura = fifo_escolha(&memoria_processo, clock);
            }else if(!strcmp("lru", alg) || !strcmp("LRU", alg)){
                //lru, me dê a posição da moldura que eu possa fazer a substituição
                indice_moldura = lru_escolha(&memoria_processo, clock);
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
            memoria_processo.molduras[indice_moldura]._carregamento = clock;
            memoria_processo.molduras[indice_moldura].ultimo_acesso = clock;

        }
    }

    printf("Arquivo de entrada: %s\n", arq);
    printf("Tamanho da memoria: %d KB\n", tamMemoriaF);
    printf("Tamanho das páginas: %d KB\n", tamPagina);
    printf("Tecnica de substiuição: %s\n", alg);
    printf("Páginas lidas: %d\n", paginas_lidas);
    printf("Paginas escritas: %d\n", paginas_escritas);
    printf("Faltas de pagina: %d\n\n", pageFault);

    return 0;
  }
