// #include "socketutils/socketutil.h"
#include "../socketutils/socketutil.h"

void startListningAndPrintMessagesOnNewThread(int socketFD);
void listenAndPrint(int socketFD);

int main(){
	printf("Hello world\n");
	int socketFD = createTCPIpv4Socket();

	struct sockaddr_in *address = createIPv4Address("127.0.0.1", 2000);

	int result = connect(socketFD, address, sizeof(*address));

	if(result == 0){
		printf("Connection is successufl.\n");
	}

	char *name = NULL;
	size_t nameSize = 0;
	printf("please enter your name\n");
	size_t nameCount = getline(&name, &nameSize, stdin);
	name[nameCount-1] = 0; //this removes the \n at the end of the name string. 


	char *line = NULL;
	size_t lineSize = 0;
	printf("Type and we will send(\"exit\" to exit)");

	startListningAndPrintMessagesOnNewThread(socketFD);

	char buffer[1024];

	/*this while loop to send stuff is blocking code. if we also want to recieve things, we will have to use multithreading*/
	while(true){
		size_t charCount = getline(&line, &lineSize, stdin);
		line[charCount-1] = 0;
		sprintf(buffer, "%s: %s", name,line);

		if(charCount > 0){
			if(strcmp(line, "exit") == 0){size_t amountWasSent = send(socketFD, "", 0, 0);
			break;}
			size_t amountWasSent = send(socketFD, buffer, strlen(buffer), 0);
		}
	}

	close(socketFD);


	// char *message;
	// message = "GET \\ HTTP/1.1\r\nHost:google.com\r\n\r\n";
	// send(socketFD, message, strlen(message), 0);

	// char buffer[1024];
	// recv(socketFD, buffer, 1024, 0);

	// printf("Response was %s\n", buffer);
	return 0;
}


void startListningAndPrintMessagesOnNewThread(int socketFD){
	pthread_t id;
	pthread_create(&id, NULL, listenAndPrint, socketFD);
}

void listenAndPrint(int socketFD){
	char buffer[1024];
	while(true){
	
		ssize_t amountRecieved = recv(socketFD, buffer, 1024, 0);
		if(amountRecieved > 0){
			buffer[amountRecieved+1] = 0;
			printf("%s\n", buffer);
			
			}
		if(amountRecieved == 0) break; 
	}

	close(socketFD);

}
