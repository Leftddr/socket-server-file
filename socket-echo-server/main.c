#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

#define  BUFF_SIZE   1024

void
main(void) {
    int server_socket;
    int client_socket;
    int client_addr_size;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    char buff[BUFF_SIZE + 5];
    char file_name[BUFF_SIZE];
    int read_len;
    int file_fd;
    int file_read_len;

    memset(&server_addr, 0, sizeof(server_addr)); // 0으로 초기화
    server_addr.sin_family = AF_INET; // AF_INET은 주소 체계에서 IPv4를 의미
    server_addr.sin_port = htons(4000); // htons 함수를 통해서 Little Endian일 경우 Big Endian으로 변경하여 포트 번호 대입
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY로 설정하여 모든 IP의 요청을 받음

    server_socket = socket(PF_INET, SOCK_STREAM, 0); // PF_INET은 프로토콜 체계에서 IPv4, SOCK_STREAM은 TCP를 의미

    if (-1 == server_socket) {
        printf("server socket 생성 실패\n");
        exit(1);
    }

    if (-1 == bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
        printf("bind() 실행 에러\n");
        exit(1);
    }

    if (-1 == listen(server_socket, 5)) { // backlog를 5로 설정하여 큐의 길이 할당
        printf("listen() 실행 실패\n");
        exit(1);
    }
    
    while (1) { // 클라이언트의 요청을 반복적으로 받기 위한 Outer 루프
                // Outer 루프가 없는 경우 1회만 통신이 됨
        memset(buff, 0, sizeof(buff)); // 0으로 초기화

        memset(&client_addr, 0, sizeof(client_addr)); // 0으로 초기화
        client_addr_size = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_addr_size); // 클라이언트의 요청을 받아들임 

        if (-1 == client_socket) { // accept 함수가 -1을 리턴한 경우 클라이언트와의 연결 실패
            printf("클라이언트 연결 수락 실패\n");
            close(server_socket);
            exit(1);
        }
 
        read_len=read(client_socket, buff, BUFF_SIZE); // 메세지 read
        if(read_len>0) {strcpy(file_name,buff);  printf("받은 메세지: %s\n", buff);}
        else {close(client_socket); break;}

        file_fd=open(file_name,O_WRONLY | O_CREAT, 0700);
        if(!read_len){printf("file create error!!!\n"); exit(1);}
        
        while(1){
                memset(buff,0x00,BUFF_SIZE);
                file_read_len=read(client_socket,buff,BUFF_SIZE);
                write(file_fd,buff,file_read_len);
                if(file_read_len == EOF | file_read_len == 0){
                   printf("finish file\n");
                   close(file_fd);
                   close(client_socket);
                   break;
                  }
                }
      }
close(server_socket);
}
