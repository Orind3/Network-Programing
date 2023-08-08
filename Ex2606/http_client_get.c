#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    // http://httpbin.org/get
    
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct addrinfo *info;
    int ret = getaddrinfo("httpbin.org", "http", NULL, &info);
    if (ret != 0 || info == NULL)
    {
        printf("Khong phan giai duoc ten mien.\n");
        exit(1);
    }

    if (connect(client, info->ai_addr, info->ai_addrlen)) {
        perror("connect() failed");
        exit(1);
    }
        
    char * request = "GET /get?param1=value1&param2=value2 HTTP/1.1\r\nHost: httpbin.org\r\n\r\n";

    send(client,request,strlen(request),0);

    char buf[2048];
    int len = recv(client,buf,sizeof(buf),0);
    buf[len] = 0;

    printf("%s\n",buf);
    
    close(client);
    return 0;
    
}