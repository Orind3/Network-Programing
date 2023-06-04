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
            while (1)
            {
                int data = recv(client_id,buf,sizeof(buf),0);
                if(data<=0){
                    printf("Something wrong!");
                }
                else{
                    client_ifo new_client;
                    buf[data] = 0;
                    if(strchr(buf,':')==NULL){
                        continue;
                    }
                    if(strchr(buf,' ')!=NULL){
                        continue;
                    }
                    char * ptr = strtok(buf,":");
                    strcpy(new_client.client_id,ptr);
                    ptr = strtok(NULL,":");
                    strcpy(new_client.client_name,ptr);
                    strcpy(buf,"Hello Client\n");
                    new_client.client_accept = client_id;
                    send(client_id,buf,strlen(buf),0);
                    clients[nfds] = new_client;
                    fds[nfds].events = POLLIN;
                    fds[nfds].fd = client_id;
                    nfds++;
                    break;
                }
            }
        }
        int i = 1;
        for(i = 1; i <= nfds; i++){
            printf("Watting for typing...\n");
            if(fds[i].revents & (POLLIN | POLLERR)){
                int data = recv(clients[i].client_accept,buf,sizeof(buf),0);
                if(data<=0){
                    printf("Something wrong!\n");
                    clients[i]=clients[nfds-2];
                    nfds--;
                }

                else{
                    buf[data] = 0;
                    char send_to_client[256];
                    memset(send_to_client, 0, sizeof(send_to_client));
                    strncpy(send_to_client,clients[i].client_id,strlen(clients[i].client_id));
                    strcat(send_to_client,":");
                    if(!strncmp(buf,"to_",3)&&strchr(buf,':')){
                        char * to = buf+3;
                        to = strtok(to,":");
                        int j = 0;
                        for(;j<=nfds;j++){
                            if(!strcmp(to,clients[j+1].client_name)||!strcmp(to,clients[j+1].client_id)){
                                to = strtok(NULL,":");
                                strcat(send_to_client,to);
                                send(clients[j+1].client_accept,send_to_client,strlen(send_to_client),0);
                            }
                        }
                    }
                    else{
                        strcat(send_to_client,buf);
                        int j = 1;
                        for(; j <= nfds; j++){
                            if(i!=j){
                                send(clients[j].client_accept,send_to_client,strlen(send_to_client),0);
                            }
                        }                       
                    }
                }
            }
        }
    }


    close(sockfd);
}
