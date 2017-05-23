#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(void) {
   int n, local_n, escalar, comm_sz, my_rank;
   int* vetor1 = NULL, vetor2 = NULL, local_vetor1, local_vetor2;
   int local_soma = 0, total_soma = 0;

   MPI_Init(NULL, NULL); 

   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 

   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

   //Calcula o local_n
   if (my_rank == 0) {
      printf("Digite o tamanho dos vetores: ");
      scanf("%d", &n);
      printf("Digite um escalar: ");
      scanf("%d", &escalar);
      local_n = n/comm_sz;
   }

   //Distribui o local_n e o n
    MPI_Bcast(&local_n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&escalar, 1, MPI_INT, 0, MPI_COMM_WORLD);

   //Aloca os vetores
   vetor1 = (int*)malloc(n*sizeof(int));
   vetor2 = (int*)malloc(n*sizeof(int));
   local_vetor1 = (int*)malloc(local_n*sizeof(int));
   local_vetor2 = (int*)malloc(local_n*sizeof(int));

   //Preenche os vetores e os distribui
   if(my_rank == 0){
      printf("Digite o primeiro vetor\n");

      for(int i=0;i<n;i++)
       scanf("%d",&vetor1[i]);

      printf("Digite o segundo vetor\n");
      
      for(int i=0;i<n;i++)
       scanf("%d",&vetor2[i]);
      
      MPI_Scatter(vetor1, local_n, MPI_INT, 
            local_vetor1, local_n, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Scatter(vetor2, local_n, MPI_INT, 
            local_vetor2, local_n, MPI_INT, 0, MPI_COMM_WORLD);

      free(vetor1);
      free(vetor2);
   }
   else{
      MPI_Scatter(vetor1, local_n, MPI_INT, 
            local_vetor1, local_n, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Scatter(vetor2, local_n, MPI_INT, 
            local_vetor2, local_n, MPI_INT, 0, MPI_COMM_WORLD);
   }

   //Calcula as somas locais
   for(i=0;i<local_n;i++){
      local_vetor1[i] *= escalar;
      local_vetor1[i] *= local_vetor2[i];
      local_soma += local_vetor1[i];
   }

   //Adiciona as somas locais no resultado
   MPI_Reduce(&local_soma, &total_soma, 1, MPI_INT, MPI_SUM, 0,
         MPI_COMM_WORLD);

   if(my_rank == 0){
      printf("O resultado final: %d\n",total_soma);
      free(local_vetor1);
      free(local_vetor2);
   }

   /* Finaliza o MPI */
   MPI_Finalize(); 

   return 0;
}  /* main */