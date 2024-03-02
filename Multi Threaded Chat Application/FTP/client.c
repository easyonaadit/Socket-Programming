#include "../socketutils/socketutil.h"

#define SIZE 1024
// #define mav val 10

void sendFile(FILE *file, int socketFD);

int main(){
	printf("Hello world\n");
    // printf("%d", max val);
	int socketFD = createTCPIpv4Socket();
    printf("Socket FD: %d\n", socketFD);

	struct sockaddr_in *address = createIPv4Address("127.0.0.1", 2002);

	int result = connect(socketFD, (struct sockaddr *)address, sizeof(*address));

	if(result == 0){
		printf("Connection is successufl.\n");
	}
    if(result < 0){
        printf("[-] error in connecting Socket\n");
    }

    FILE *file;
    char *fileName, *fileName2;
    fileName = "demo.txt";
    // fileName2 = "image1.jpg";
    // fileName2 = "all_gray.bmp";
    fileName2 = "audio_sent.mp3";

    file = fopen(fileName2, "rb");
    if(file == NULL){
        printf("[-] Error in opening file");
        return 0; 
    }
    printf("[+] File opened succesfully.\n");

    sendFile(file, socketFD);
    printf("[+] file sent succesfully");



	// char *name = NULL;
	// size_t nameSize = 0;
	// printf("please enter your name\n");
	// size_t nameCount = getline(&name, &nameSize, stdin);
	// name[nameCount-1] = 0; //this removes the \n at the end of the name string. 


	// char *line = NULL;
	// size_t lineSize = 0;
	// printf("Type and we will send(\"exit\" to exit)");

	// startListningAndPrintMessagesOnNewThread(socketFD);

	// char buffer[1024];

	// /*this while loop to send stuff is blocking code. if we also want to recieve things, we will have to use multithreading*/
	// while(true){
	// 	size_t charCount = getline(&line, &lineSize, stdin);
	// 	line[charCount-1] = 0;
	// 	sprintf(buffer, "%s: %s", name,line);

	// 	if(charCount > 0){
	// 		if(strcmp(line, "exit") == 0){size_t amountWasSent = send(socketFD, "", 0, 0);
	// 		break;}
	// 		size_t amountWasSent = send(socketFD, buffer, strlen(buffer), 0);
	// 	}
	// }

	close(socketFD);


	// char *message;
	// message = "GET \\ HTTP/1.1\r\nHost:google.com\r\n\r\n";
	// send(socketFD, message, strlen(message), 0);

	// char buffer[1024];
	// recv(socketFD, buffer, 1024, 0);

	printf("Socket closed succesfully\n");
	return 0;
}

void sendFile(FILE *file, int socketFD){
    unsigned char data[SIZE];
    // bzero(data, SIZE);
    memset(data, 0, SIZE);
    int i = 0;
    size_t bytesRead;

    while((bytesRead = fread(data, 1, SIZE, file)) > 0){
        if(bytesRead < SIZE){
            printf("IN THE CLIENT IF ELSE\nBYTES READ: %ld\nSENDT DATA: ", bytesRead);
            int payload = send(socketFD, data, bytesRead, 0);
            for(int k = 0; k < bytesRead; k++){
                printf("%c", data[k]);
            }
            // unsigned char buffer[1];
            // buffer[0] = EOF;
            // payload = send(socketFD, buffer, 1, 0);
            break;
        }
        int payload = send(socketFD, data, bytesRead, 0);
        printf("THIS IS CALL: %d, PAYLOAD: %d\n", i++, payload);
        printf("[+]Sent Data: %s\n",data);
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

printf("\nBROKE OUT OF LOOP");
}

