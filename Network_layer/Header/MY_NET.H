#ifndef my_net_h_
#define my_net_h_
unsigned char MY_ADDR1[6];
unsigned char MY_ADDR2[6];
unsigned int PACKET_RECEIVED=0;
unsigned int PACKET_DROPPED=0;
unsigned char PKT_INT=0x60;
unsigned char PKT_INT2=0x70;
unsigned char CLASS[2];
unsigned char TYPE[2];
unsigned char NUMBER[2];
unsigned char flag[2];
unsigned handle[2];
unsigned int RCV1=0;
unsigned int RCV2=0;
int MAX_BUFF_LEN=600;
unsigned char BUFF[600];
unsigned char BUFF2[600];
unsigned int length=0;
unsigned char PACKET[100];
unsigned bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags;
void free_buffer(unsigned int l,unsigned char Interrupt){
	int i;
	for(i=0;i<600;i++){
		if (Interrupt==PKT_INT)
		{
			BUFF[i]=0x00;
		}
		else{
			BUFF2[i]=0x00;
		}
	}
	return;
}
int filter_packet(unsigned char *buf){
	int ok=0;
	int i;
	for(i=0;i<6;i++){
		if(buf[i]!=MY_ADDR1[i]&&buf[i]!=MY_ADDR2[i]){
			ok=1;
			break;
		}
	}
	if(!ok&&(buf[12]!=0xab)||(buf[13]!=0xcd)){
		ok=1;
	}
	if(ok==0){
		PACKET_RECEIVED++;
	}
	else if(ok==1){
		PACKET_DROPPED++;
	}
	return ok;
}
void display_packet(unsigned int l,unsigned char *buf,unsigned char Interrupt){
	int i;
	if(filter_packet(buf)==0){
		printf("%s\n",&buf[18]);
		if(routing(buf)){
			printf("%s\n","Routing has been done" );
			printf("\n\n\n %s \n\n\n","----------------------------------");
		}
		else{
			printf("%s\n","routing not needed" );
		}
	}
	free_buffer(l,Interrupt);
	fflush(stdout);
	return;
}
void interrupt receiver(bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags){
	int i;
	//printf("\n receiver-1 value of ax = %02x : \n",ax );
	if(ax==0){
		RCV1++;
		es=FP_SEG(BUFF);
		di=FP_OFF(BUFF);
		length=cx;
	}
	if(ax==1){
		display_packet(length,BUFF,PKT_INT);
		length=0;
	}
	return;
}
void interrupt receiver2(bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags){
	int i;
	if(ax==0){
		RCV2++;
		es=FP_SEG(BUFF2);
		di=FP_OFF(BUFF2);
		length=cx;
	}
	if(ax==1){
		display_packet(length,BUFF2,PKT_INT2);
	}
	return;
}

