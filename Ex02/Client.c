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
    if(argc!=3){
        printf("Please use this fomat: sv_client <server_ip_address> <server_port>\n");
    }
    sockaddr_in client;
    client.sin_addr.s_addr = inet_addr(argv[1]);
    client.sin_family = AF_INET;
    client.sin_port = htons(atoi(argv[2]));
    //Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd==-1){
        printf("Error: %d - %s\n",errno, strerror(errno));
        printf("Here\n");
        exit(1);
    }
    socklen_t client_len = sizeof(client);
    if(connect(sockfd,(struct sockaddr *)&client,client_len)){
        printf("Error: %d - %s\n",errno, strerror(errno));
        exit(1);
    }
    while (1)
    {
        char name[20];
        char type[30];
        scanf("%s",name);
        scanf("%s",type);
        char buf[256];
        sprintf(buf,"%s %s",name,type);
        send(sockfd,buf,sizeof(buf),0);
        break;
    }
    close(sockfd);
}
