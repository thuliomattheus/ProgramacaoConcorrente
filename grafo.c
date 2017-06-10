#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct grafo{
	int n_vertices; //Numero de vertices
	int **arestas; 	//Linhas representam os vertices e as colunas as ligacoes
	int **pesos;	//Linhas representam os vertices e as colunas os pesos
	int *grau;		//Grau de cada aresta, ou seja, quantidade de ligacoes que aquela aresta possui
} Grafo;

Grafo* criar(int n_vertices); 					//Criar grafo de "n_vertices" vertices

int inserir(Grafo* grafo, int origem, int destino, int peso); 	//Inserir aresta entre a origem e o destino

int remover(Grafo* grafo, int origem, int destino); 			//Remover aresta do grafo
																//Caso remova uma aresta multipla, 
																//Todas serão removidas

void liberar(Grafo* grafo); 									//Liberar alocacoes de memoria

int menorDistancia(int* distancia, int* visitado, int n_vertices);   //Menor distancia entre dois vertices

void menorCaminho(Grafo *grafo, int inicial, int* anterior, int* distancia);   //Menor caminho entre dois vertices

void imprimirGrafo(Grafo* grafo);    //Mostrar arestas do grafo

void imprimirDistancias(Grafo* grafo, int inicial);

void imprimirMenorDistancia(Grafo* grafo, int inicial, int final);

void preencherGrafo(Grafo* grafo);

int main(){

	Grafo* grafo;
	srand(time(NULL));

	int n = 1;

	printf("A quantidade de vertices do grafo sera igual a uma potencia de 2\n");
	printf("Digite o expoente que sera utilizado para a criacao. n deve ser no minimo 2.\n");
	printf("\n	Exemplo: n = 2, vertices = 4;     n=3, vertices = 8...\n");

	while(n<2){
		printf("n deve ser no minimo 2.\n");
		printf("n = ");
		scanf("%d", &n);
	}

	printf("\n\n");

	int n_vertices = (int) pow(2, n);
	grafo = criar(n_vertices);

	preencherGrafo(grafo);

	printf("n = %d", grafo->n_vertices);

	imprimirGrafo(grafo);
	imprimirDistancias(grafo, 5);
	imprimirMenorDistancia(grafo, 2, 3);
	liberar(grafo);
}

Grafo* criar(int n_vertices){
	Grafo* grafo = (Grafo*) malloc(sizeof(struct grafo));           // Alocando o grafo na memória
	if(grafo != NULL){                                              // Caso a alocação tiver funcionado

		grafo->n_vertices = n_vertices;

		grafo->arestas = (int**)malloc(n_vertices * sizeof(int*));  // Alocando as arestas
		grafo->pesos = (int**)malloc(n_vertices * sizeof(int*));    // Alocando os pesos

		grafo->grau = (int*)calloc(n_vertices, sizeof(int));        // Alocando o grau dos vertices com 0

		for (int i = 0; i < n_vertices; i++){
			grafo->arestas[i] = (int*)malloc(n_vertices * sizeof(int));     //Alocando a lista de adj.
			grafo->pesos[i] = (int*)malloc(n_vertices * sizeof(int));       //         ||
		}
	}

	return grafo;
}

int inserir(Grafo* grafo, int origem, int destino, int peso){

	// Verificações de existência do grafo

	if(grafo == NULL)
		return 0;

	if(origem < 0 || origem > grafo->n_vertices)
		return 0;

	if(destino < 0 || destino > grafo->n_vertices)
		return 0;


	//Como é um grafo não direcionado, teremos que inserir a ida e a volta

	grafo->arestas[origem][grafo->grau[origem]] = destino;
	grafo->pesos[origem][grafo->grau[origem]] = peso;
	grafo->grau[origem]++;

	grafo->arestas[destino][grafo->grau[destino]] = origem;
	grafo->pesos[destino][grafo->grau[destino]] = peso;
	grafo->grau[destino]++;


	return 1;
}

int remover(Grafo* grafo, int origem, int destino){

	// Verificações de existência do grafo

	if(grafo == NULL)
		return 0;

	if(origem < 0 || origem > grafo->n_vertices)
		return 0;

	if(destino < 0 || destino > grafo->n_vertices)
		return 0;

	/*

		Verifica se existe uma aresta entre a origem e o destino.

		Caso não exista, retorne um erro '0'.

		Caso exista, decremente o grau do vértice e substitui a aresta a ser apagada pela última.
		O mesmo é feito para o peso da aresta.

		Assim como na inserção de uma nova aresta, eu preciso adicionar o caminho inverso, na remoção
		o processo deve ser análogo.



		Exemplo Prático:
			Caso o vértice 0, se ligue com [1][2][3] respectivamente e eu queira remover a aresta de 
			0 até 1...

			O grau do vértice 0 que era 3, agora será 2.
			E o vetor de arestas que era [1][2][3] se tornará [3][2].
			O mesmo para o peso.

	*/

	int i = 0;
	while(i < grafo->grau[origem] && grafo->arestas[origem][i] != destino)
		i++;

	if(i == grafo->grau[origem]) //Elemento nao encontrado
		return 0;

	grafo->grau[origem]--;
	grafo->arestas[origem][i] = grafo->arestas[origem][grafo->grau[origem]];
	grafo->pesos[origem][i] = grafo->pesos[origem][grafo->grau[origem]];

	printf("\nAresta removida %d - %d\n", origem, destino);

	//Remoção do "caminho inverso"

	//Como só pode existir uma aresta entre dois vértices,
	//após a primeira recursão, o retorno será 0

	return remover(grafo, destino, origem);
}

