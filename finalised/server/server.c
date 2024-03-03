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
void sendFileToOtherClients(char *filename,int socketFD);
void sendRecievedFileToTheOtherClients(unsigned char *buffer, int socketFD);
void sendFileToOtherClientsPhase2(FILE *file,int socketFD);

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
	return 0;
}

void startAcceptingIncomingConnections(int serverSocketFD){
	while(true){
	struct AcceptedSocket *clientSocket = acceptIncommingConnection(serverSocketFD);
	acceptedSockets[acceptedSocketsCount++] = clientSocket;
	recieveAndPrintDataOnSeperateThread(clientSocket);
	}
}



void recieveAndPrintDataOnSeperateThread(struct AcceptedSocket *pSocket){
	pthread_t id;
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
    #pragma GCC diagnostic ignored "-Wint-conversion"
	pthread_create(&id, NULL, recieveAndPrintIncomingData, pSocket->acceptedSocketFD);
    #pragma GCC diagnostic pop
}

struct AcceptedSocket * acceptIncommingConnection(int serverSocketFD){
	struct AcceptedSocket* acceptedsocket = malloc(sizeof(struct AcceptedSocket)); //this retunrs a dynamically allocated structure. the user of the memory is responsible for freeing up the memory whcih we have allocated. 
	struct sockaddr_in clientAddress; 
	int clientAddressSize = sizeof( struct sockaddr_in);
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
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
		if(amountRecieved > 0){
            if(strcmp(buffer, FILE_CODE) == 0)/*checks for the file code*/{ 
                
                sendRecievedMessageToTheOtherClients(buffer, socketFD); //sendes the file code
                amountRecieved = recv(socketFD, buffer, 1024, 0); //recieves the file name
                buffer[amountRecieved] = 0;
                sendRecievedMessageToTheOtherClients(buffer, socketFD); //sends the file name
                sendFileToOtherClients(buffer, socketFD); //to recieve and send the payload
            }
			buffer[amountRecieved] = 0;
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

void sendFileToOtherClients(char *filename, int socketFD){
    int n;
    unsigned char buffer[SIZE];
    FILE *file = fopen(filename, "wb");

    int i = 0;

    while(1){
        n = recv(socketFD, buffer, SIZE, 0);
        printf("THIS IS CALL: %d, PAYLOAD: %d\n", i++, n);
        printf("[+]Recieved Data: {[%s]}\n", buffer);
        if(n <= 0){
            break;
            return;
        }
        else if(n < SIZE){
            unsigned char buffer2[n];
            for(int l = 0; l < n; l++){
                buffer2[l] = buffer[l];
            }
            fwrite(buffer, n, 1, file);
            // sendRecievedFileToTheOtherClients(buffer, socketFD);
            break;
        }
        else{
            fwrite(buffer, SIZE, 1, file);
            // sendRecievedFileToTheOtherClients(buffer, socketFD);
            memset(buffer, 0, SIZE);
        }
    }
    sendFileToOtherClientsPhase2(file, socketFD);
    fclose(file);
    return;
}

void sendFileToOtherClientsPhase2(FILE *file,int socketFD){
    unsigned char data[SIZE];
    // bzero(data, SIZE);
    memset(data, 0, SIZE);
    int i = 0;
    size_t bytesRead;

    while((bytesRead = fread(data, 1, SIZE, file)) > 0){
        if(bytesRead < SIZE){
            printf("IN THE CLIENT IF ELSE\nBYTES READ: %ld\nSENDT DATA: ", bytesRead);
            // int payload = send(socketFD, data, bytesRead, 0);
            sendRecievedFileToTheOtherClients(data, socketFD);
            for(int k = 0; k < bytesRead; k++){
                printf("%c", data[k]);
            }
            // unsigned char buffer[1];
            // buffer[0] = EOF;
            // payload = send(socketFD, buffer, 1, 0);
            break;
        }
        // int payload = send(socketFD, data, bytesRead, 0);
        sendRecievedFileToTheOtherClients(data, socketFD);
        // printf("THIS IS CALL: %d, PAYLOAD: %d\n", i++, payload);
        printf("[+]Sent Data: %s\n",data);
        // if(payload == -1){
        //     perror("[-] error in sending data");
        //     exit(1);
        // }
        // bzero(data, SIZE);
        memset(data, 0, SIZE);
        // printf("[+]After having memset: [%s]\n", data);
    }

    // while(fgets(data, SIZE, file) != NULL){
    //     printf("[+]Gotten Data: %s\n\n",data);
    //     int payload = send(socketFD, data, sizeof(data), 0);
    //     if(payload == -1){
    //         perror("[-] error in sending data");
    //         exit(1);
    //     }
    //     printf("THIS IS CALL: %d, PAYLOAD: %d\n", i++, payload);
    //     // bzero(data, SIZE);
    //     memset(data, 0, SIZE);
    //     printf("[+]After having memset: [%s]\n", data);
    // }

printf("\nBROKE OUT OF LOOP");


}

