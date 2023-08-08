#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    // http://httpbin.org/post
    
    int client = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    struct addrinfo * server_info;

    int ret = getaddrinfo("httpbin.org","http",NULL,&server_info);

    if(ret!=0||server_info==NULL){
        printf("Khong phan giai duoc ten mien\n");
        exit(1);
    }
    if(connect(client,server_info->ai_addr,server_info->ai_addrlen)){
        perror("connect() failed");
        exit(1);
    }

    char * request = "POST /post HTTP/1.1\r\n Host: httpbin.org\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 27\r\n\r\n";
    send(client,request,strlen(request),0);

    char * body = "param1=value1&param2=value2";
    send(client,body,strlen(body),0);

    char buf[2048];
    int data = recv(client,buf,sizeof(buf),0);
    if(data==0){
        printf("Khong nhan duoc phan hoi tu server\n");
        return 0;
    }
    buf[data] = 0;
    printf("%s\n",buf);
    close(client);
    return 0;

}