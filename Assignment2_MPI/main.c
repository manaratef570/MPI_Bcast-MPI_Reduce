#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int main(int argc , char * argv[])
{
   int my_rank;		/* rank of process	*/
	int p;			/* number of process	*/
  int n;
  int i;
  int *arr;
  int globalSum = 0,localSum = 0;
  int globalMean = 0;
  int localSumOfD = 0 ,globalSumOfD = 0;
  int remSum;
  int differences = 0;

  time_t t;
	MPI_Status status;	/* return status for 	*/
				/* recieve		*/

	/* Start up MPI */
	MPI_Init( &argc , &argv );

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

    if( my_rank == 0)
    {
        //Read n from the user
          printf("Enter number of elements per each process : ");
          scanf("%d",&n);
	}

	//Broadcast n to each slave process
    MPI_Bcast(&n , 1 , MPI_INT, 0,MPI_COMM_WORLD);

    if( my_rank != 0)
	{

    arr =  malloc ((n) * sizeof (int));

     //Generate n random elements
    srand((unsigned) time(&t));
         i=0;
    for(;i<n;i++){
      arr[i]=(rand() % (my_rank*n*20));
    //  printf("nums %d is %d\n",my_rank,arr[i] );
      localSum += arr[i];
    }

     printf("SUM at rank %d is %d\n",my_rank,localSum );

	}

  //Share this local sum with the processes
   MPI_Allreduce(&localSum,&globalSum,1,MPI_INT,MPI_SUM,MPI_COMM_WORLD);

   printf("SUM at rank %d is %d\n",my_rank,globalSum );

    if(my_rank!=0){

     //Calculate the global mean
      globalMean= (globalSum /( n * (p-1)));

      //Calculate local sum of squared differences
      i=0;
   for(;i<n;i++){
    differences = arr[i]-globalMean;
    localSumOfD+=(differences*differences);
    }


     }


    //Share this local sum of squared differences with the master
     MPI_Reduce(&localSumOfD,&globalSumOfD,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);


    if (my_rank == 0 ){


      //Calculate the square root of the mean of squared differences.

      double s=sqrt(globalSumOfD/(n*(p-1)));

         printf("Standard Deviation %f\n" , s );

  }
	/* shutdown MPI */
	MPI_Finalize();
    return 0;
}
