#include <unistd.h>

#include <stdlib.h>

#include <stdio.h>

#include <string.h>

#include <sys/shm.h>

#include <time.h>

#include <sys/time.h>

#include "shm_str.h"
#include <wait.h>
#define MICRO_SEC_IN_SEC 1000000
int main(){

	struct timeval start,end;
	gettimeofday(&start,NULL);// start time
	
	pid_t pid;
	struct shared_use_st *matrix;
	int shmid;
	int status;
	void *shared_memory = (void *)0;
	int d=0;
	
	// cresting shared memory	shmget creates a shared memory
	shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
	if (shmid == -1) { // error handling 

		fprintf(stderr, "shmget failed\n");

		exit(EXIT_FAILURE);

	}
	
	// making shared memory accessible to the program shmat() returns a pointer to void
	shared_memory = shmat(shmid, (void *)0, 0);

	if (shared_memory == (void *)-1) {

		fprintf(stderr, "shmat failed\n");

		exit(EXIT_FAILURE);

	}
	//casting of type struct
	matrix = (struct shared_use_st *)shared_memory;
	
	//This is the test given to us to try out
	matrix->M[0][0]=20;
	matrix->M[1][0]=20;
	matrix->M[2][0]=50;
	matrix->M[0][1]=10;
	matrix->M[1][1]=6;
	matrix->M[2][1]=70;
	matrix->M[0][2]=40;
	matrix->M[1][2]=3;
	matrix->M[2][2]=2;


	




	
	int i=0;

	for(;i<3;i++){
		pid=fork();
		if(pid==0){
			break;
		}
	}

	switch(pid){
		case -1:
			printf("fork failed");
			exit(0);
		case 0:
			if(i==0){
				printf("first child i=%d working with  the  0 element  of the array\n", i);
				matrix->d[0]=(matrix->M[1][1]*matrix->M[2][2] )- (matrix->M[2][1] * matrix->M[1][2]);
				printf("%d\n",matrix->d[0]);

			}
			else if(i==1){
				printf("second child i=%d working with  the  1 element  of the array\n", i);
				matrix->d[1]=matrix->M[0][1]*matrix->M[2][2] - matrix->M[2][1] * matrix->M[0][2];
				printf("%d\n",matrix->d[1]);
			}
			else if(i==2){
				printf("third child i=%d working with  the  2 element  of the aray\n",i);
				matrix->d[2]=matrix->M[0][1]*matrix->M[1][2] - matrix->M[1][1] * matrix->M[0][2];
				printf("%d\n",matrix->d[2]);
			}
			else{
				printf("error wrong\n"); 
			}
			//child detaches from  the shared memory
			if (shmdt(shared_memory) == -1) {

				fprintf(stderr, "shmdt failed\n");

				exit(EXIT_FAILURE);

			}
			exit(0);
		default:
			printf("parent id with i=%d\n",i);
			for(int j =0; j<3; j++);{
				wait(&status);
				
			}
			
			matrix->d[3]=(matrix->M[0][0]*matrix->d[0]) - (matrix->M[1][0]*matrix->d[1]) + (matrix->M[2][0]*matrix->d[2]);
			d=matrix->d[3];
			printf("This is the determinant: %d\n",d);
			// Time taken to perform all operation
			gettimeofday(&end,NULL);
			printf("Time taken to perform all operation:%ld microseconds.\n",((end.tv_sec*MICRO_SEC_IN_SEC + end.tv_usec)-(start.tv_sec*MICRO_SEC_IN_SEC +start.tv_usec)));
			break;
			
			
			
			
			
	//parent detaches from  the shared memory and deletes it
			if (shmdt(shared_memory) == -1) {

				fprintf(stderr, "shmdt failed\n");

				exit(EXIT_FAILURE);

			}
			if (shmctl(shmid, IPC_RMID, 0) == -1) {

				fprintf(stderr, "shmctl(IPC_RMID) failed\n");

				exit(EXIT_FAILURE);

			}
			




	}
}
