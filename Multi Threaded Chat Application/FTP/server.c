#include "../socketutils/socketutil.h"

#define SIZE 1024

void writeFile(int newSocket){
    int n;
    FILE *file;
    char *fileName = "recievedFile.jpg";
    // char *fileName2 = "recievedFile2.bmp";
    char *fileName2 = "recievedFile2.mp3";
    unsigned char buffer[SIZE];

    file = fopen(fileName2, "wb");
    int i = 0;

    while(1){
        // if(i == 8) break;
        printf("BLOCKED BEFORE RECV: %d\n", i);
        n = recv(newSocket, buffer, SIZE, 0);
        printf("BLOCKED AFTER RECV: %d\n", i);
        // printf("DATA IS: ");
        // for(int j = 0; j < SIZE; j++){
        //     printf("%c, ", buffer[j]);
        // }
        // printf("\n\n");
        printf("THIS IS CALL: %d, PAYLOAD: %d\n", i++, n);
        printf("[+]Recieved Data: {[%s]}\n", buffer);

        
        for(int l = 0; l < n; l++){
            printf("%c", buffer[l]);
        }
        printf("\n");
        if(n <= 0){
            break;
            return;
        }
        else if(n < SIZE){
            unsigned char buffer2[n];
            for(int l = 0; l < n; l++){
                buffer2[l] = buffer[l];
            }
            printf("\nThis is the if vale of n: %d\n", n);
            for(int l = 0; l < n; l++){
            printf("%c", buffer2[l]);
        }
        printf("\n");
            // printf("INITIAL BUFFER: %s\n", buffer2);
            // memset(buffer+n, 0, SIZE-n);
            // printf("MEMSET BUFFER: %s\n", buffer);
            // fputs(buffer2,file);
            fwrite(buffer, n, 1, file);
            printf("In the if-else part: %d\n", n);
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
            fwrite(buffer, SIZE, 1, file);
            // fputs(buffer, file);
            memset(buffer, 0, SIZE);
            // printf("[+]After Memset: [%s]\n\n", buffer);
            // bzero(buffer, SIZE);
        }
    }
    fclose(file);
    return;
}

int main(){
    int socketFD = createTCPIpv4Socket(); //socket(AF_INET, SOCK_STREAM, 0);
    if(socketFD == -1){
        printf("[-] Error in creating socket");
        return 0;
    }
    printf("[+] Server Socket created\n");

    // struct sockaddr_in * createIPv4Address(char *ip, int port)
    struct sockaddr_in *serverAddress = createIPv4Address("", 2002);

    int result = bind(socketFD, (struct sockaddr *)serverAddress, sizeof(*serverAddress));  //tells the OS that this process wants the open port 8099 to listen on for incoming connections 

    if(result < 0) {
		printf("[-] Server Bound Successfully.\n");
        return 0;

    }
    printf("[+] Server Bound Succesfully\n");

    int listenResult = listen(socketFD, 10); // the 10, means that there is a backlog of 10, ie at a time 10 clients can connect. (ig)
    if(listenResult == 0)
	    printf("Listen Result: %d\n", listenResult);
    else{
        printf("[-] Error in listning.");
        return 0;
    }

     //accepting the clinetn
    struct sockaddr_in clientSocket;
    socklen_t addr_size = sizeof(clientSocket);

    int newSocket = accept(socketFD, (struct sockaddr*) &clientSocket, &addr_size);

    writeFile(newSocket);
    printf("[+] data written successfully");







}