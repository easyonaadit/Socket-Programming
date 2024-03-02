#include "../socketutils/socketutil.h"

void startListningAndPrintMessagesOnNewThread(int socketFD);
void listenAndPrint(int socketFD);
void sendFileInBinaryFormat(int socketFD, char *filename);
void sendFile(FILE *file, int socketFD);
void writeFile(int newSocket);

#define PORT 8996
#define FILE_CODE "5Z7YtpvKeRgHb32oL9dQFxJwYNPmDz"
#define SIZE 1024


int main(){
    int socketFD = createTCPIpv4Socket();
    if(socketFD < 0){
        perror("[-] Error in creating socket.");
        exit(1);
    }
    else{
        printf("[+] Socket Created Successfully.\n");
    }

    struct sockaddr_in *address = createIPv4Address("127.0.0.1", PORT);

    

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
    int result = connect(socketFD, address, sizeof(*address));
    #pragma DCC diagnostic pop

	if(result == 0){
        printf("[+] Socket Connected Successfully.\n");
	}
    else{
		perror("[-] Error in connecting socket.");
        exit(1);
    }

    char *name = NULL;
	size_t nameSize = 0;
	printf("Please enter your name\n");
	size_t nameCount = getline(&name, &nameSize, stdin);
	name[nameCount-1] = 0; //this removes the \n at the end of the name string. 



    char *line = NULL;
	size_t lineSize = 0;
	printf("Start Chatting \n(Enter: `$$EXIT` to exit) \n(Enter: `$$FILE` followed by file name to send a file)\n");

	startListningAndPrintMessagesOnNewThread(socketFD);
    char buffer[1024];

	/*this while loop to send stuff is blocking code. if we also want to recieve things, we will have to use multithreading*/
	while(true){
		size_t charCount = getline(&line, &lineSize, stdin);
		line[charCount-1] = 0;
        sprintf(buffer, "%s: %s", name,line);

		

		if(charCount > 0){
			if(strcmp(line, "$$EXIT") == 0){
                size_t amountWasSent = send(socketFD, "", 0, 0);
			    break;
            }
            if(strcmp(line, "$$FILE") == 0){
                printf("Enter the file name\n");
                size_t fileNameCount = getline(&line, &lineSize, stdin);
                line[fileNameCount-1] = 0;
                // printf("THIS IS THE FILE NAME: %s", line);

                size_t FILE_CODE_LENGTH = strlen(FILE_CODE);
                size_t amountWasSent = send(socketFD, FILE_CODE, FILE_CODE_LENGTH, 0); //sending the file code
                amountWasSent = send(socketFD, line, fileNameCount, 0); // sending the file name

                sendFileInBinaryFormat(socketFD, line); //to send the file payload


                
            }
            else{
                size_t amountWasSent = send(socketFD, buffer, strlen(buffer), 0);

            }
		}
	}


    
    return 0;


}

void startListningAndPrintMessagesOnNewThread(int socketFD){
	pthread_t id;
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wint-conversion"
	pthread_create(&id, NULL, listenAndPrint, socketFD);
    #pragma GCC diagnostic pop
}

void listenAndPrint(int socketFD){
	char buffer[1024];
	while(true){
	
		ssize_t amountRecieved = recv(socketFD, buffer, 1024, 0);
		if(amountRecieved > 0){
            if(strcmp(buffer, FILE_CODE) == 0)/*recieves the file code*/{
                printf("FILE CODE IS : %s\n", buffer);
                writeFile(socketFD); //recieves the file name in this function itself 
            }
            else{
                buffer[amountRecieved+1] = 0;
                printf("%s\n", buffer);
            }	
        }
		if(amountRecieved == 0) break; 
	}

	int closeResult = close(socketFD);
    if(closeResult == -1){
        perror("[+] Error in closing socket: ");
        printf("%d", errno);
    }

}

void sendFileInBinaryFormat(int socketFD, char *filename){
    FILE *file = fopen(filename, "rb");;

    if(file == NULL){
        printf("[-] Error in opening file");
        return; 
    }
    // printf("[+] File opened succesfully.\n");

    sendFile(file, socketFD);
    printf("[+] file sent succesfully");
    return;

}

void sendFile(FILE *file, int socketFD){
    unsigned char data[SIZE];
    // bzero(data, SIZE);
    memset(data, 0, SIZE);
    int i = 0;
    size_t bytesRead;

    while((bytesRead = fread(data, 1, SIZE, file)) > 0){

        /**
        if(bytesRead < SIZE){
            // printf("IN THE CLIENT IF ELSE\nBYTES READ: %ld\nSENDT DATA: ", bytesRead);
            int payload = send(socketFD, data, bytesRead, 0);
            // for(int k = 0; k < bytesRead; k++){
            //     printf("%c", data[k]);
            // }
            // unsigned char buffer[1];
            // buffer[0] = EOF;
            // payload = send(socketFD, buffer, 1, 0);
            break;
        }

        */
        int payload = send(socketFD, data, bytesRead, 0);
        // printf("THIS IS CALL: %d, PAYLOAD: %d\n", i++, payload);
        // printf("[+]Sent Data: %s\n",data);
        if(payload == -1){
            perror("[-] error in sending data");
            exit(1);
        }
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

// printf("\nBROKE OUT OF LOOP");
}

void writeFile(int newSocket){
    char fileName[SIZE];
    int n = recv(newSocket, fileName, SIZE, 0);

    printf("\033[0;31m"); // Set color to red
                printf("FILE NAME IS: %s\n", fileName);
                printf("\033[0m"); // Reset color to default
    
    FILE *file;

    
    // char *fileName = "recievedFile.jpg";
    // char *fileName2 = "recievedFile2.bmp";
    // char *fileName2 = "recievedFile2.mp3";
    unsigned char buffer[SIZE];

    file = fopen(fileName, "ab");
    if(file == NULL){
        printf("[-] Error in opening file\n");
        return; 
    }
    int i = 0;

    while(1){
        // if(i == 8) break;
        // printf("BLOCKED BEFORE RECV: %d\n", i);
        n = recv(newSocket, buffer, SIZE, 0);
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
            printf("\nThis is the if vale of n: %d\n ", n);
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