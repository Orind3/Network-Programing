#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>

void signal_handler(){
    wait(NULL);
    return;
}

void *handle_client(void * client){
    int client_id = *(int *) client;
    char buf[2048];
    int data = recv(client_id,buf,sizeof(buf),0);
    buf[data] = 0;
    if(data<=0){
        perror("recv() failed");
        return NULL;
    }
    char method[16];
    char path[256];
    sscanf(buf,"%s %s",method,path);
    if(!strcmp(path,"/web")){
        char * response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        send(client_id,response_header,strlen(response_header),0);
        char * response_body = "<html><body><h1 style = {color: \"red\"}>Web</h1></body></html>";
        send(client_id,response_body,strlen(response_body),0);
    }
    else if(!strcmp(path,"/photo")){
        char * response_header = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nConnection: close\r\n\r\n";
        send(client_id,response_header,strlen(response_header),0);
        FILE * f = fopen("image.jpeg","rb");
        int len = 0;
        while(len = fread(buf,1,sizeof(buf),f)){
            send(client_id,buf,len,0);
        };
        fclose(f);
    }
    else if(!strcmp(path,"/audio")){
        char response_header[2048];
        FILE * f = fopen("download.mp3","rb");
        fseek(f,0,SEEK_END);
        long fsize = ftell(f);
        fseek(f,0,SEEK_SET);
        sprintf(response_header,"HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nContent-Type: audio/mp3\r\nConnection: keep-alive\r\n\r\n",fsize);
        send(client_id,response_header,strlen(response_header),0);
        int len = 0;
        while(len = fread(buf,1,sizeof(buf),f)) {
            send(client_id,buf,len,0);
        }
        fclose(f);
    }
    else{
        char * response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        send(client_id,response_header,strlen(response_header),0);
        char * response_body = "<html><body><h1 style = {color: \"red\"}>Hello world</h1></body></html>";
        send(client_id,response_body,strlen(response_body),0);
    }
    close(client_id);
}

int main(){

    int server = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(server==-1){
        perror("socket() failed");
    }
    struct sockaddr_in server_addr;
    server_addr.sin_port = htons(9000);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(server,(struct sockaddr *) &server_addr,sizeof(server_addr))){
        perror("bind() failed");
        exit(1);
    }

    if(listen(server,5)){
        perror("listen() failed");
    }

    signal(SIGPIPE, signal_handler);

    while(1){
        int client = accept(server,NULL,0);
        if(client==-1){
            perror("accept() failed");
            continue;
        }
        printf("New client connected!\n");
        pthread_t client_thread;
        pthread_create(&client_thread,NULL,handle_client,&client);
        pthread_detach(client_thread);
    }

}