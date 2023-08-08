#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <malloc.h>

int main() {
    // http://lebavui.id.vn/samples/test.mp4
    
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct addrinfo *info;
    int ret = getaddrinfo("lebavui.id.vn", "http", NULL, &info);
    if (ret != 0 || info == NULL)
    {
        printf("Khong phan giai duoc ten mien.\n");
        exit(1);
    }

    if (connect(client, info->ai_addr, info->ai_addrlen)) {
        perror("connect() failed");
        exit(1);
    }
        
    // Gui yeu cau den server
    char *request = "GET /samples/test.mp4 HTTP/1.1\r\nHost: lebavui.id.vn\r\nConnection: close\r\n\r\n";
    send(client, request, strlen(request), 0);

    // Nhan header
    char buf[2048];
    char *header = NULL;
    int size = 0;
    char *pos = NULL;

    while (1)
    {
        int len = recv(client, buf, sizeof(buf), 0);
        if (len <= 0)
        {
            printf("Loi khi tai file.\n");
            close(client);
            exit(1);
        }
        
        header = realloc(header, size + len);
        memcpy(header + size, buf, len);
        size += len;

        if ((pos = memmem(header, size, "\r\n\r\n", 4)) != NULL)
            break;
    }

    int header_length = pos - header;
    header[header_length - 1] = 0;
    puts(header);

    int body_length = size - header_length - 4;
    FILE * f = fopen("download.mp4","wb");
    fwrite(pos+4,1,body_length,f);
    int downloadded = body_length;
    while(1){
        int data = recv(client,buf,sizeof(buf),0);
        if(data<=0){
            break;
        }
        fwrite(buf,1,data,f);
        downloadded+=data;
        printf("Downloading: %d\n",downloadded);
    }
    close(client);
    printf("Finished: %d bytes\n", downloadded);
    return 1;
}