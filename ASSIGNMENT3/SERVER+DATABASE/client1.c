
/*
		**  Client will do the following tasks : 
				*** Create socket()
				*** Assign the server details with which it wants to connect 
				*** Connect() with the server
				*** Start reading from the socket and write into that socekt 
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
void error(char *msg);
void validation(int argc);
void *handler_read(void *);
void *handler_write(void *);
void filter_msg(char buf[]);
char prev_msg[255];
#define h_addr h_addr_list[0]
int main(int argc, char *argv[])
{
	int client_socket,port,n;
	struct sockaddr_in server_addr;
	struct hostent *server;
	char data[256];
	validation(argc);
	port = atoi(argv[2]);
	client_socket = socket(AF_INET,SOCK_STREAM,0);
	if(client_socket < 0){
		error("socket can not be created");
	}
	/*
		** The gethostbyname() function returns a structure of type hostent for
       	   the given host name.  Here name is either a hostname or an IPv4
           address in standard dot notation.

        *** 1st argument : char *name -> it will represent the name of the server.
        	If this argument will be provided in IPv4 format (A.B.C.D) then no DNS look up will be used.

	*/
	server = gethostbyname(argv[1]);
	if(server == NULL){
		error(" No host has been found");
	}
	printf("%s\n"," Server has been found Succesfully" );
	bzero((char *)&server_addr,sizeof(server_addr));
	bzero(prev_msg,255);
	server_addr.sin_family = AF_INET;
	/*
		**The inet_addr() function interprets character strings representing host addresses expressed in standard dotted-decimal 
		  notation and returns host addresses suitable for use as an Internet address.
	*/
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);  // To convert internet address to network address
	server_addr.sin_port = htons(port);
	if(connect(client_socket,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
		printf("%s\n",inet_ntoa(server_addr.sin_addr));
		error("Can not connect with the server");
	}
	pthread_t read_thread,write_thread;
	int *thread_sock = (int *)malloc(1);
	*thread_sock = client_socket;
	/*
		** Created two threads one for for reading from the socket and one for writing into 
		   Socket
	*/
	if(pthread_create((pthread_t *)&read_thread,NULL,handler_read,(void *)thread_sock) < 0){
			printf("%s\n","Can not create read thread , Drop connection" );
	}
	if(pthread_create((pthread_t *)&write_thread,NULL,handler_write,(void *)thread_sock) < 0){
			printf("%s\n","Can not create write thread , Drop connection" );
	}
	else{
		printf("%s\n","Both read and write thread has been created" );
	}
	/*
		** To keep alive MAIN  thread as I have created the two child threads from main. So to keep alive them
			I need to block the main thread. So only possible solution to terminate the client is ctrl + C.
			But that should be modified. 

		** Link (http://stackoverflow.com/questions/4666628/do-child-threads-exit-when-the-parent-thread-terminates)
	*/
	while(1);                     
	return 0;
}
void validation(int argc){
	if(argc < 3){
		error("Less argument");
	}
}
void error(char *msg){
	perror(msg);
	exit(1);
}
void *handler_read(void *sock){
	int client_sock = *(int*)sock;
	int n;
	char data[256];
	while(1){
		if((n = read(client_sock,data,255)) < 0){
			error("Error reading client socket");
		}
		else{
			if(data[0] == '\0'){
				break;
			}
			//filter message before reading to stop duplicated reading. 
			//filter_msg(data);
			printf("Received Message => %s\n",data);
			bzero(data,256);
		}
	}
}
void *handler_write(void *sock){
	int client_sock = *(int*)sock;
	char data[256];
	int n;
	printf("%s\n","Enter Your User name" );
	bzero(data,sizeof(data));
	while(fgets(data,255,stdin) !=NULL){
		if((n=write(client_sock,data,strlen(data))) < 0){
			error("Can not write into socket");
		}
		else{
			printf("Succesfully send data %s",data );
			bzero(data,256);
		}
	}
}
void filter_msg(char buf[]){
	int i;
	if(prev_msg[0] == '\0'){
		printf("RECEIVED MESSEGE => %s\n",buf );
		strcpy(prev_msg,buf);
		return;
	}
	if(strcmp(buf,prev_msg)){
		strcpy(prev_msg,buf);
		printf("RECEIVED MESSEGE => %s\n",prev_msg );
		return;
	}
	return;
}