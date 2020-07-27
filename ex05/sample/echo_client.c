#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  int sockfd, nbytes;
  char send_buf[BUFSIZ];
  char buf[BUFSIZ];
  char* mesg = "Hello, world!";
  struct sockaddr_in servaddr;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(80); /* echo port is 7 */

  /* IP address of localhost is 127.0.0.1 */
  if (inet_pton(AF_INET, "150.65.205.18", &servaddr.sin_addr) < 0) {
    perror("inet_pton");
    exit(1);
  }

  printf("hello");
  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    perror("connect");
    exit(1);
  }

  /* nbytesの値のチェックは省略 (一度に送信できない場合もある) */
  // nbytes = write(sockfd, mesg, strlen(mesg) + 1);
  // nbytes = read(sockfd, buf, sizeof(buf) - 1);
  // buf[nbytes] = '\0'; /* 念のため後ろにnull文字を追加 */
  // puts(buf);          /* 改行付き出力 */

  /* HTTP プロトコル生成 & サーバに送信 */
  sprintf(send_buf, "GET %s HTTP/1.0\r\n", "/");
  write(sockfd, send_buf, strlen(send_buf));

  sprintf(send_buf, "Host: %s:%d\r\n", "150.65.205.18", 80);
  write(sockfd, send_buf, strlen(send_buf));

  sprintf(send_buf, "\r\n");
  write(sockfd, send_buf, strlen(send_buf));

  while (1) {
    int read_size;
    read_size = read(sockfd, buf, BUFSIZ);
    if (read_size > 0) {
      write(1, buf, read_size);
    } else {
      break;
    }
  }

  close(sockfd);
  return 0;
}
