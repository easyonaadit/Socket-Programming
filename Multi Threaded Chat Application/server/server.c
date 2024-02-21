#include "../socketutils/socketutil.h"





struct AcceptedSocket{
	int acceptedSocketFD;
	struct sockaddr_in address;
	int error;
	bool acceptedSuccessfully;
};

struct AcceptedSocket* acceptedSockets[10];
int acceptedSocketsCount = 0;

struct AcceptedSocket* acceptIncommingConnection(int serverSocketFD); //blocks and accepts incoming connection, and returns the properties about that connection in a custom structure. 
void recieveAndPrintIncomingData(int socketFD);
void startAcceptingIncomingConnections(int serverSocketFD);
void acceptNewConnectionAndREcieveAndPrintItsData(int serverSocketFD);
void recieveAndPrintDataOnSeperateThread(struct AcceptedSocket *pSocket);
void sendRecievedMessageToTheOtherClients(char *buffer, int socketFD);

int main(){

	int serverSocketFD = createTCPIpv4Socket();
	struct sockaddr_in *serverAddress = createIPv4Address("", 2000);  //create the server socket

	int result = bind(serverSocketFD, (struct sockaddr *)serverAddress, sizeof(*serverAddress));  //tells the OS that this process wants the open port 8099 to listen on for incoming connections 

    if(result == 0) 
		printf("Server Bound Successfully.\n");

	// //make the server start listing on the specified port no. 
	int listenResult = listen(serverSocketFD, 10); // the 10, means that there is a backlog of 10, ie at a time 10 clients can connect. (ig)
	printf("Listen Result: %d\n", listenResult);
	// //the server will queue all the incoming connections upto 10 connections.
	// //before calling the next function which is the accept() function, the server will queue all the incoming connections.


	/**
	for multithreading, we need to run the acceptedIncomingConnection function in a seperate thread, as it is a blocking funciton, it will block the main thread. 
	*/

	startAcceptingIncomingConnections(serverSocketFD);  //once we started accpeting connectiosn on the main thread, this function becomes a bloockign function. 
	/**
	we cannot accept connections on a new thread, beacuse then the main thread is free so it moves ahead and returns 0 and terminates the process. alternately, we can use the join function to make the code blocking

	*/
	


	

	
	printf("Befoe the blockign accept code: \n");
	printf("Gello World\n");
	// //the accept() function returns the file descriptor of the connecting client. 
	// //if it returns a negative number it means soem error has occured.
	
	

	
	// shutdown(serverSocketFD, SHUT_RDWR);

	// printf("REsponse was %s\n", buffer);
	return 0;
}

void startAcceptingIncomingConnections(int serverSocketFD){
	while(true){

	struct AcceptedSocket *clientSocket = acceptIncommingConnection(serverSocketFD);
	acceptedSockets[acceptedSocketsCount++] = clientSocket;
	recieveAndPrintDataOnSeperateThread(clientSocket);
	
	
	}
	// pthread_t id;
	// pthread_create(&id, NULL, acceptNewConnectionAndREcieveAndPrintItsData, serverSocketFD); //creates a new thread in the OS and asks for a function, then it will run the function in the new thread. 
}

// void acceptNewConnectionAndREcieveAndPrintItsData(int serverSocketFD){

// 	while(true){

// 	struct AcceptedSocket *clientSocket = acceptIncommingConnection(serverSocketFD);
// 	recieveAndPrintDataOnSeperateThread(clientSocket);
	
	
// 	}

// }

void recieveAndPrintDataOnSeperateThread(struct AcceptedSocket *pSocket){
	pthread_t id;
	pthread_create(&id, NULL, recieveAndPrintIncomingData, pSocket->acceptedSocketFD);
	// recieveAndPrintIncomingData(clientSocket->acceptedSocketFD);

	
}

struct AcceptedSocket * acceptIncommingConnection(int serverSocketFD){
	struct AcceptedSocket* acceptedsocket = malloc(sizeof(struct AcceptedSocket)); //this retunrs a dynamically allocated structure. the user of the memory is responsible for freeing up the memory whcih we have allocated. 

	struct sockaddr_in clientAddress; 
	int clientAddressSize = sizeof( struct sockaddr_in);
	int clientSocketFD = accept(serverSocketFD, &clientAddress, &clientAddressSize); //this is a blocking function.

	acceptedsocket->address = clientAddress;
	acceptedsocket->acceptedSocketFD = clientSocketFD;
	acceptedsocket->acceptedSuccessfully = clientSocketFD > 0;
	if(!acceptedsocket->acceptedSuccessfully){
		acceptedsocket->error = clientSocketFD;
	}
	return acceptedsocket;
}


void recieveAndPrintIncomingData(int socketFD){
	char buffer[1024];
	while(true){
	
		ssize_t amountRecieved = recv(socketFD, buffer, 1024, 0);
		if(amountRecieved > 0){
			buffer[amountRecieved+1] = 0;
			printf("%s\n", buffer);
			sendRecievedMessageToTheOtherClients(buffer, socketFD);
			}
		if(amountRecieved == 0) break; 
	}

	close(socketFD);
}

void sendRecievedMessageToTheOtherClients(char *buffer, int socketFD){
	for(int i = 0; i < acceptedSocketsCount; i++){
		if(acceptedSockets[i]->acceptedSocketFD != socketFD){
			send(acceptedSockets[i]->acceptedSocketFD, buffer, strlen(buffer), 0);
		}
	}
}

