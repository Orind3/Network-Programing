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


typedef struct birthday
{
    int date;
    int month;
    int year;
}birthday;

typedef struct student_info
{   
    char mssv[1000];
    char name[1000];
    birthday date;
    double average_point;
    char current_time[1000];
    char ip[1000];
}student_info;


int main(int argc,char * argv[]){

    sockaddr_in server;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[1]));
    //Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
    if(listen(sockfd,5)==-1){
        printf("Failed to listen for incoming connections\n");
        exit(1);
    }
    sockaddr_in client;
    socklen_t client_len = sizeof(client);

    int client_id = accept(sockfd,(struct sockaddr*)&client,&client_len);

    if(client_id==-1){
        printf("Error: %d - %s\n",errno, strerror(errno));
        printf("3\n");
        exit(1);
    }
    char * str = (char*)malloc(10);
    int data_len = 100;
    while(1){
        char buf[256];
        int data_rec = recv(client_id,buf,sizeof(buf),0);
        if(data_rec>0){
            data_len+=data_rec;
            str = (char*) realloc(str,data_len+1);
            strcat(str,buf);
        }
        else{
            printf("\nConnection closed!\n");
            break;
        }
    }
    printf("%s\n",str);
    int count = 0;
    printf("%s\n",str);
    str = strstr(str,"0123456789");
    while(strstr(str,"0123456789")!=NULL){
        count++;
        str = strstr(str,"0123456789");
        str+=10;
    };
    printf("So luong 0123456789 la: %d\n",count);
    close(client_id);
    close(sockfd);
}
