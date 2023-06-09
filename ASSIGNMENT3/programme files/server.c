#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sqlite3.h>
#include <string>
#include <sstream>
#include <limits.h>
using namespace std;
#include "mysqlite.h"
void error(string msg);
void validation(int argc);
bool contain_ter(char data[]);
void parse_messege(char data[],int client_sock);
void message(string *parse,char data[]);
string sanitization(string data);
string user_refine(string data);
void *handler(void *);
void *handler_read(void *);
void *handler_write(void *);
#define MAX_CLIENT 20
#define DATABASE_NAME "Networking_Assignmenet3.db"
#define TABLE_NAME "USERS"
unsigned long int TIME_LIMIT = ULLONG_MAX;
int main(int argc, char *argv[])
{
	int old_socket,new_socket,port,bind_flag,l,n;
	int *client_sock;
	struct sockaddr_in server_addr,client_addr;
	validation(argc);
	// Create IPv4 Internet protocol + Stream socket
	cout<<"TIME LIMIT : "<<TIME_LIMIT<<endl;
	if((old_socket = socket(AF_INET,SOCK_STREAM,0))<0){
		error("can not create 1st socket");
	}
	// Bind the socket with Port no
	bzero((char *) &server_addr,sizeof(server_addr));
	port = atoi(argv[1]);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if((bind_flag = bind(old_socket,(struct sockaddr *)&server_addr,
		sizeof(server_addr))) < 0){

		error("Binding error");
	}
	
	// Listen to the port no
	listen(old_socket,5);
	// accept the client socket 
	cout <<"Server is waiting for incoming connections .........."<<endl;
	l = sizeof(client_addr);
	//new_socket = accept(old_socket, (struct sockaddr *)&client_addr,&l);
	create_database(DATABASE_NAME);
	create_table(TABLE_NAME);
	while((new_socket = accept(old_socket, (struct sockaddr *)&client_addr,(socklen_t*)&l))){
		pthread_t con_thread;
		client_sock = new int[1];
		*client_sock = new_socket;
		if(pthread_create((pthread_t *)&con_thread,NULL,handler,(void *)client_sock) < 0){
			cout<< "Can not create thread , Drop connection"<<endl;
		}
		else{
			cout<<"Thread has been created and task has been performed"<<endl;
			/*printf("%d.%d.%d.%d\n",
  						(int)(client_addr.sin_addr.s_addr&0xFF),
  						(int)((client_addr.sin_addr.s_addr&0xFF00)>>8),
  						(int)((client_addr.sin_addr.s_addr&0xFF0000)>>16),
  						(int)((client_addr.sin_addr.s_addr&0xFF000000)>>24));*/
		}
	}
	if(new_socket < 0){
			error("Error creating new socket final socket");
	}
	return 0;
}
void error(string msg){
	perror(msg.c_str());
	exit(1);
}
void validation(int argc){
	if (argc<2){
		error("less argument");
	}
}
void *handler(void * sock){
	char data[256];
	int n;
	pthread_t read_thread,write_thread;
	int client_sock = *(int*)sock;
	int *temp_sock = new int[1];
	*temp_sock = client_sock;
	bzero(data,256);
	/*while(1){
		if((n = read(client_sock,data,255)) < 0){
			error("Error reading client socket");
		}
		else{
			if(data[0] == '\0'){
				break;
			}
			printf(" The received messege %s\n", data);
			bzero(data,256);
		}
		printf("%s\n","ack" );
	}*/	
	if(pthread_create((pthread_t *)&read_thread,NULL,handler_read,(void *)temp_sock) < 0){
		cout<<"Can not create thread for reading , Drop connection"<<endl;
	}
	if(pthread_create((pthread_t *)&write_thread,NULL,handler_write,(void *)temp_sock) < 0){
		cout<< "Can not create thread for writing , Drop connection"<<endl;
	}
	else{
		cout<<"both read and write thread has been created"<<endl;
	}
}
void *handler_read(void *sock){
	int client_sock = *(int*)sock;
	int n;
	char data[256];
	string parse[2];
	while(1){
		if((n = read(client_sock,data,255)) < 0){
			error("Error reading client socket");
		}
		else{
			if(data[0] == '\0'){
				cout<<"NULLLL READ"<<endl;             //thread termination condition
				break;
			}
			//cout<<"The received messege "<<data<<endl;
			if(!contain_ter(data)){
				char d[255];
				strncpy(d,data,strlen(data)-1);
				if(!strncmp(data,d,strlen(data)-1)){
					//cout<<"////////////////////////"<<strlen(data)<<strlen(d)<<endl;
				}
				insert(d," Intitial Data",1,client_sock);
			}
			else{
				message(parse,data);
				parse[1] = sanitization(parse[1]);
				//cout<<" AFter sanitization : /////////////////////////////"<<parse[1]<<endl;
				parse[0] = user_refine(parse[0]);
				update(parse[0],parse[1]);
			}
			bzero(data,256);
		}
	}
}
void *handler_write(void *sock){
	int client_sock = *(int*)sock;
	int n;
	int K = 0;
	record R;
	// Retrieve messege from database and populate char array and the databse should contain the socket id as primary key 
	char data[255];//="messege from server from write thread"; 	// This will be repalced by messege retrieved from database for two connected users
	while(1){
		sleep(1);
		parse_messege(data,client_sock);
		if(data[0] == '\0'){
			if(K > TIME_LIMIT){
				break;
			}
			K++;                      // thread termination condition 
		}
		else{
			if((n = write(client_sock,data,255)) < 0){
				error("Error writing client socket");
			}
			else{
				cout<<"Has been written to socket no "<<client_sock<<endl;
			}
			bzero(data,255);
		}
	}
}		
bool contain_ter(char buf[]){
	for (int i = 0; i < strlen(buf); ++i)
	{
		if(buf[i] == '@'){
			return true;
		}
	}
	return false;
}
void message(string *parse,char buf[]){
	string msg="";
	int k = 0;
	for (int i = 0; i < strlen(buf); ++i)
	{
		if(buf[i] != '@'){
			msg += buf[i];
		}
		else{
			parse[k] = msg;
			msg = "";
			k++;
		}
	}
	parse[k] = msg;
}
void parse_messege(char data[],int client_sock){
	record R;
	string msg="";
	R = retrieve(client_sock);
	msg = R.messege;
	//cout<<" parsed data : "<<msg<<endl;
	//msg = "Shankha Choudhuri";
	strcpy(data,msg.c_str());
	//cout<<"DATA in parsing : "<<data<<endl;
	return;
}
string sanitization(string data){
	//cout<<"LENGTH : ++++++++++++++++++++ "<<data.length()<<endl;
	return data.substr(0,data.length()-1);
}
string user_refine(string data){
	string msg="";
	for (int i = 0; i < data.length(); ++i)
	{
		if(data[i] != ' '){
			msg += data[i];
		}
	}
	return msg;
}