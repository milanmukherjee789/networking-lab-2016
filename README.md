In this assignment an application has been developed by which users of two computers (Operating System : FreeDOS/ MS-DOS),which are directly connected via LAN, can chat with each other.  

#Components: 
	
		1. Host Operating System -> Ubuntu-16.04 LTS (Linux)

        2. The  Virtualization platform -> Virtual Box (Version 5.1.12 r112440 (Qt5.5.1))

        3. The Guest Operating System -> FreeDOS (version:1.2)

        4. Device Driver (Packet Driver) -> PCNTPK.COM (network Adapter PCnet-FAST ||| (AM79c973))

#Setup Network section of Virtual Box :

		I have changed the value of "Attached to" options in "Network Settings" in Virtual Box to "Bridge Adapter" and have set the "promiscuous mode" to "allow all" in advanced settings.   

#Installation of Packet Driver :  
			
				After installing FreeDos (Mode : FreeDOS without drivers (Emergency Mode)) packet Driver for the network adapter of the VirtualBox (PCnet-FAST ||| (AM79c973)) has been downloaded . In this case the driver is "PCNTPK.COM".
				After starting up the OS this driver has been installed by using the following command :

											PCNTPK INT=0X60
				But to install the driver at start-up the following command has been added into the "autoexec.bat" file of FreeDOS.

											LH PCNTPK INT=0X60 (PCNTPK.COM should be copied into C:\ drive to execute this command)

				After Executing the above command the output was like following :

											Packet driver is at segment 167A
											Interrupt number 0xA (10)
											I/O port 0xD020 (53280)
											My Ethernet address is 08:00:27:5E:CE:43

				After installing the driver to check if it has been successfully installed or not some applications like DHCP,FTP have been installed in the OS and those applications have been successfully run which indicates "Successful installation of Packet Driver".

#Compiler:

				Turbo C compiler has been used in this assigment.

#Development of the Application :
	
				In this application some APIs have been used and those APIs have been called in a particular sequence in the main function. The functions,which have been used in the code, are listed below;

									1.	get_driver_info()
									2.	get_mac(unsigned char *my_add)
									3.	access_type()
									4.	get_receive_mode()
									5.	set_receive_mode()
									6.	create_packet()
									7.	send_packet(unsigned char *PACKET,unsigned length)
									8.	release_type()

				Here EtherPacket has been defined as below :

									PACKET[0] ----> PACKET[5] ==> Destination Mac address
									PACKET[6] -----> PACKET[11] ===> Source Mac Address
									PACKET[12] -----> PACKET[13] ===> 2 Bytes Packet Type Information
									PACKET[14] -----> PACKET[MAX-1] ======> Information or Chat content


				By this Application only two computers can communicate in a half Duplex type so a filter function has been added into the code just before displaying the chat content . In this filter function only first 6 bytes will be checked and if any of the 6 bytes is equals to 0xff then the application will reject the packet as that is a broadcast packet.  

				Function Description:

								1.	get_driver_info():	<CLASS,TYPE,NUMBER> are three global variables in the 												     code and those variables will be used in the access_t												  ype() function and have been assigned the value by 												       this function.

								2.	get_mac(unsigned char *add):	This function will store the mac address of t													     he virtual device into "add" buffer.

								3.	access_type():		Which type of packet the application will receive can be set by this function and by this function the "receiver" (Interrupt handler) can be set so that when ever a packet(having specified type,determined by the value of CX) will be received the receiver function will be called with some specific arguments. After successful execution of this function an unsigned int "HANDLE" will be returned and this "HANDLE" will be used through out the application for receiving packets.

								4.	get_receive_mode():	This function will return the receive mode of the res												     pective "HANDLE",which has already been initialised by access_type()

								5.	set_receive_mode():	 This function will set the receive mode to desired 												      one. There are 6 modes are available. In this applic												   ation mode 6 has been used as all packets should be received.

								6.	create_packet():	 In this function only first 14 bytes of Ethernet 												      packet has been initialised. 

								7.	send_packet(unsigned char *packet,unsigned length):	this function will send the packet.

								8.	release_type():		 This function ends access to packets associated with a handle returned 						 by access_type(). The handle is no longer valid. 	

#Run The code : 		
			To run the code "tc" editor should be used with compilation mode -> "huge". As the source and Destination Mac address has been hard-coded inside the application so before establishing new communication the destination address should be changed and the code should be recompiled. 
