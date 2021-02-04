/* $Id: http-client.c,v 1.6 2013/01/23 06:57:19 68user Exp $ */

#include <dirent.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

typedef struct HeadderInfo {
  char info[256];
  struct HeadderInfo* next;
} headderInfo_t;

void send_request(int socket_fd, char path[], char host[], int port);
headderInfo_t* store_headderInfo(char* buf);

int main(int argc, char* argv[]) {
  int sockfd; /* ソケットのためのファイルディスクリプタ */
  struct hostent* servhost; /* ホスト名と IP アドレスを扱うための構造体 */
  struct sockaddr_in server; /* ソケットを扱うための構造体 */
  struct servent* service; /* サービス (http など) を扱うための構造体 */

  char host[BUFSIZ] = "localhost"; /* 接続するホスト名 */
  char path[BUFSIZ] = "/";         /* 要求するパス */
  unsigned short port = 80;        /* 接続するポート番号 */

  if (argc > 1) { /* URLが指定されていたら */
    char host_path[BUFSIZ];

    if (strlen(argv[1]) > BUFSIZ - 1) {
      fprintf(stderr, "URL が長すぎます。\n");
      return 1;
    }
    /* http:// から始まる文字列で */
    /* sscanf が成功して */
    /* http:// の後に何か文字列が存在するなら */
    if (strstr(argv[1], "http://") && sscanf(argv[1], "http://%s", host_path) &&
        strcmp(argv[1], "http://")) {
      char* p;

      p = strchr(host_path, '/'); /* ホストとパスの区切り "/" を調べる */
      if (p != NULL) {
        strcpy(path, p); /* "/"以降の文字列を path にコピー */
        *p = '\0';
        strcpy(host, host_path); /* "/"より前の文字列を host にコピー */
      } else { /* "/"がないなら＝http://host という引数なら */
        strcpy(host, host_path); /* 文字列全体を host にコピー */
      }

      p = strchr(host, ':'); /* ホスト名の部分に ":" が含まれていたら */
      if (p != NULL) {
        port = atoi(p + 1); /* ポート番号を取得 */
        if (port <= 0) { /* 数字でない (atoi が失敗) か、0 だったら */
          port = 80;     /* ポート番号は 80 に決め打ち */
        }
        *p = '\0';
      }
    } else {
      fprintf(stderr, "URL は http://host/path の形式で指定してください。\n");
      return 1;
    }
  }

  printf("http://%s %s %dを取得します。\n\n", host, path, port);

  /* ホストの情報(IPアドレスなど)を取得 */
  servhost = gethostbyname(host);
  if (servhost == NULL) {
    fprintf(stderr, "[%s] から IP アドレスへの変換に失敗しました。\n", host);
    return 0;
  }

  bzero(&server, sizeof(server)); /* 構造体をゼロクリア */

  server.sin_family = AF_INET;

  /* IPアドレスを示す構造体をコピー */
  bcopy(servhost->h_addr, &server.sin_addr, servhost->h_length);

  if (port != 0) { /* 引数でポート番号が指定されていたら */
    server.sin_port = htons(port);
  } else { /* そうでないなら getservbyname でポート番号を取得 */
    service = getservbyname("http", "tcp");
    if (service != NULL) { /* 成功したらポート番号をコピー */
      server.sin_port = service->s_port;
    } else { /* 失敗したら 80 番に決め打ち */
      server.sin_port = htons(80);
    }
  }
  /* ソケット生成 */
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "ソケットの生成に失敗しました。\n");
    return 1;
  }
  /* サーバに接続 */
  if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
    fprintf(stderr, "connect に失敗しました。\n");
    return 1;
  }

  send_request(sockfd, path, host, port);

  char file_name[BUFSIZ];
  if (*(path + 1) == '\0') {
    strncpy(file_name, "index.html", BUFSIZ);
  } else {
    strncpy(file_name, path + 1, BUFSIZ);
  }
  int ofd = open(file_name, (O_WRONLY | O_CREAT | O_TRUNC), 0644);

  /* Headder processing */
  char buf[BUFSIZ];
  int read_size;
  read_size = read(sockfd, buf, BUFSIZ);
  char* p2 = strstr(buf, "\r\n\r\n");
  p2 += 4;
  read_size = read_size - ((p2 - buf) / sizeof(char));
  headderInfo_t* headder = store_headderInfo(buf);
  
  headderInfo_t* h = headder;
  while (h != NULL) {
    printf("%s\n", h->info);
    h = h->next;
  }

  if (read_size > 0) {
    write(ofd, p2, read_size);
    while (1) {
      read_size = read(sockfd, buf, BUFSIZ);
      if (read_size > 0) {
        write(ofd, buf, read_size);
      } else {
        break;
      }
    }
  }

  close(sockfd);

  return 0;
}

void send_request(int socket_fd, char path[], char host[], int port) {
  char send_buf[BUFSIZ];

  sprintf(send_buf, "GET %s HTTP/1.0\r\n", path);
  write(socket_fd, send_buf, strlen(send_buf));

  sprintf(send_buf, "Host: %s:%d\r\n", host, port);
  write(socket_fd, send_buf, strlen(send_buf));

  sprintf(send_buf, "\r\n");
  write(socket_fd, send_buf, strlen(send_buf));
}

headderInfo_t* store_headderInfo(char* buf) {
  char copy[BUFSIZ];
  strncpy(copy, buf, BUFSIZ);
  char* tail = strstr(copy, "\r\n\r\n");
  char* p = copy;
  headderInfo_t* HEAD = NULL;
  headderInfo_t* h = HEAD;

  while (p < tail) {
    char* q = p;
    while (*(++p) != '\n')
      ;
    *p = '\0';
    headderInfo_t* tmp = (headderInfo_t*)malloc(sizeof(headderInfo_t));
    if (tmp == NULL) {
      fprintf(stderr, "error: could not allocate memory.\n");
      return NULL;
    }
    strncpy(tmp->info, q, 256);
    tmp->next = NULL;

    if (h == NULL) {
      HEAD = h = tmp;
    } else {
      h->next = tmp;
      h = tmp;
    }
    p++;
  }

  return HEAD;
}