void liberar(Grafo* grafo){
	if(grafo != NULL){
		for (int i = 0; i < grafo->n_vertices; i++){
			free(grafo->arestas[i]);              //Liberando a lista de adjacência
			free(grafo->pesos[i]);                //             ||
		}

		free(grafo->arestas);					  //Liberando o vetor de arestas
		free(grafo->pesos);						  //Liberando o vetor de pesos
		free(grafo->grau);
		free(grafo);
	}
}

int menorDistancia(int* distancia, int* visitado, int n_vertices){
	
	int menor = -1, primeiro = 1;

	for(int i = 0; i < n_vertices; i++){
		if(distancia[i] >= 0 && visitado[i] == 0){
			if(primeiro){
				menor = i;
				primeiro = 0;
			}
			else{
				if(distancia[menor] > distancia[i])
					menor = i;
			}
		}
	}

	return menor; //index do vertice que tem a menor distancia e que nao foi visitado
}

void menorCaminho(Grafo *grafo, int inicial, int* anterior, int* distancia){

	// u será o índice do vértice de menor distância.

	int qtd_nao_visitados, n_vertices, u, vizinho_u, *visitados;

	qtd_nao_visitados = n_vertices = grafo->n_vertices;

	visitados = (int*)malloc(n_vertices*sizeof(int));   //Alocação dos vértices já visitados


	// Inicialização dos vetores auxiliares anterior, distancia e visitados

	for(int i=0; i < n_vertices; i++){
		anterior[i]  = -1;
		distancia[i] = -1;
		visitados[i] = 0;
	}


	// Após a inicialização, diremos que a distância de inicial pra ele mesmo, é 0

	distancia[inicial] = 0;


	while(qtd_nao_visitados > 0){
		u = menorDistancia(distancia, visitados, n_vertices);
		if(u == -1) // grafo desconexo
			break; 
		visitados[u] = 1;
		qtd_nao_visitados--;

		for(int i = 0; i < grafo->grau[u]; i++){ //Visitando os vizinhos de u
			vizinho_u = grafo->arestas[u][i];
			if(distancia[vizinho_u] < 0){ //Se ninguem chegou nele ainda
				distancia[vizinho_u] = distancia[u] + grafo->pesos[u][i];
				anterior[vizinho_u] = u;
			}
			else{ //Verifica se a distancia atual maior e maior
				if(distancia[vizinho_u] > distancia[u] + grafo->pesos[u][i]){
					distancia[vizinho_u] = distancia[u] + grafo->pesos[u][i];
					anterior[vizinho_u] = u;
				}
			}
		}
	}

	free(visitados);
}


void imprimirGrafo(Grafo* grafo){

	for(int i = 0; i < grafo->n_vertices; i++){
		
		for(int j=0; j < grafo->grau[i]; j++){
			printf("Aresta %d-%d      PESO = %d\n", i, grafo->arestas[i][j], grafo->pesos[i][j]);
		}
		printf("\n");
	}

}


void imprimirDistancias(Grafo* grafo, int inicial){

	int anterior[grafo->n_vertices], distancia[grafo->n_vertices];

	menorCaminho(grafo, inicial, anterior, distancia);

	printf("\n");

	for(int i=0; i < grafo->n_vertices; i++){
		printf("Menor distancia entre %d e %d = %d\n", inicial, i, distancia[i]);
	}

	printf("\n");

}


void imprimirMenorDistancia(Grafo* grafo, int inicial, int final){

	int anterior[grafo->n_vertices], distancia[grafo->n_vertices];

	menorCaminho(grafo, inicial, anterior, distancia);

	printf("\n");

	for(int i=0; i < grafo->n_vertices; i++){
		if(i==final)
			printf("Menor distancia entre %d e %d = %d\n", inicial, i, distancia[i]);
	}

	printf("\n");

}

void preencherGrafo(Grafo* grafo){

	int n_vertices = grafo->n_vertices;

	for(int i=0; i < n_vertices; i++){

		// Primeiro vamos preencher a ligação entre a camada superior e a camada do meio 

		if(i < n_vertices/4){
			for(int j = n_vertices/4; j< n_vertices*3/4; j++){
				inserir(grafo, i, j, rand()%10);
			}
		}

		// Por fim, preencheremos a ligação entre as camadas do meio e inferior

	else if(i >= n_vertices*3/4){
			for(int j = n_vertices/4; j< n_vertices*3/4; j++){
				inserir(grafo, i, j, rand()%10);
			}	
		}	
	}

}