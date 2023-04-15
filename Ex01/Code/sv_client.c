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
    printf("Input data for student\n");
    while(1){
        fflush(stdin);
        student_info stu;
        printf("Enter id for student: ");
        scanf("%[^\n]%*c",stu.mssv);
        printf("Enter name for student: ");
        scanf("%[^\n]%*c",stu.name);
        printf("Enter birthday for student (yyyy/mm/dd): ");
        char date[20];
        scanf("%s",date);
        stu.date.year = atoi(strtok(date,"/"));
        stu.date.month = atoi(strtok(NULL,"/"));
        stu.date.date = atoi(strtok(NULL,"/"));
        getchar();
        printf("Enter average grade: ");
        scanf("%lf",&stu.average_point);
        getchar();
        printf("Do you want to send to Server (Y/N): ");
        char option;
        scanf("%c",&option);
        strcpy(stu.ip,argv[1]);
        time_t t;
        time(&t);
        strcpy(stu.current_time,ctime(&t));
        getchar();
        if(option=='Y'||option=='y'){
            printf("Sending....\n");
            send(sockfd,&stu,sizeof(stu),0);
        }
        printf("Do you want to enter data for other student?(Y/N): ");
        char option1;
        scanf("%c",&option1);
        getchar();
        if(option1=='N'||option1=='n'){
            break;
        }
    }
    close(sockfd);
}
