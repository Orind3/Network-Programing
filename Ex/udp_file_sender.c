#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<time.h>


typedef struct sockaddr_in sockaddr_in;



int main(int argc,char * argv[]){
    if(argc!=4){
        printf("Please use format: <IP address> <Port> <File name>\n");
        exit(1);
    }
    sockaddr_in client;
    client.sin_addr.s_addr = inet_addr(argv[1]);
    client.sin_family = AF_INET;
    client.sin_port = htons(atoi(argv[2]));
    //Create socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(sockfd==-1){
        printf("Error: %d - %s\n",errno, strerror(errno));
        exit(1);
    }

    while(1){
        char cont[256];
        printf("Enter content for file: \n");
        scanf("%[^\n]%*c",cont);
        char buf[1000];
        sprintf(buf,"%s %s",argv[3],cont);
        socklen_t client_size = sizeof(client);
        int data_send = sendto(sockfd,buf,strlen(buf),0,(struct sockaddr*)&client,client_size);
        if(data_send==-1){
            printf("Error: %d - %s\n",errno,strerror(errno));
            break;
        }
        break;
    }
    close(sockfd);
}
