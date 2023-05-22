#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/poll.h>


#define MAX_MSG_LEN 1024

int main(int argc, char *argv[]) {  
  if (argc < 4) {
    printf("Su dung: %s <ip-address> <recv-port> <send-port>\n", argv[0]);
    exit(1);
  }

  int recv_sockfd, send_sockfd;
  struct sockaddr_in recv_addr, send_addr;
  // Khởi tạo socket nhận
  recv_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (recv_sockfd < 0) {
    perror("Tao socket that bai");
    exit(1);
  }

  // Thiết lập địa chỉ IP và cổng của máy nhận
  memset(&recv_addr, 0, sizeof(recv_addr));
  recv_addr.sin_family = AF_INET;
  recv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  recv_addr.sin_port = htons(atoi(argv[2]));

  // Gắn địa chỉ IP và cổng vào socket nhận
  if (bind(recv_sockfd, (struct sockaddr *)&recv_addr, sizeof(recv_addr)) < 0) {
   perror("Error binding recv socket");
    exit(1);
  }


  // Khởi tạo socket gửi
  send_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (send_sockfd < 0) {
    perror("Tao socket that bai");
    exit(1);
  }

  // Thiết lập địa chỉ IP và cổng của máy chờ
  memset(&send_addr, 0, sizeof(send_addr));
  send_addr.sin_family = AF_INET;
  send_addr.sin_addr.s_addr = inet_addr(argv[1]);
  send_addr.sin_port = htons(atoi(argv[3]));
    //Poll
    struct pollfd fds[100];
    int nfds = 2;
    //Gan su kien nhan du lieu tu ban phim cho fds
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    //Gan su kien nahn du lieu nguoi chat
    fds[1].fd = recv_sockfd;
    fds[1].events = POLLIN;    
    char buf[MAX_MSG_LEN];
    while(1){
        int ret = poll(fds,nfds,-1);
        if (fds[0].revents & POLLIN) { // Nếu có dữ liệu từ bàn phím
            fgets(buf, sizeof(buf), stdin);
            sendto(send_sockfd, buf, strlen(buf), 0, (struct sockaddr *)&send_addr, sizeof(send_addr));
        }
        if (fds[1].revents & (POLLIN | POLLERR)) {// Sự kiện client
            ret = recvfrom(recv_sockfd, buf, MAX_MSG_LEN, 0, NULL, NULL);
            if (ret <= 0) {
                printf("Ket noi bi ngat\n");
                return 1; 
            }
            buf[ret] = 0;
            printf("Nhan du lieu tu nguoi chat %d: %s", fds[1].fd, buf);
        }
}
}