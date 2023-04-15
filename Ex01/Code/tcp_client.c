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

    if(argc!=3){
        printf("Please use this fomat: cp_client <server_ip_address> <server_port>\n");
    }

    //Initial value for address
    sockaddr_in addrclient;
    addrclient.sin_port = htons(atoi(argv[2]));
    addrclient.sin_family = AF_INET;
    addrclient.sin_addr.s_addr = inet_addr(argv[1]);
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
    socklen_t addrclient_len = sizeof(addrclient);
    int connection = connect(sockfd,(struct sockaddr *) &addrclient,addrclient_len);
    if(connection==-1){
        printf("Failed to connect: %d - %s\n",errno, strerror(errno));
        perror("connect() failed");
        exit(1);
    }
    else{
        printf("Connected\n");
    }
    char buf[1024];
while(1){
    int data_rec = recv(sockfd,buf,sizeof(buf),0);
    if(data_rec<=0){
        break;
    }
    else{
        buf[data_rec] = 0;
        if(!strncmp(buf,"exit",4)){
            break;
        }
        printf("%s",buf);
    }
}
    printf("Send data for Server.....\n");
    printf("Type exit to close connection!\n");
    while(1){
        fgets(buf,sizeof(buf),stdin);
        if(!strncmp(buf,"exit",4)){
            break;
        }
        send(sockfd,buf,strlen(buf),0);
    }
    close(sockfd);
}