Set-up: 

	For this assignment we need atleast five laptops. Among them two devices will be connected via hub-1 (they will be in same network having netID-1) and rest two will be connected via hub-2 (they will be having netID-2). hub-1 and hub-2 will be connected with the laptop with two different NICs (PCI & USB in my case) in which "Router programme" will run. In virtual box I have to select two adapters and each of these adapters have been bridged with corresponding NICs(PCI + USB). By this complete network set up we have created a private network to reduce the unintended network traffic to prevent the "Crash is programme execution".

Packet Drivers installation:
	
	I have to install two packet drivers for two different NICs in my virtual box. For doing this I have used the patched version of "PCNTPK.COM" (avilable at this link : "http://unix.oppserver.net/vmware/unix/fixpcnt.com"). The respective Commands to install packet drivers for two virtual adapters are as below:

				1. fixpcnt.com  (it will create pcntpk2.com ==>patched version of pcntpk.com)
				2. pcntpk2.com int=0x60 ioaddr=0 (it will install packet driver for adapter[0]==>1st ADP)
				3. pcntpk2.com int=0x70 ioaddr=1 (it will install packet driver for adapter[1]==>2nd ADP)

	*** For doing (2 & 3) at startup I have inserted these two commands in autoexec.bat in DOS.

Packet Structure:
	
			1. packet[0] -> packet[5] ==> destination mac address
			2. packet[6] -> packet[11] ==> source mac address
			3. packet[12] -> packet[13] ==> packet Type (in my case 0xABCD)
			4. packet[14] -> packet[15] ==> destination iip
			5. packet[16] -> packet[17] ==> source iip
			6. packet[18] -> packet[18+size_of_data] ==> data
			7. packet[size_of_data+19] -> packet[MAX-1] ==> padded by NULL (0x00)

Experiment:
		
	In router programme I have maintained a ARP table,which has two fields 1. iip[2] , 2. mac[6]. The entries have been read from a file ("routing.txt" ==> < <netID hostID> <mac> >).This ARP table will be populated whenever the programme will be executed. I have also maintained same ARP table in each client so that iip to mac mapping can be done also in client side so that when clients want to communicate within the network they will send those packets directly to the destination device not in router.But when client with netID-1 wants to communicate client with netID-2 then (by that ARP table) the client will send the packet to corresponding router adapters and in router it will first try to find if there is an entry for the comming destination iip, if any entry will be found then it will put the destination into the packet and send that packet through paticular adapter. If  no such entry will be found then ERROR messege will be displayed in router side.

Code:
		
	The main file for the "Router" is "Router.c". In this file I have included two extra headers               
	                        1."my_net.h" ===> all those packet driver APIs has been defined in this header
	                          
	                        2. "routing.h" ===> Main routing algorithm ,Creation of ARP table have been implemented in this header.

Extra software:
	
	1. For transferring file between linux host and Virtual Box (MS-DOS) I have used "mtcp->FTP".

	2. For debugging purpose like (If packets are sending or receiving correctly) I have used "wireshark".
