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
    sockaddr_in server;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    //Create socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd==-1){
        printf("Error: %d - %s\n",errno, strerror(errno));
        printf("1\n");
        exit(1);
    }
    socklen_t server_len = sizeof(server);
    if(bind(sockfd,(struct sockaddr *)&server, server_len)){
        printf("Error: %d - %s\n",errno, strerror(errno));
        printf("2\n");
        exit(1);
    }

    char buf[1000];

    while(1){
        struct sockaddr client;
        socklen_t size_of_client = sizeof(client);
        int data_rec = recvfrom(sockfd,buf,sizeof(buf),0,&client,&size_of_client);
        if(data_rec==-1){
            printf("Error: %d - %s\n",errno, strerror(errno));
            exit(1);
        }
        printf("%d - %s\n",data_rec,buf);
        buf[data_rec] = 0;
        break;
    }
    char file[1000];
    char cont[1000];
    char * space = strchr(buf,' ');
    strcpy(cont,space+1);
    *space = 0;
    strcpy(file,buf);
    FILE * ptr = fopen(file,"w");
    fprintf(ptr,"%s",cont);
    fclose(ptr);
    close(sockfd);
}
