/*
 * socket_exam.c
 * derived from Advanced Programming in the UNIX Environment, 3rd Edition
 *
	 * Eric McGregor 02/27/20
 */

#include <string.h>	
#include <stdio.h>	
#include <stdlib.h> 
#include <unistd.h>	
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>  
#include <signal.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>	
#include <sys/socket.h> 
#include <pthread.h>
#include <sys/sem.h>

#define QLEN 128

#ifndef HOST_NAME_MAX 
	#define HOST_NAME_MAX 256 
#endif

#define MAX_THREADS 1000000L

extern int errno;
int descri[1096];
int size=4;
void * thread_do(void *arg);

char* path = "/tmp";
int tokid = 250;
key_t key;
int semid[500];
	//created key


void printids(const char *s);
void remove_semaphore_set(char* user, int semid); 

		int main(int argc, char *argv[]) 
{
	char *host_name = malloc(HOST_NAME_MAX);
	memset(host_name, 0, HOST_NAME_MAX);

	if (gethostname(host_name, HOST_NAME_MAX) < 0) 
	{
		perror("gethostname error");
		return(-1);
	}

	printf("host name: %s\n", host_name);

	struct addrinfo 	*host_ai;
	struct addrinfo 	hint;

	hint.ai_flags = 0;
    	hint.ai_family = 0;
    	hint.ai_socktype = SOCK_STREAM;
    	hint.ai_protocol = 0;
    	hint.ai_addrlen = 0;
    	hint.ai_canonname = NULL;
    	hint.ai_addr = NULL;
    	hint.ai_next = NULL;

	int err;
    	if ((err = getaddrinfo(host_name, "tokenserver", &hint, &host_ai)) != 0)
	{
        	printf("getaddrinfo error: %s\n", gai_strerror(err));
        	return 0;
    	}

   	int host_fd;
	if ((host_fd = socket(host_ai->ai_addr->sa_family, SOCK_STREAM, 0)) < 0) 
	{
		perror("unable to create socket"); 
	}
	printf("socket created [%d]\n", host_fd);

    	if (bind(host_fd, host_ai->ai_addr, host_ai->ai_addrlen) < 0)
	{
   	     perror("unable to bind to socket");
    	}
	printf("bind returned success\n");

	freeaddrinfo(host_ai);

    	if (listen(host_fd, QLEN) < 0)
	{
        	perror("listen failed");
    	}
	printf("listen returned success\n");

	struct sockaddr client_sockaddr;
	socklen_t client_sockaddr_len = sizeof(client_sockaddr);
	
	for (;;) 
	{

		printf("waiting for connection ...\n");
    	int clfd = accept(host_fd, &client_sockaddr, &client_sockaddr_len);
       	if (clfd < 0||clfd >1096) 
	{
			perror("accept error");
		}
		printf("accepted connection, socket [%d]\n", clfd);

		pthread_t tid;	
		if (pthread_create(&tid, NULL, thread_do, &clfd) != 0)
		{
			perror("Error creating thread");
		}
				
		printf("running thread %d\n", tid);	
		if (pthread_detach(tid) != 0) 
		{
			perror("detach error");
		}


			
		
	} 
}



