#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

typedef struct grafo{
	unsigned long int n_vertices; //Numero de vertices
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

int menorDistancia(int* distancia, int* visitado, int n_vertices, int thread_count);   //Menor distancia entre dois vertices

void menorCaminho(Grafo *grafo, int inicial, int* anterior, int* distancia, int thread_count);   //Menor caminho entre dois vertices

void imprimirGrafo(Grafo* grafo);    //Mostrar arestas do grafo

void imprimirDistancias(Grafo* grafo, int inicial, int thread_count);

void imprimirMenorDistancia(Grafo* grafo, int inicial, int final, int thread_count);

void preencherGrafo(Grafo* grafo);

int main(int argc, char* argv[]){
	
	int thread_count = strtol(argv[1], NULL, 10);
	
	Grafo* grafo;
	srand(time(NULL));

	int n = 1;

	printf("A quantidade de vertices do grafo sera igual a uma potencia de 2\n");
	printf("Digite o expoente que sera utilizado para a criacao. n deve ser no minimo 2.\n");
	printf("\n	Exemplo: n = 2, vertices = 4;     n=3, vertices = 8...\n");

	
	printf("n deve ser no minimo 2.\n");	
		FILE *myfile;
		myfile = fopen("ValorN.txt","r");
		fscanf(myfile,"%d",&n);
		fclose(myfile);
	printf("n = %d",n);
	
	omp_set_num_threads(thread_count);
	
	printf("\n\n");

	int n_vertices = (int) pow(2, n);
	
	double t1c=omp_get_wtime();
	
	grafo = criar(n_vertices);	
	preencherGrafo(grafo);
	
	double t2c=omp_get_wtime();
	printf("Tempo de criaçao foi %lf\n",t2c-t1c);
	
//	imprimirGrafo(grafo);
	imprimirDistancias(grafo, 0, thread_count);
//	imprimirMenorDistancia(grafo, 2, 3);
	//liberar(grafo);
	printf("========================================\n");
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

int menorDistancia(int* distancia, int* visitado, int n_vertices, int thread_count){
	
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

void menorCaminho(Grafo *grafo, int inicial, int* anterior, int* distancia, int thread_count){

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
	//********************************
	double t1,t2;
	
	omp_lock_t writelock;
	omp_init_lock(&writelock);
	
	t1 = omp_get_wtime();	
#	pragma omp parallel for schedule(static,1) \
	shared(distancia, visitados, n_vertices, thread_count) private(u,vizinho_u)
	for (int aux = qtd_nao_visitados; aux > 0; aux--){
		
		omp_set_lock(&writelock); //área critica		
		while(1){
			u = menorDistancia(distancia, visitados, n_vertices,thread_count);				
			visitados[u] = 1;
			if (u != -1){			
				break;}
		
		}//fim do critical	
		omp_unset_lock(&writelock); //unlock
		
		//printf("Thread: %d Loop: %d e U=%d\n",omp_get_thread_num() ,aux,u);

		for(int i = 0; i < grafo->grau[u]; i++){ //Visitando os vizinhos de u
			vizinho_u = grafo->arestas[u][i];

			if(vizinho_u!=inicial){
				if(distancia[vizinho_u] < 0){ //Se ninguem chegou nele ainda
					distancia[vizinho_u] = distancia[u] + grafo->pesos[u][i];
					anterior[vizinho_u] = u;
					//if(vizinho_u==n_vertices-1){
					//	double t3=omp_get_wtime();
					//	printf("Em t=%lf\n",t3-t1);
					//	printf("1stTime:-%d- de anterior %d e antianterior%d\n",u,anterior[u],anterior[anterior[u]]);}
				}else{ //Verifica se a distancia atual é maior
					if(distancia[vizinho_u] > distancia[u] + grafo->pesos[u][i]){
						distancia[vizinho_u] = distancia[u] + grafo->pesos[u][i];
						anterior[vizinho_u] = u;
						//if(vizinho_u==n_vertices-1){
						//	double t3=omp_get_wtime();
						//	printf("Em t=%lf\n",t3-t1);
						//	printf("UPDATE:-%d- de anterior %d e antianterior%d e antianti%d\n",u,anterior[u],anterior[anterior[u]],anterior[anterior[anterior[u]]]);}
					}
				}				
			}
		}
	}		
	t2 = omp_get_wtime();
	
	omp_destroy_lock(&writelock);
	printf("\nPARALELO para %d::O tempo foi %lf\n",thread_count,t2-t1);
	//free(visitados);
}


void imprimirGrafo(Grafo* grafo){
	int n = grafo->n_vertices;
	for(int i = 0; i < grafo->n_vertices; i++){
		//if(i==0 || i== grafo->n_vertices-1 || (i>0&&i<n/4) ){
			for(int j=0; j < grafo->grau[i]; j++){
				printf("Aresta %d-%d      PESO = %d\n", i, grafo->arestas[i][j], grafo->pesos[i][j]);
			}
			printf("\n");
		//}	
	}
}


void imprimirDistancias(Grafo* grafo, int inicial, int thread_count){

	int anterior[grafo->n_vertices], distancia[grafo->n_vertices];
			
	menorCaminho(grafo, inicial, anterior, distancia, thread_count);
		
	printf("\n");
/*
	for(int i=0; i < grafo->n_vertices; i++){
		printf("Menor distancia entre %d e %d = %d\n", inicial, i, distancia[i]);
	}*/
	int x = grafo->n_vertices - 1;
	printf("Menor distancia entre %d e %d = %d\n", inicial, x, distancia[x]);
	
}


void imprimirMenorDistancia(Grafo* grafo, int inicial, int final, int thread_count){

	int anterior[grafo->n_vertices], distancia[grafo->n_vertices];

	menorCaminho(grafo, inicial, anterior, distancia, thread_count);

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
				inserir(grafo, i, j, rand()%99 + 1);
			}
		}

		// Por fim, preencheremos a ligação entre as camadas do meio e inferior

	else if(i >= n_vertices*3/4){
			for(int j = n_vertices/4; j< n_vertices*3/4; j++){
				inserir(grafo, i, j, rand()%99 + 1);
			}	
		}	
	}

}