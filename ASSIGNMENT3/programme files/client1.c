#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
void error(char *msg);
void validation(int argc);
void *handler_read(void *);
void *handler_write(void *);
void filter_msg(char buf[]);
char prev_msg[255];
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
	server = gethostbyname(argv[1]);
	if(server == NULL){
		error(" No host has been found");
	}
	bzero((char *)&server_addr,sizeof(server_addr));
	bzero(prev_msg,255);
	server_addr.sin_family = AF_INET;
	strncpy((char *)server->h_addr, 
         (char *)&server_addr.sin_addr.s_addr,
         server->h_length);	
	server_addr.sin_port = htons(port);
	if(connect(client_socket,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
			error("Can not connect with the server");
	}
	/*printf("%s\n","enter messege" );
	bzero(data,sizeof(data));
	while(fgets(data,255,stdin) !=NULL){
		if((n=write(client_socket,data,strlen(data))) < 0){
			error("Can not write into socket");
		}
		else{
			printf("%s\n","Succesfully send data" );
			bzero(data,sizeof(data));
		}
	}*/
	pthread_t read_thread,write_thread;
	int *thread_sock = (int *)malloc(1);
	*thread_sock = client_socket;
	if(pthread_create((pthread_t *)&read_thread,NULL,handler_read,(void *)thread_sock) < 0){
			printf("%s\n","Can not create read thread , Drop connection" );
	}
	if(pthread_create((pthread_t *)&write_thread,NULL,handler_write,(void *)thread_sock) < 0){
			printf("%s\n","Can not create write thread , Drop connection" );
	}
	else{
		printf("%s\n","Both read and write thread has been created" );
	}
	while(1);                     // to keep alive parent thread.
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
			//printf("");              //filter messege before reading to stop spamming
			filter_msg(data);
			bzero(data,256);
		}
	}
}
void *handler_write(void *sock){
	int client_sock = *(int*)sock;
	char data[256];
	int n;
	printf("%s\n","enter messege" );
	bzero(data,sizeof(data));
	while(fgets(data,255,stdin) !=NULL){
		if((n=write(client_sock,data,strlen(data))) < 0){
			error("Can not write into socket");
		}
		else{
			printf("%s\n","Succesfully send data" );
			bzero(data,sizeof(data));
		}
	}
}
void filter_msg(char buf[]){
	int i;
	if(prev_msg[0] == '\0'){
		printf("NULL %s\n",buf );
		strcpy(prev_msg,buf);
		return;
	}
	if(strcmp(buf,prev_msg)){
		strcpy(prev_msg,buf);
		printf("DIFF %s\n",prev_msg );
		return;
	}
	//printf(" No %s\n",buf );
	return;
}