//NET ID 2 WILL BE CONNECTED WITH USB ETHERNET
//NET ID 1 WILL BE CONNECTED WITH PCI ETHERNET
#include<stdio.h>
#include<dos.h>
#include<stdlib.h>
#include<string.h>
#include "my_net.h"
#include "routing.h"
void main(){
	unsigned char *my_add1=(unsigned char *)malloc(6);
	unsigned char *my_add2=(unsigned char *)malloc(6);
	char word[256];
	char ter[4]="BYE";
	char c;
	int i,length,k,j;
	create_table();
	get_driver_info(PKT_INT);
	get_mac(my_add1,PKT_INT);
	access_type(PKT_INT);
	printf("HANDLE IN MAIN AFTER 1ST ACCESS_TYPE : %x\n",handle[0]);
	set_receive_mode(PKT_INT);
	get_receive_mode(PKT_INT);
	get_driver_info(PKT_INT2);
	get_mac(my_add2,PKT_INT2);
	access_type(PKT_INT2);
	printf("HANDLE IN MAIN AFTER 2nd ACCESS_TYPE : %x\n",handle[1]);
	set_receive_mode(PKT_INT2);
	get_receive_mode(PKT_INT2);
	getch();
	release_type(PKT_INT);
	release_type(PKT_INT2);
}
