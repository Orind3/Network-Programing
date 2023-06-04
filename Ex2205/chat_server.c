#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<time.h>
#include<poll.h>
#include<ctype.h>


void normalize_string(char *str) {
    int len = strlen(str);
    int i, j = 0;
    int count_spaces = 0;

    // Loại bỏ khoảng trắng ở đầu chuỗi
    while (isspace(str[j])) {
        j++;
    }

    // Viết hoa chữ cái đầu tiên của từ
    str[j] = toupper(str[j]);

    // Xóa khoảng trắng ở giữa và ở cuối chuỗi
    for (i = j + 1; i <= len; i++) {
        if (isspace(str[i]) || str[i] == '\0') {
            if (!isspace(str[i - 1])) {
                count_spaces++;
            }
        } else {
            str[i] = tolower(str[i]);
        }
    }

    // Xóa khoảng trắng ở giữa các từ
    int k = 0;
    for (i = j; i <= len; i++) {
        if (!isspace(str[i])) {
            str[k++] = str[i];
        } else if (!isspace(str[i - 1])) {
            str[k++] = ' ';
        }
    }

   // Xóa khoảng trắng ở cuối chuỗi
    if (isspace(str[k - 1])) {
        k--;
    }

    str[k] = '\n';
    str[k+1] = 0;
}

typedef struct sockaddr_in sockaddr_in;

typedef struct client_ifo{
    int client_accept;
    char client_id[100];
    char client_name[100];
}client_ifo;


int main(int argc,char * argv[]){
    sockaddr_in server;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi("9000"));
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

    client_ifo clients[100];
    char buf[256];
    struct pollfd fds[100];
    int nfds = 1;
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;

    while(1){
        //timeout = 1 block it
        printf("Waiting for somthing happen.\n");
        int ret = poll(fds,nfds,-1);

        //Event create, delete client
        if(fds[0].revents & POLLIN){
            int client_id = accept(sockfd,NULL,NULL);
            client_ifo new_client;
            new_client.client_accept = client_id;
            char hello[] = "Hello";
            char clientConnected[] = "clients connected\n";
            sprintf(buf,"%s %d %s",hello,nfds,clientConnected);
            send(client_id,buf,strlen(buf),0);
            clients[nfds] = new_client;
            fds[nfds].events = POLLIN;
            fds[nfds].fd = client_id;
            nfds++;
        }
        int i = 1;
        for(i = 1; i <= nfds; i++){
            if(fds[i].revents & (POLLIN | POLLERR)){
                int data = recv(clients[i].client_accept,buf,sizeof(buf),0);
                if(data<=0){
                    printf("Something wrong!\n");
                    clients[i]=clients[nfds-2];
                    nfds--;
                }
                else{
                    normalize_string(buf);
                    send(clients[i].client_accept,buf,strlen(buf),0);
                }
            }
        }
    }


    close(sockfd);
}
