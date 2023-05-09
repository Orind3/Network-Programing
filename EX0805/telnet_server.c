#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<time.h>
#include<poll.h>
#include<string.h>

typedef struct sockaddr_in sockaddr_in;

typedef struct client_ifo{
    int client_accept;
    char client_id[100];
    char client_name[100];
}client_ifo;

typedef struct user{
    char user_name[100];
    char pass_word[100];
}user;

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
    user user_info[100];
    int nfds = 1;
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;
    FILE *f = fopen("user_login.txt","r");
    int number_user = 0;
    while(!feof(f)){
        fscanf(f,"%s %s",user_info[number_user].user_name,user_info[number_user].pass_word);
        number_user++;
    } 
    fclose(f);
    while(1){
        //timeout = 1 block it
        int ret = poll(fds,nfds,-1);

        //Event create, delete client
        if(fds[0].revents & POLLIN){
            int client_id = accept(sockfd,NULL,NULL);
            int login = 0;
            strcpy(buf,"Enter username and password\n");
            send(client_id,buf,strlen(buf),0);
            while (login==0)
            {  
                int data = recv(client_id,buf,sizeof(buf),0);
                if(data<0){
                    printf("%d - %s\n",errno, strerror(errno));
                    break;
                }
                else if(data==0){
                    printf("Connection lost!\n");
                    break;
                }
                else{
                    client_ifo new_client;
                    buf[data-1] = 0;
                    if(strchr(buf,' ')==NULL){
                        strcpy(buf,"Invalid login: User name or pass word invalid!\n");
                        send(client_id,buf,strlen(buf),0);
                        continue;
                    }
                    else{
                        char * pch;
                        char username[100],password[100];
                        pch = strtok(buf," ");
                        strcpy(username,pch);
                        pch = strtok(NULL," ");
                        strcpy(password,pch);
                        int i = 0;
                        for(; i < number_user; i++){
                            if(!strcmp(username,user_info[i].user_name)&&!strcmp(password,user_info[i].pass_word)){
                                strcpy(buf,"Hello Client\n");
                                new_client.client_accept = client_id;
                                send(client_id,buf,strlen(buf),0);
                                clients[nfds] = new_client;
                                fds[nfds].events = POLLIN;
                                fds[nfds].fd = client_id;
                                nfds++;
                                login = 1;
                                break;
                            }
                        }
                        if(login==0){
                            strcpy(buf,"Invalid login: User name or pass word invalid!\n");
                            send(client_id,buf,strlen(buf),0);
                        }
                    }
                }
            }
        }
        for(int i = 1; i <= nfds; i++){
            if(fds[i].revents & (POLLIN | POLLERR)){
                int data = recv(clients[i].client_accept,buf,sizeof(buf),0);
                if(data<0){
                    printf("%d - %s\n",errno, strerror(errno));
                }
                else if(data==0){
                    printf("Connection lost!\n");
                    clients[i]=clients[nfds-2];
                    nfds--;
                }
                else{
                    buf[data-1] = 0;
                    char output[20] = " > out.txt";
                    strcat(buf,output);
                    system(buf);
                    FILE * f = fopen("out.txt","r");
                    while(fgets(buf,sizeof(buf),f)){
                        send(clients[i].client_accept,buf,strlen(buf),0);
                    }
                    fclose(f);
                }
            }
        }
    }

    close(sockfd);
}