void get_driver_info(unsigned char INT){
	union REGS a,b;
	struct SREGS s;
	char far *p;
	a.h.ah=1;
	a.h.al=255;
	int86x(INT,&a,&b,&s);
	if(b.x.cflag){
		exit(1);
	}
	if(INT==PKT_INT){
		CLASS[0]=b.h.ch;
		TYPE[0]=b.x.dx;
		NUMBER[0]=b.h.cl;
	}
	else{
		CLASS[1]=b.h.ch;
		TYPE[1]=b.x.dx;
		NUMBER[1]=b.h.cl;
	}
	printf("ver : %x\n",b.x.bx);
	printf("cls : %x\n",b.h.ch);
	printf("no : %x\n",b.h.cl);
	printf("type : %x\n",b.x.dx);
	p=MK_FP(s.ds,b.x.si);
	printf("name : %s\n",p);
	return;
}
void get_mac(unsigned char *add,unsigned char INT){
	union REGS a,b;
	struct SREGS s;
	int i;
	a.h.ah=6;
	a.x.cx=6;
	s.es=FP_SEG(add);
	a.x.di=FP_OFF(add);
	int86x(INT,&a,&b,&s);
	printf("MY MAC ADDRESS : %d\n",b.x.cx);
	for(i=0;i<6;i++){
		printf("%02x:",*(add+i));
		if(INT==PKT_INT){
			MY_ADDR1[i]=*(add+i);
		}
		else if (INT==PKT_INT2)
		{
			MY_ADDR2[i]=*(add+i);
		}
	}
	printf("\n");
	return;

}
void get_receive_mode(unsigned char INT){
	union REGS a,b;
	struct SREGS s;
	a.h.ah=21;
	a.x.bx=(INT==PKT_INT)?handle[0]:handle[1];
	int86x(INT,&a,&b,&s);

	if(b.x.cflag){
		printf("ERROR IN RECEIVE_MODE %x\n",b.h.dh);
		exit(1);
	}
	printf("RECEIVE _MODE : %x\n",b.x.ax);
	return;
}
void set_receive_mode(unsigned char INT){
	union REGS a,b;
	struct SREGS s;
	a.h.ah=20;
	a.x.bx=(INT==PKT_INT)?handle[0]:handle[1];
	a.x.cx=6;
	int86x(INT,&a,&b,&s);
	if(b.x.cflag){
		printf("ERROR IN SET RECEIVE MODE \n");
		exit(1);
	}
	return;
}
void packet_stat(){
	printf("RECEIVED PACKET PACKET = %d\n",PACKET_RECEIVED);
	printf("PACKET REJECTED = %d\n",PACKET_DROPPED);
	printf("receiver1 received : %d\n",RCV1);
	printf("receiver2 received : %d\n",RCV2);
	return;
}
void release_type(unsigned char INT){
	union REGS a,b;
	struct SREGS s;
	a.h.ah=3;
	a.x.bx=(INT==PKT_INT)?handle[0]:handle[1];
	int86x(INT,&a,&b,&s);
	if(b.x.cflag){
		printf("ERROR IN RELEASE TYPE : \n");
		exit(1);
	}
	packet_stat();
	printf("DONE RELEASED HANDLE= =%x\n",b.x.bx);
	//exit(1);
}
void send_packet(unsigned char *buffer,unsigned length,unsigned char INT){
	int i;
	union REGS a,b;
	struct SREGS s;
	a.h.ah=4;
	s.ds=FP_SEG(buffer);
	a.x.si=FP_OFF(buffer);
	a.x.cx=length;
	int86x(INT,&a,&b,&s);
	if(b.x.cflag){
		printf("CAN NOT SEND PACKET\n");
		exit(1);
	}
	printf("\n SUCCESSFULLY SEND PACKET\n");
	for(i=0;i<16;i++){
		printf("%02x ",buffer[i]);
	}
	printf("\n");
	return;
}
void access_type(unsigned char INT){
	union REGS a,b;
	struct SREGS s;
   // printf("%x %x %x\n",CLASS,TYPE,NUMBER);
	if (INT==PKT_INT)
	{
		a.h.al=CLASS[0];
		a.x.bx=TYPE[0];
		a.h.dl=NUMBER[0];
	}
	else{
		a.h.al=CLASS[1];
		a.x.bx=TYPE[1];
		a.h.dl=NUMBER[1];
	}
	a.x.cx=0;
	a.h.ah=2;
	if(INT==PKT_INT){
		s.es=FP_SEG(receiver);
		a.x.di=FP_OFF(receiver);
	}
	else if(INT==PKT_INT2){
		s.es=FP_SEG(receiver2);
		a.x.di=FP_OFF(receiver2);
	}
	flag[0]=0x01;
	flag[1]=0x01;
	s.ds=FP_SEG(flag);
	a.x.si=FP_SEG(flag);
	int86x(INT,&a,&b,&s);
	if (INT==PKT_INT)
	{
		handle[0]=b.x.ax;
	}
	else{
		handle[1]=b.x.ax;
	}
	printf("CARRY FLAG : %x\n",b.x.cflag);
	//printf("HANDLE IN ACCESS TYPE: %x\n",handle);
	return;
}

void flush(char word[]){
	int i;
	for(i=14;i<100;i++){
		PACKET[i]=0x00;
	}
	for(i=0;i<256;i++){
		word[i]='\0';
	}
	return;
}
#endif