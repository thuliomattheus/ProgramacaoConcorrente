#include <stdio.h>
#include <stdlib.h>

typedef struct grafo Grafo;

struct grafo{
	int n_vertices; //Numero de vertices
	int grau_max; 	//Numero maximo de ligacoes para cada vertice
	int** arestas; 	//Linhas representam os vertices e as colunas as ligacoes
	int** pesos;	//Pesos das ligacoes
	int* grau;		//Grau de cada aresta,ou seja, quantidade de ligacoes q aquela aresta possui
};

Grafo* criar(int n_vertices, int grau_max); 					//Criar grafo
int inserir(Grafo* grafo, int origem, int destino, int peso); 	//Inserir aresta no grafo
int remover(Grafo* grafo, int origem, int destino); 			//Remover aresta do grafo
void liberar(Grafo* grafo); 									//Liberar alocacoes de memoria

int menorDistancia(int* distancia, int* visitado, int n_vertices);
int menorCaminho(Grafo *grafo, int inicial, int* anterior, int* distancia);

void imprimir(int* menorCaminho, int t);

int main(){
	Grafo *grafo = criar(5, 5);
	inserir(grafo, 0, 1, 2);
	inserir(grafo, 1, 3, 1);
	inserir(grafo, 1, 2, 3);
	inserir(grafo, 2, 4, 2);
	inserir(grafo, 3, 0, 5);
	inserir(grafo, 3, 4, 2);
	inserir(grafo, 4, 1, 1);

	int anteriror[5], distancia[5];
	menorCaminho(grafo, 0, anteriror, distancia);

	imprimir(anteriror, 5);

	liberar(grafo);
}

Grafo* criar(int n_vertices, int grau_max){
	Grafo* grafo = (Grafo*) malloc(sizeof(struct grafo));
	if(grafo != NULL){
		int i;
		grafo->n_vertices = n_vertices;
		grafo->grau_max = grau_max;
		grafo->arestas = (int**)malloc(n_vertices * sizeof(int*));
		grafo->pesos = (int**)malloc(n_vertices * sizeof(int*));
		grafo->grau = (int*)calloc(n_vertices, sizeof(int));
		for (i = 0; i < n_vertices; i++){
			grafo->arestas[i] = (int*)malloc(grau_max*sizeof(int));
			grafo->pesos[i] = (int*)malloc(grau_max*sizeof(int));
		}
	}
}

int inserir(Grafo* grafo, int origem, int destino, int peso){
	if(grafo == NULL)
		return 0;

	if(origem < 0 || origem > grafo->n_vertices)
		return 0;

	if(destino < 0 || destino > grafo->n_vertices)
		return 0;

	grafo->arestas[origem][grafo->grau[origem]] = destino;
	grafo->pesos[origem][grafo->grau[origem]] = peso;
	grafo->grau[origem]++;

	return 1;
}

int remover(Grafo* grafo, int origem, int destino){
	if(grafo == NULL)
		return 0;

	if(origem < 0 || origem > grafo->n_vertices)
		return 0;

	if(destino < 0 || destino > grafo->n_vertices)
		return 0;

	int i = 0;
	while(i < grafo->grau[origem] && grafo->arestas[origem][i] != destino)
		i++;

	if(i == grafo->grau[origem]) //Elemento nao encontrado
		return 0;

	grafo->grau[origem]--;
	grafo->arestas[origem][i] = grafo->arestas[origem][grafo->grau[origem]];
	grafo->pesos[origem][i] = grafo->pesos[origem][grafo->grau[origem]];

	return 1;
}

void liberar(Grafo* grafo){
	if(grafo != NULL){
		int i;
		for (i = 0; i < grafo->n_vertices; i++){
			free(grafo->arestas[i]);
			free(grafo->pesos[i]);
		}
		free(grafo->arestas);
		free(grafo->pesos);
		free(grafo->grau);
		free(grafo);
	}
}

int menorDistancia(int* distancia, int* visitado, int n_vertices){
	int i, menor = -1, primeiro = 1;
	for(i = 0; i < n_vertices; i++){
		if(distancia[i] >= 0 && visitado[i] == 0){
			if(primeiro){
				menor = i;
				primeiro = 0;
			}else{
				if(distancia[menor] > distancia[i])
					menor = i;
			}
		}
	}
	return menor; //index do vertice que tem a menor distancia e que nao foi visitado
}

int menorCaminho(Grafo *grafo, int inicial, int* anterior, int* distancia){
	int i, cont, n_vertices, u, vizinho_u, *visitados;  
	cont = n_vertices = grafo->n_vertices;
	visitados = (int*)malloc(n_vertices*sizeof(int));
	for(int i=0; i < n_vertices; i++){
		anterior[i]  = -1;
		distancia[i] = -1;
		visitados[i] = 0;
	}

	distancia[inicial] = 0;
	while(cont > 0){
		u = menorDistancia(distancia, visitados, n_vertices);
		if(u == -1) // grafo desconexo
			break; 
		visitados[u] = 1;
		cont--;

		for(i=0; i<grafo->grau[u]; i++){ //Visitando os vizinhos de u
			vizinho_u = grafo->arestas[u][i];
			if(distancia[i] < 0){ //Se ninguem chegou nele ainda
				distancia[vizinho_u] = distancia[u] + grafo->pesos[u][i];
				anterior[vizinho_u] = u;
			}else{ //Verifica se a distancia atual maior e maior
				if(distancia[vizinho_u] > distancia[u] + grafo->pesos[u][i]){
					distancia[vizinho_u] = distancia[u] + grafo->pesos[u][i];
					anterior[vizinho_u] = u;
				}
			}
		}
	}

	free(visitados);

}

void imprimir(int* menorCaminho, int t){
	int i;
	for(i = 0; i < t; i++){
		if(i == t-1)
			printf("%d \n", menorCaminho[i]);
		else
			printf("%d -> \n", menorCaminho[i]);
	}
}