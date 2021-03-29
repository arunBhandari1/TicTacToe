#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

extern int errno;
static int sockfd;
void * msg_print(void *fd);
void * msg_type(void *fd); 
void print_error(char *);
pthread_mutex_t lock;

pthread_mutex_t lock1;
pthread_mutex_t lock2;
pthread_mutex_t lock3;
int main(int argc, char *argv[]) {
	struct addrinfo *host_ai;
	struct addrinfo hint;
	int err;
    
	if (argc != 2) {
        	print_error("usage: client hostname");
    }

	hint.ai_flags = 0;
    	hint.ai_family = AF_INET;
    	hint.ai_socktype = SOCK_STREAM;
    	hint.ai_protocol = 0;
    	hint.ai_addrlen = 0;
    	hint.ai_canonname = NULL;
    	hint.ai_addr = NULL;
    	hint.ai_next = NULL;

	if ((err = getaddrinfo(argv[1], "tokenserver", &hint, &host_ai)) != 0) {
		printf("getaddrinfo error: %s\n", gai_strerror(err));
		return 0;
	}

	printf("creating socket\n");
	if ((sockfd = socket(host_ai->ai_addr->sa_family, SOCK_STREAM, 0)) < 0) {
		print_error("Error creating socket");
	}
	printf("socket created\n");

	printf("attempting Connection\n");
	if (connect(sockfd, host_ai->ai_addr, host_ai->ai_addrlen) != 0) {
		close(sockfd);
		printf("can't connect to %s\n", argv[1]);
		print_error("Error connecting to server");
	}
	printf("connection made...\n"); 
	freeaddrinfo(host_ai);
	
	pthread_t tid;
	if(pthread_create(&tid,NULL,msg_print,&sockfd)!=0)
	{
		perror("Error creating thread");
	}

	printf("running thread %d\n", tid);
       if (pthread_detach(tid) != 0)
	 {
        	  perror("detach error");
         }
 	while(1)
        {
	
               	int p;
		memset(&p,'\0',4);		
		printf("Enter place 0-9 \n");
		scanf(" %d", &p);
		//while(getchar() !='\n' );
		//fflush(stdin);
		
		if (send(sockfd,&p ,4,0)<0)
                {
			close(sockfd);
               		print_error("Error sending string");
			
		 }
}
}

void * msg_print(void *fd)
{
	
	int sockfd= *(int *) fd;

		
	
		
	while(1)
		{
				
		pthread_mutex_lock(&lock1);
			int i=0;
			printf("Value of i :%s\n",i);
		//	pthread_mutex_lock(&lock2);		
			int clfd[9];
			memset(&clfd,'\0',36);
			if(recv(sockfd,&clfd,36,0)<=0)
			{
				close(sockfd);
				print_error("Error receiveing");
				exit(0);	
			}
//		pthread_mutex_unlock(&lock2);

//		pthread_mutex_lock(&lock);
		int j=0;
		for (j=0;j<9;j++)
        	{

        	        if(clfd[j]==2)
                	{
                        	printf("- ");
                	}
               		 else
                	{
                        	printf("%d ", clfd[j]);
                	}
                	if (j==2 || j == 5 || j ==8)
                	{
                        	printf("\n");
                	}
       		}
//		pthread_mutex_unlock(&lock);
		
		printf("-------------------------\n");			
		printf("whats up this shit\n");	
		char result[5];
		memset(result,'\0',5);
		char won[5]="won!\0";
		char lost[5]= "Lost\0";
	//	memset(result,'\0',4);
		if (recv(sockfd,&result,4,0)<=0)
		{
			
			exit(1);
		}
		printf("%s",result);
		int res;
		if (strcmp(result,won)==0)
		{
		
			printf("You won\n");
		}
		else if (strcmp(result,lost)==0)
		{
			printf("You lost\n");
		}
		pthread_mutex_unlock(&lock1);	
		i++;
	}
	}

void print_error(char *str) {
	printf("%s: %s\n", str, strerror(errno));
	exit(1);
}


// END OF FILE	
