#include "tabela.h"
#include <math.h>
/*
Este arquivo será responsavel por implementar funções referentes a tabela de páginas.
*/

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
