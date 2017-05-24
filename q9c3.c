#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(){

	int n, escalar, my_rank, comm_sz;
	double prod_esc = 0;

	MPI_Init(NULL, NULL); 
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	if(my_rank == 0){

		printf("Digite o tamanho dos vetores : ");
		scanf("%d", &n);

		double vetor1[n], vetor2[n];

		//Leitura do primeiro vetor
		for(int i=0; i<n; i++){
			printf("Digite o %do elemento do vetor 1 : ", i+1);
			scanf("%lf" , &vetor1[i]);
		}

		printf("\n");

		//Leitura do segundo vetor
		for(int i=0; i<n; i++){
			printf("Digite o %do elemento do vetor 2 : ", i+1);
			scanf("%lf" , &vetor2[i]);
		}

		//Leitura do escalar
		printf("\nDigite o escalar : ");
		scanf("%d" , &escalar);


		printf("\n");
	}

	//Impressao do primeiro vetor
	for(int i=0; i<n; i++){
		vetor1[i] *= escalar;
		printf("%do elemento do vetor 1 : %lf\n", i+1, vetor1[i]);
	}

	printf("\n");

	//Impressao do segundo vetor
	for(int i=0; i<n; i++){
		printf("%do elemento do vetor 2 : %lf\n", i+1, vetor2[i]);
	}

	//Calculo do produto escalar
	for(int i=0; i<n; i++){
		prod_esc += vetor1[i]*vetor2[i];
	}	

	printf("\nO produto escalar entre os vetores eh : %lf\n", prod_esc);

	MPI_Finalize();

	return 0;
}