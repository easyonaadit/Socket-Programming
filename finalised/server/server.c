#include "../socketutils/socketutil.h"



#define PORT 8996
#define FILE_CODE "5Z7YtpvKeRgHb32oL9dQFxJwYNPmDz"
#define SIZE 1024

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
void sendFileToOtherClients(int socketFD);
void sendRecievedFileToTheOtherClients(unsigned char *buffer, int socketFD);

int main(){

	int serverSocketFD = createTCPIpv4Socket();
	struct sockaddr_in *serverAddress = createIPv4Address("", PORT);  //create the server socket

	int result = bind(serverSocketFD, (struct sockaddr *)serverAddress, sizeof(*serverAddress));  //tells the OS that this process wants the open port 8099 to listen on for incoming connections 

    if(result == 0) 
		printf("Server Bound Successfully.\n");

	// //make the server start listing on the specified port no. 
	int listenResult = listen(serverSocketFD, 10); // the 10, means that there is a backlog of 10, ie at a time 10 clients can connect. (ig)
    //returns 0 on success, -1on fail(errno )
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
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
    #pragma GCC diagnostic ignored "-Wint-conversion"
	pthread_create(&id, NULL, recieveAndPrintIncomingData, pSocket->acceptedSocketFD);
    #pragma GCC diagnostic pop
	// recieveAndPrintIncomingData(clientSocket->acceptedSocketFD);

	
}

struct AcceptedSocket * acceptIncommingConnection(int serverSocketFD){
	struct AcceptedSocket* acceptedsocket = malloc(sizeof(struct AcceptedSocket)); //this retunrs a dynamically allocated structure. the user of the memory is responsible for freeing up the memory whcih we have allocated. 

	struct sockaddr_in clientAddress; 
	int clientAddressSize = sizeof( struct sockaddr_in);
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
	// pthread_create(&id, NULL, recieveAndPrintIncomingData, pSocket->acceptedSocketFD);
	int clientSocketFD = accept(serverSocketFD, &clientAddress, &clientAddressSize); //this is a blocking function.
    #pragma GCC diagnostic pop

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
	
		ssize_t amountRecieved = recv(socketFD, buffer, 1024, 0); //recieves the file code
		// if(buffer[0] == 'G') printf("THIS IS HTTP GET");
		if(amountRecieved > 0){
            if(strcmp(buffer, FILE_CODE) == 0)/*checks for the file code*/{ 
                
                sendRecievedMessageToTheOtherClients(buffer, socketFD); //sendes the file code
                amountRecieved = recv(socketFD, buffer, 1024, 0); //recieves the file name
                buffer[amountRecieved+1] = 0;
                sendRecievedMessageToTheOtherClients(buffer, socketFD); //sends the file name
                sendFileToOtherClients(socketFD); //to recieve and send the payload
            }
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

void sendRecievedFileToTheOtherClients(unsigned char *buffer, int socketFD){
	for(int i = 0; i < acceptedSocketsCount; i++){
		if(acceptedSockets[i]->acceptedSocketFD != socketFD){
			send(acceptedSockets[i]->acceptedSocketFD, buffer, strlen(buffer), 0);
		}
	}
}

void sendFileToOtherClients(int socketFD){
    int n;
    // FILE *file;
    // char *fileName = "recievedFile.jpg";
    // // char *fileName2 = "recievedFile2.bmp";
    // char *fileName2 = "recievedFile2.mp3";
    unsigned char buffer[SIZE];

    // file = fopen(fileName2, "wb");
    int i = 0;

    while(1){
        // if(i == 8) break;
        // printf("BLOCKED BEFORE RECV: %d\n", i);
        n = recv(socketFD, buffer, SIZE, 0);
        // printf("BLOCKED AFTER RECV: %d\n", i);
        // printf("DATA IS: ");
        // for(int j = 0; j < SIZE; j++){
        //     printf("%c, ", buffer[j]);
        // }
        // printf("\n\n");
        printf("THIS IS CALL: %d, PAYLOAD: %d\n", i++, n);
        printf("[+]Recieved Data: {[%s]}\n", buffer);

        
        // for(int l = 0; l < n; l++){
        //     printf("%c", buffer[l]);
        // }
        // printf("\n");
        if(n <= 0){
            break;
            return;
        }
        else if(n < SIZE){
            unsigned char buffer2[n];
            for(int l = 0; l < n; l++){
                buffer2[l] = buffer[l];
            }
            // printf("\nThis is the if vale of n: %d\n", n);
            //     for(int l = 0; l < n; l++){
            //     printf("%c", buffer2[l]);
            // }
            // printf("\n");
            // printf("INITIAL BUFFER: %s\n", buffer2);
            // memset(buffer+n, 0, SIZE-n);
            // printf("MEMSET BUFFER: %s\n", buffer);
            // fputs(buffer2,file);


            // fwrite(buffer, n, 1, file);
            sendRecievedFileToTheOtherClients(buffer, socketFD);



            // printf("In the if-else part: %d\n", n);
            // unsigned char buffer2[n];
            // for(int k = 0; k < n; k++){
                
            //     buffer2[k] = buffer[k];
            //     printf("%c ", buffer2[k]);
            // }
            // printf("\n");
            // printf("Length of buffer2: %ld\n", strlen(buffer2));
            // unsigned char buffer3[82];
            // for(int k = 0; k < 82; k++){
                
            //     buffer3[k] = buffer2[k];
            //     printf("%c ", buffer3[k]);
            // }
            // fputs(buffer2, file);
            break;
        }
        else{
            // fprintf(file, "%s", buffer);


            // fwrite(buffer, SIZE, 1, file);
            sendRecievedFileToTheOtherClients(buffer, socketFD);


            // fputs(buffer, file);
            memset(buffer, 0, SIZE);
            // printf("[+]After Memset: [%s]\n\n", buffer);
            // bzero(buffer, SIZE);
        }
    }
    // fclose(file);
    return;
}

