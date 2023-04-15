#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>

typedef struct sockaddr_in sockaddr_in;

int main(int argc, char *argv[]){
    //Check argv
    if (argc != 4) {
        printf("Please use this format: tcp_server <server_port> <welcome_message_file> <client_message_file>\n");
        exit(1);
    }
    //Initial value for address
    sockaddr_in addrserver;
    addrserver.sin_port = htons(atoi(argv[1]));
    addrserver.sin_family = AF_INET;
    addrserver.sin_addr.s_addr = htonl(INADDR_ANY);
    //Create socket to connecto to Server
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(sockfd!=-1){
        printf("Socket created: %d\n",sockfd);
    }
    else{
        printf("Failed to create socket: %d - %s\n",errno, strerror(errno));
        perror("socket() failed");
        exit(1);
    }
    //Bind at port
    socklen_t addserver_len = sizeof(addrserver);
    int bind_fd = bind(sockfd, (struct sockaddr *)&addrserver, addserver_len);
    if(bind_fd==-1){
        printf("Failed to bind socket: %d - %s\n",errno, strerror(errno));
        perror("bind() failed");
        exit(1);
    }
    else{
        printf("Bind suceesfully!");
    }
    //Listen for incoming connections
    if(listen(sockfd,5)==-1){
        printf("Failed to listen for incoming connections\n");
        exit(1);
    }
    //Information of Client connect to Server
    sockaddr_in addrclient;
    socklen_t addrclient_len = sizeof(addrclient);
    int clientid = accept(sockfd,(struct sockaddr*)&addrclient,&addrclient_len);
    if(clientid==-1){
        printf("Failed to bind socket: %d - %s\n",errno, strerror(errno));
        perror("accept() failed");
        exit(1);
    }
    printf("Client IP: %s:%d\n", inet_ntoa(addrclient.sin_addr), ntohs(addrclient.sin_port));
//   //Open file and send hello message

    char path[]="/media/orind30/E/Network-Programing/";
    strcat(path,argv[2]);
    if(strstr(path,"txt")==NULL){
        strcat(path,".txt\0");
    }
    printf("Send hello message from %s\n",path);
    FILE *fp = fopen(path, "r");
    if(fp == NULL){
        printf("Failed to open file: %s\n", strerror(errno));
        perror("fopen() failed");
        exit(1);
    }
    else{
        printf("File opened successfully!\n");
    }
    char buf[1024];
    while(fgets(buf, sizeof(buf), fp)!= NULL){
        write(clientid,buf,strlen(buf));
    }
    write(clientid,"exit",sizeof("exit"));
    fclose(fp);
    //Get input from client
    char path_client_input_file[] = "/media/orind30/E/Network-Programing/";
    strcat(path_client_input_file,argv[3]);
    if(strstr(path_client_input_file,"txt")==NULL){
        strcat(path_client_input_file,".txt\0");
    }
    printf("Writing to file: %s\n",path_client_input_file);
    FILE *input_file = fopen(path_client_input_file,"a");
    while(1){
        int data_rec = recv(clientid,buf,sizeof(buf),0);
        if(data_rec<=0){
            printf("Connection closed!");
            break;
        }
        else{
            buf[data_rec] = 0;
            fprintf(input_file,"%s",buf);
        }
    }
    //Close file and connection
    fclose(input_file);
    close(clientid);
    close(sockfd);
    return 0;
}