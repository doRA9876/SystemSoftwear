#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  int sockfd, nbytes;
  char buf[BUFSIZ];
  char* mesg = "Hello, world!";
  char* portname = "echo";
  char* servername = "localhost";
  struct servent* serv;
  struct hostent* servhost;
  struct sockaddr_in servaddr;

  if (argc >= 2) {
    servername = argv[1]; /* 第1引数: サーバ名 */
    if (argc >= 3) {
      portname = argv[2]; /* 第2引数: ポート名 */
      if (argc >= 4) {
        mesg = argv[3]; /* 第3引数: 送信文字列 */
      }
    }
  }

  serv = getservbyname(portname, "tcp");
  if (serv == NULL) {
#if 0
    perror("getservbyname");
#endif
    serv = getservbyport(atoi(portname), "tcp");
    if (serv == NULL) {
      perror("getservbyname");
      exit(1);
    }
  }

  servhost = gethostbyname(servername);
  if (servhost == NULL) {
    in_addr_t addr;
    addr = inet_addr(servername);
    servhost = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
    if (servhost == NULL) {
      perror("gethostbyaddr");
      exit(1);
    }
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = serv->s_port;
  memcpy((char*)&servaddr.sin_addr, servhost->h_addr, servhost->h_length);
  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    perror("connect");
    exit(1);
  }

  /* nbytesの値のチェックは省略 (一度に送信できない場合もある) */
  nbytes = write(sockfd, mesg, strlen(mesg) + 1);
  nbytes = read(sockfd, buf, sizeof(buf) - 1);
  buf[nbytes] = '\0'; /* 念のため後ろにnull文字を追加 */
  puts(buf);          /* 改行付き出力 */
  close(sockfd);
  return 0;
}
