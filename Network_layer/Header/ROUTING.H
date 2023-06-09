#ifndef routing_h_
#define routing_h_
struct ARP{
	unsigned char iip[2];
	unsigned char mac[6];
}MY_ARP[6];
int ReadHexFile(FILE *inf, unsigned char *dest) {
  int count = 0;
  int n;
  unsigned char OneByte;
  if (dest == NULL) {
	while ((n = fscanf(inf, "%hhx", &OneByte)) == 1 ) {
	  count++;
	}
  }
  else {
	while ((n = fscanf(inf, "%hhx", dest)) == 1 ) {
	  dest++;
	}
  }
  return count;
}
void create_table(void){
	 int i,j,index,iip_index,mac_index;
	 FILE *inf;
	 int n;
	 unsigned char *hex;
	 inf = fopen("routing.txt", "rt");
	 n = ReadHexFile(inf, NULL);
	 printf("%d\n",n);
	 rewind(inf);
	 hex = malloc(n);
	 ReadHexFile(inf, hex);
	 index=0;
	 iip_index=0;
	 mac_index=0;
	 for(i=0;i<n;i++){
		if(i%8!=0 && i%8!=1){
			iip_index=0;
			MY_ARP[index].mac[mac_index]=hex[i];
			if(mac_index==5){
				index++;
			}
			mac_index++;
		}
		else{
			MY_ARP[index].iip[iip_index++]=hex[i];
			mac_index=0;
		}
	 }
	 for(i=0;i<n/8;i++){
		printf("%s\n","\n IIP : ");
		for(j=0;j<2;j++){
			printf("%02x:",MY_ARP[i].iip[j]);
		}
		printf("\n MAC : ");
		for(j=0;j<6;j++){
			printf("%02x:",MY_ARP[i].mac[j]);
		}
		printf("\n");
	 }
	 if(inf!=NULL){
		printf("%s\n","SUCCESS");
	 }
	 fclose(inf);
	 free(hex);
}
int routing(unsigned char *data){
	unsigned char source[6];
	unsigned char netID;
	int i,j,flag,ARP_index;
	ARP_index=-1;
	printf("in routing : \n");
	netID=data[16];
	printf("%s\n","ROUTER HAS RECEIVED PACKET DATA : ");
	for(i=0;i<16;i++){
		printf("%02x ",data[i]);
	}
	printf("\n");
	if(netID==data[14]){
		printf("%s\n","Same network" );
		return 0;
	}
	else{
		printf("%s\n","different network" );
		fflush(stdout);
		fflush(stdin);
		for(i=0;i<6;i++){
			if(MY_ARP[i].iip[0]==data[14]&&MY_ARP[i].iip[1]==data[15]){
				ARP_index=i;
				break;
			}
		}
		if(ARP_index>=0){
			if (data[14]==0x01){
				if(data[15]==0x02){
					printf("%s\n","This packet will be passed to default gateway" );
					return 1;
				}
				else{
					for(i=0;i<6;i++){
						data[i]=MY_ARP[ARP_index].mac[i];
						data[i+6]=MY_ADDR1[i];
					}
					send_packet(data,100,PKT_INT);
					printf("%s %02x\n","PACKET HAS COME FROM netID:2 AND WILL BE SENT TO netID:1 BY ",PKT_INT);
				}
			}
			else if(data[14]==0x02){
				if(data[15]==0x02){
					printf("%s\n","This packet will be passed to default gateway" );
					return 1;
				}
				else{
					for(i=0;i<6;i++){
						data[i]=MY_ARP[ARP_index].mac[i];
						data[i+6]=MY_ADDR2[i];
					}
					send_packet(data,100,PKT_INT2);
					printf("%s %02x\n","PACKET HAS COME FROM netID:1 AND WILL BE SENT TO netID:2 BY ",PKT_INT2);
				}
			}
		}
		else{
			printf("%s\n","No entry for coming netID found" );
		}
		return 1;
	}
}
#endif