void * thread_do(void *fd) 
{	
	int clfd;	
//	int early=0;
//	if (early==1)
//	{
//		clfd=clfd+1;
		 clfd = *(int*) fd;
//	}
//	else
//	{
	
//		 clfd = *(int*) fd;
//	}
	descri[size]=clfd;
	
	if (clfd%2==0)
	{
		tokid = tokid +1;
		if ((key = ftok(path, tokid)) == -1)
		{
			perror("Can not create token");
			return NULL;
		}
			printf(" key : %d\n ", key);	

		if ((semid[clfd] = semget(key, 9, 0666 | IPC_CREAT)) == -1) {
			perror("Can not create semaphore");
			return NULL;
		}


		int i ;
		for (i=0 ; i<9; i++)
		{

			if ((semctl(semid[clfd], i, SETVAL, 2)) == -1)
			{
				perror("Error setting semaphore to 0");
				return NULL;
			
			}
		}
	}
		
		
		while(1)
		{
		
	   		 printf("Clfd:%d  ", clfd);
			//receving the place holder
			int set;
			int z;
			memset(&set,'\0',4);
			if (recv(clfd,&set,4,0)<=0)
			{
				int sub[10];
				sub[0]=9;
				if(clfd%2==0)
				{
					send(clfd+1,sub,40,0);
				}
				else
				{
					send(clfd-1,sub,40,0);
				}
				printf("Cannot receive the place holder\n");
				pthread_exit(0);
			}
			printf("set: %d\n", set);	
			
			
			//odd desc use 0 and even use 1
			if (clfd%2==0){

				semctl(semid[clfd],set,SETVAL,1);
			}
			else
			{
				semctl(semid[clfd-1],set,SETVAL,0);
			}

			
			int j;

			int semValue;
			int a[10];
			memset(a,'\0',40);
			//printing the tic tac toe board
			for (j=0;j<9;j++)
			{

				if (clfd%2==0)
				{	
					semValue = semctl(semid[clfd],j,GETVAL);
				}
			else
			{
				semValue = semctl(semid[clfd-1],j,GETVAL);
			}
				a[j]= semValue;
				if(a[j]==2)
				{
					printf("- ");
				}
				 else if(a[j]==1)
				{
					printf("X ");
				}

					else
					 {
						printf("O ");
					 }
					if (j==2 || j == 5 || j ==8)
					{
						printf("\n");
					}
				}
				
				if (clfd%2==0){
				if (z==1)
				{
					a[0]=9;
				}
					if (send(clfd,a,40,0)<=0)
					{
						printf("Sending array error 1 \n");
					}
					if (send(clfd+1,a,40,0)<=0)
					{
						printf("Sending array error 2 \n");
					}
				
				}
				else
				{
					 if (send(clfd,a,40,0)<=0)
					{   
						printf("Sending array error 1 \n");
					}   
					if (send(clfd-1,a,40,0)<=0)
					{   
						printf("Sending array error 2 \n");
					}   
				
				}

				int k=2;
				//checking if somebody won or not
				if ((a[0]==a[1]) && (a[1]==a[2]) && (a[0]==1||a[0]==0)) 
				{
					
					if (a[0]==0)
					{
						k =0;
					}
					else
					{
						k=1;
					}

				}
				else if((a[0]==a[3])&&(a[3]==a[6]) && (a[0]==1||a[0]==0))
				{
				 if (a[0]==0)
					{
						k =0;
					}
					else
					{
						k=1;
					}
				}
				
				else if((a[0]==a[4])&&(a[0]==a[8]) && (a[0]==1||a[0]==0))
				{
				 if (a[0]==0)
					{
						k =0;
					}
					else
					{
						k=1;
					}
				}
				else if((a[6]==a[4])&&(a[4]==a[2]) && (a[6]==1||a[6]==0))
				{
				 if (a[6]==0)
					{
						k =0;
					}
					else
					{
						k=1;
					}
				}

				else if((a[6]==a[7])&&(a[7]==a[8]) && (a[6]==1||a[6]==0))
				{
				 if (a[6]==0)
					{
						k =0;
					}
					else
					{
						k=1;
						}
				}
				else if(a[5]==a[2]&& a[2]==a[8] && (a[8]==1||a[8]==0))
				{
				 if (a[5]==0)
					{
						k =0;
					}
					else
					{
						k=1;
					}
				}
				else if((a[1]==a[4])&&(a[1]==a[7]) && (a[1]==1||a[1]==0))
				{
				 if (a[1]==0)
					{
						k =0;
					}
					else
					{
						k=1;
					}
				}
				else if((a[3]==a[4])&&(a[4]==a[5]) && (a[3]==1||a[4]==0))
				{
				 if (a[3]==0)
					{
						k =0;
					}
					else
					{
						k=1;
					}
				}


					printf("----------------\n");


					char won[5]="won!\0";
					char lost[5]= "Lost\0";
					if (k==1)
					{
					
						if (clfd%2==0)
						{
							printf("We got a winner\n");
							send(clfd,&won,5,0 );
							send(clfd+1,&lost,5,0);
						}
						else
						{
								printf("We got a winner\n");
							send(clfd-1,&won,5,0);
							send(clfd,&lost,5,0);
						}
					}
					else if (k==0)
					 {
						if (clfd%2==1)
						{

							printf("We got a winner\n");
							send(clfd,&won,5,0 );
							send(clfd-1,&lost,5,0);
						}
						else
						{
							printf("We got a winner\n");
							send(clfd,&won,5,0);
							send(clfd,&lost,5,0);
						}
					}
					else{

						char nost[5]="Cont\0";
						
						if (clfd%2==0)
						{
							printf("Matching is still going\n");
							send(clfd,&nost,5,0);
							send(clfd+1,&nost,5,0);
						}
						else
						{
						
							printf("Matching is still going\n");
							send(clfd,&nost,5,0);
							send(clfd-1,&nost,5,0);
						}
					}
					size++;

				}
				
				close(clfd);
				
				}

			// sdfsdEND OF FILE
