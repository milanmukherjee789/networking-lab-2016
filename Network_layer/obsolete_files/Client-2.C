#include<stdio.h>
#include<dos.h>
#include<stdlib.h>
#include<string.h>
unsigned char PKT_INT=0x60;
unsigned char CLASS,TYPE,NUMBER;
unsigned char flag[2];
unsigned handle;
unsigned bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags;
int MAX_BUFF_LEN=600;
unsigned char BUFF[600];
unsigned int length=0;
unsigned char PACKET[100];
unsigned char *ACK="Messege succsfully received";
unsigned char MY_DEST[6]={0x08,0x00,0x27,0x93,0xd6,0x34};
//unsigned char MY_DEST[6]={0x08,0x00,0x27,0x2e,0x11,0x75};
unsigned char MY_ADDR[6];//={0x08,0x00,0x27,0x00,0x2b,0xb3};
unsigned int PACKET_RECEIVED=0;
unsigned int PACKET_DROPPED=0;
void send_packet(unsigned char *buf,unsigned length);
void free_buffer(unsigned int l){
	int i;
	for(i=0;i<600;i++){
		BUFF[i]=0x00;
	}
	return;
}
int filter_packet(){
	int ok=0;
	int i;
	
	for(i=0;i<6;i++){
		if(BUFF[i]!=MY_ADDR[i]){
			ok=1;
			break;
		}
	} 
	if(!ok&&BUFF[12]!=0xab||BUFF[13]!=0xcd){
		ok=1;
	}
	/*
	if((BUFF[12]!=0xab)||(BUFF[13]!=0xcd)){
		ok=1;
	} */
	if(ok==0){
		PACKET_RECEIVED++;
	}
	else if(ok==1){
		PACKET_DROPPED++;
	}
	return ok;
}
void display_packet(unsigned int l){
	int i;
	if(filter_packet()==0){
		printf("%d\n",l);
		for(i=0;i<12;i++){
			printf("%02x:",BUFF[i]&0xff);
			if(i==5){
				printf("\n");
			}
		}
		printf("\n MESSAGE : \n");
		printf("%s\n",&BUFF[16]);
		fflush(stdout);
	}
	free_buffer(l);
	return;
}
void interrupt receiver(bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags){
	int i;
	if(ax==0){
		es=FP_SEG(BUFF);
		di=FP_OFF(BUFF);
		length=cx;
	}
	if(ax==1){
		display_packet(length);
		length=0;
	}
	return;
}
void get_driver_info(){
	union REGS a,b;
	struct SREGS s;
	char far *p;
	a.h.ah=1;
	a.h.al=255;
	int86x(PKT_INT,&a,&b,&s);
	if(b.x.cflag){
		exit(1);
	}
	CLASS=b.h.ch;
	TYPE=b.x.dx;
	NUMBER=b.h.cl;
	printf("ver : %x\n",b.x.bx);
	printf("cls : %x\n",b.h.ch);
	printf("no : %x\n",b.h.cl);
	printf("type : %x\n",b.x.dx);
	p=MK_FP(s.ds,b.x.si);
	printf("name : %s\n",p);
	return;
}
void get_mac(unsigned char *add){
	union REGS a,b;
	struct SREGS s;
	int i;
	a.h.ah=6;
	a.x.cx=6;
	s.es=FP_SEG(add);
	a.x.di=FP_OFF(add);
	int86x(PKT_INT,&a,&b,&s);
	printf("MY MAC ADDRESS : %d\n",b.x.cx);
	for(i=0;i<6;i++){
		printf("%02x:",*(add+i));
		MY_ADDR[i]=*(add+i);
	}
	printf("\n");
	return;

}
void access_type(){
	union REGS a,b;
	struct SREGS s;
	printf("%x %x %x\n",CLASS,TYPE,NUMBER);
	a.h.al=CLASS;
	a.x.bx=TYPE;
	a.h.dl=NUMBER;
	a.x.cx=0;
	a.h.ah=2;
	s.es=FP_SEG(receiver);
	a.x.di=FP_OFF(receiver);
	flag[0]=0x01;
	flag[1]=0x01;
	s.ds=FP_SEG(flag);
	a.x.si=FP_SEG(flag);
	int86x(PKT_INT,&a,&b,&s);
	handle=b.x.ax;
	printf("CARRY FLAG : %x\n",b.x.cflag);
	printf("HANDLE IN ACCESS TYPE: %x\n",handle);
	return;
}
void get_receive_mode(){
	union REGS a,b;
	struct SREGS s;
	a.h.ah=21;
	a.x.bx=handle;
	int86x(PKT_INT,&a,&b,&s);

	if(b.x.cflag){
		printf("ERROR IN RECEIVE_MODE %x\n",b.h.dh);
		exit(1);
	}
	printf("RECEIVE _MODE : %x\n",b.x.ax);
	return;
}
void set_receive_mode(){
	union REGS a,b;
	struct SREGS s;
	a.h.ah=20;
	a.x.bx=handle;
	a.x.cx=6;
	int86x(PKT_INT,&a,&b,&s);
	if(b.x.cflag){
		printf("ERROR IN SET RECEIVE MODE \n");
		exit(1);
	}
	return;
}
void packet_stat(){
	printf("RECEIVED PACKET PACKET = %d\n",PACKET_RECEIVED);
	printf("PACKET REJECTED = %d\n",PACKET_DROPPED);
	return;
}
void release_type(){
	union REGS a,b;
	struct SREGS s;
	a.h.ah=3;
	a.x.bx=handle;
	int86x(PKT_INT,&a,&b,&s);
	if(b.x.cflag){
		printf("ERROR IN RELEASE TYPE : \n");
		exit(1);
	}
	packet_stat();
	printf("DONE RELEASED HANDLE= =%x\n",handle);
	exit(1);
}
void create_packet(){
	int i;
	for(i=0;i<100;i++){
		PACKET[i]=0x00;
	}
	/*
	PACKET[6]=0x08;
	PACKET[7]=0x00;
	PACKET[8]=0x27;
	PACKET[9]=0x00;
	PACKET[10]=0x2b;
	PACKET[11]=0xb3;*/
	for(i=0;i<6;i++){
		PACKET[i]=MY_DEST[i];
		PACKET[i+6]=MY_ADDR[i];
	}
	/*
	PACKET[0]=0x08;
	PACKET[1]=0x00;
	PACKET[2]=0x27;
	PACKET[3]=0x45;
	PACKET[4]=0x08;
	PACKET[5]=0xbc;*/

	PACKET[12]=0xab;
	PACKET[13]=0xcd;
	PACKET[14]=0x01;  //net ID
	PACKET[15]=0x01;   //host ID
	printf("PACKET HAS BEEN CREATED\n");
	return;
}
void send_packet(unsigned char *buffer,unsigned length){
	union REGS a,b;
	struct SREGS s;
	a.h.ah=4;
	s.ds=FP_SEG(buffer);
	a.x.si=FP_OFF(buffer);
	a.x.cx=length;
	int86x(PKT_INT,&a,&b,&s);
	if(b.x.cflag){
		printf("CAN NOT SEND PACKET\n");
		exit(1);
	}
	printf("SUCCESSFULLY SEND PACKET\n");
	return;
}
void flush(char word[]){
	int i;
	for(i=16;i<100;i++){
		PACKET[i]=0x00;
	}
	for(i=0;i<256;i++){
		word[i]='\0';
	}
	return;
}
void main(){
	unsigned char *my_add=(unsigned char *)malloc(6);
	char word[256];
	char ter[4]="BYE";
	char c;
	int length,k,j;
	printf("\n %s\n","-------- NETWORK ID : 2 -------");
	get_driver_info();
	get_mac(my_add);
	access_type();
	printf("HANDLE IN MAIN : %x\n",handle);
	set_receive_mode();
	get_receive_mode();
	//set_receive_mode();
	create_packet();
/*  send_packet(&PACKET,100);*/
	while(1){
		for(j=0;j<20;j++){
			c=getch();
			if((int)c==13){
				word[j]='\0';
				break;
			}
			word[j]=c;
			putch(word[j]);
		}
		printf("\n THE MESSAGE : %s\n",word);
		if(strncmp(word,ter,3)==0){
			break;
		}
		length=strlen(word);
		k=16;
		for(j=0;j<length;j++){
			PACKET[k++]=word[j];
		}
		PACKET[k]='\0';
		send_packet(&PACKET,100);
		flush(word);
	}
	release_type();
	getch();
}
