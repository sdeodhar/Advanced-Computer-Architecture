/* C Example */
#include <stdio.h>
#include <mpi.h>
#include <trialdata.h>

int main (int argc, char *argv[])
{
	int rank; /* rank is your pid, staring with 0 */
	int size; /* size, is the number of processes you */
	int number,i = 0;
	int commu_rank, commu_no_tasks;
	int no_tasks_remaining[4];
	int lowest_no_task ,highest_no_task ;
	
	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	
	double start, finish;
	start=MPI_Wtime();
	
	int my_no_task = 100 * ( 1 + rank);
	
	//MPI_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,int dest, int sendtag,
	//		void *recvbuf, int recvcount, MPI_Datatype recvtype,int source, int recvtag,MPI_Comm comm, MPI_Status *status)
	int sum = 1 ,j,count = 0;

/*	for( j = 0; j < size ; j++)
	{
		sum += no_tasks_remaining[j];
	}*/	
	
	do
	{
		for( i = 0; i < size ; i++)
		{
			if( i != rank )
			{
				MPI_Sendrecv(&rank, 1, MPI_INT , i ,1, &commu_rank, 1, MPI_INT , i ,1 ,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				MPI_Sendrecv(&my_no_task, 1, MPI_INT , i ,1, &commu_no_tasks, 1, MPI_INT , i ,1 ,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				printf("Number received = %d from processor %d to %d processors\n",commu_no_tasks,commu_rank ,rank);
				no_tasks_remaining[ commu_rank ] = commu_no_tasks; 
				no_tasks_remaining[ rank ] = my_no_task;
			}
		}

		sum = 0;

		for( j = 0; j < size ; j++)
		{
			sum += no_tasks_remaining[j];
		}

	//	printf("Processor %d has %d tasks remaining\n",rank,no_tasks_remaining[rank]);
		MPI_Barrier(MPI_COMM_WORLD);
	
		int max, min, num;
		max = min = no_tasks_remaining[0];

		for (i = 1; i < 4; i++)
		{ 
			num = no_tasks_remaining[i];
		        if (max < num)
			max = num;
			else if (min > num)
		    	min = num;
	    	}

		lowest_no_task = min;
		highest_no_task = max;
		//printf("max = %d and min = %d\n",highest_no_task,lowest_no_task);

		//i = 0;
		do
		{
			if(rank == 0) number = Task0array[i];
			else if(rank == 1) number = Task1array[i];
			else if(rank == 2) number = Task2array[i];
			else if(rank == 3) number = Task3array[i];
			count++;
			no_tasks_remaining[rank]--;

			//file write
			//printf("Number Printed = %d from processor %d of total %d processors\n",number, rank, size);
		}while( count!= lowest_no_task);
	
		printf("Processor %d has %d tasks remaining\n",rank,no_tasks_remaining[rank]);
		MPI_Barrier(MPI_COMM_WORLD);


		my_no_task = no_tasks_remaining[rank];
	
		//load_balancing

		for( i = 0; i < size ; i++)
		{
			if( i != rank )
			{
				MPI_Sendrecv(&rank, 1, MPI_INT , i ,1, &commu_rank, 1, MPI_INT , i ,1 ,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				MPI_Sendrecv(&my_no_task, 1, MPI_INT , i ,1, &commu_no_tasks, 1, MPI_INT , i ,1 ,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
				//printf("Number received = %d from processor %d to %d processors\n",commu_no_tasks,commu_rank ,rank);
				no_tasks_remaining[ commu_rank ] = commu_no_tasks; 
				no_tasks_remaining[rank] = my_no_task;
			}
		}	
	
		max = min = no_tasks_remaining[0];
		int maxi = 0,mini = 0;

		for (i = 1; i < 4; i++)
		{ 
			num = no_tasks_remaining[i];
		        if (max < num)
			{max = num; maxi = i;}
			else if (min > num)
		    	{min = num; mini = i;}
	    	}

		lowest_no_task = min;
		highest_no_task = max;
		//printf("max = %d and min = %d\n",highest_no_task,lowest_no_task);
		//printf("maxi = %d and mini = %d\n",maxi,mini);
	
		if(rank == maxi)
		{ 
			int my_value = no_tasks_remaining[rank]/2;
			no_tasks_remaining[rank]= no_tasks_remaining[rank]/2 ;
			MPI_Send(&my_value, 1, MPI_INT, mini , 1, MPI_COMM_WORLD);
		}
		else if(rank == mini)
		{
			MPI_Recv(&my_no_task,1, MPI_INT, maxi , 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			no_tasks_remaining[rank] = my_no_task;
		}
	
		my_no_task = no_tasks_remaining[rank];
	}while( sum != 0 );

	//load_balancing

	printf("Processor %d has %d tasks remaining\n",rank,no_tasks_remaining[rank]);
	MPI_Barrier(MPI_COMM_WORLD);
	
	finish=MPI_Wtime();
	double elapsed_time=(finish-start);
	printf("Total Elapsed time=%f\n",elapsed_time);

	MPI_Finalize();
	return 0;
}

/*
MPI_Send(&my_value, 1, MPI_DOUBLE, ((my_size - my_rank) -1) , 1, MPI_COMM_WORLD);
MPI_Recv(&rcv_value,1, MPI_DOUBLE, MPI_ANY_SOURCE , 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
*/
