
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define INFO_SIZE 1024

typedef struct HeadderInfo {
  char info[INFO_SIZE];
  struct HeadderInfo* next;
} headderInfo_t;

void http(int sockfd);
int send_msg(int fd, char* msg);
void parth_uri2host(char* uri, char* host, char* path, int* port);
headderInfo_t* client(char* host, char* path, int port);
void send_request(int socket_fd, char path[], char host[], int port);
headderInfo_t* store_headderInfo(char* buf);
void send_httpHeadder(int fd, headderInfo_t* head);

int main(void) {
  int listenfd, connfd, nbytes;
  char buf[BUFSIZ];
  struct sockaddr_in servaddr;

  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(10000);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(listenfd, 5) < 0) {
    perror("listen");
    exit(1);
  }

  while (1) {
    if ((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) < 0) {
      perror("accept");
      exit(1);
    }
    http(connfd);
    close(connfd);
  }
}

void http(int sockfd) {
  char buf[BUFSIZ];
  char method[16];
  char uri_addr[256];
  char host[256] = "localhost";
  char path[256] = "/";
  int port = 80;
  char http_ver[64];

  if (read(sockfd, buf, BUFSIZ) <= 0) {
    fprintf(stderr, "error: reading a request.\n");
  } else {
    sscanf(buf, "%s %s %s", method, uri_addr, http_ver);

    parth_uri2host(uri_addr, host, path, &port);

    printf("method:%s http version:%s host:%s path:%s port:%d\n", method,
           http_ver, host, path, port);
  }

  headderInfo_t* head = NULL;
  if ((head = client(host, path, port)) == NULL) {
    return;
  }

  int read_fd = open(path + 1, O_RDONLY, 0666);
  if (read_fd == -1) {
    send_msg(sockfd, "HTTP/1.0 404 Not Found");
    close(read_fd);
    return;
  }

  send_httpHeadder(sockfd, head);

  int length = 0;
  while ((length = read(read_fd, buf, BUFSIZ)) > 0) {
    if (write(sockfd, buf, length) != length) {
      fprintf(stderr, "error: writing a response.\n");
      break;
    }
  }

  close(read_fd);
}

int send_msg(int fd, char* msg) {
  int length;
  length = strlen(msg);

  if (write(fd, msg, length) != length) {
    fprintf(stderr, "error: writing.\n");
  }

  return length;
}

void parth_uri2host(char* uri, char* host, char* path, int* port) {
  char* p;

  p = strchr(++uri, '/'); 
  if (p != NULL) {
    strcpy(path, p); 
    *p = '\0';
    strcpy(host, uri); 
  } else { 
    strcpy(host, uri); 
  }

  if (*(path + 1) == '\0') {
    strcpy(path + 1, "index.html");
  }

  p = strchr(host, ':'); 
  if (p != NULL) {
    *port = atoi(p + 1); 
    if (*port <= 0) { 
      *port = 80;     
    }
    *p = '\0';
  }
}

headderInfo_t* client(char* host, char* path, int port) {
  int sockfd;
  struct hostent* servhost;
  struct sockaddr_in server;

  // printf("client side: host:%s path:%s port:%d\n", host, path, port);

  servhost = gethostbyname(host);
  if (servhost == NULL) {
    fprintf(stderr, "[%s] から IP アドレスへの変換に失敗しました。\n", host);
    return 0;
  }

  bzero(&server, sizeof(server));

  server.sin_family = AF_INET;
  if (port != 0) {
    server.sin_port = htons(port);
  } else {
    server.sin_port = htons(80);
  }

  bcopy(servhost->h_addr, &server.sin_addr, servhost->h_length);

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "ソケットの生成に失敗しました。\n");
    return NULL;
  }
  if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
    fprintf(stderr, "connect に失敗しました。\n");
    return NULL;
  }

  printf("http://%s %s %dを取得します。\n\n", host, path, port);
  send_request(sockfd, path, host, port);

  int ofd = open(path + 1, (O_WRONLY | O_CREAT | O_TRUNC), 0644);

  /* Headder processing */
  char buf[BUFSIZ];
  int read_size;
  read_size = read(sockfd, buf, BUFSIZ);
  char* p2 = strstr(buf, "\r\n\r\n");
  p2 += 4;
  read_size = read_size - ((p2 - buf) / sizeof(char));
  headderInfo_t* head = store_headderInfo(buf);

  headderInfo_t* h = head;
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
  return head;
}

void send_request(int socket_fd, char path[], char host[], int port) {
  char send_buf[BUFSIZ];

  sprintf(send_buf, "GET %s HTTP/1.0\r\n", path);
  printf("%s\n", send_buf);
  write(socket_fd, send_buf, strlen(send_buf));

  sprintf(send_buf, "Host: %s:%d\r\n", host, port);
  printf("%s\n", send_buf);
  write(socket_fd, send_buf, strlen(send_buf));

  sprintf(send_buf, "\r\n");
  printf("%s\n", send_buf);
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
    while (*p != '\n' && p < tail)
      p++;
    *p = '\0';
    headderInfo_t* tmp = (headderInfo_t*)malloc(sizeof(headderInfo_t));
    if (tmp == NULL) {
      fprintf(stderr, "error: could not allocate memory.\n");
      return NULL;
    }
    strncpy(tmp->info, q, INFO_SIZE);
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

void send_httpHeadder(int fd, headderInfo_t* head) {
  headderInfo_t* h = head;
  while (h != NULL) {
    send_msg(fd, h->info);
    send_msg(fd, "\r\n");
    h = h->next;
  }
  send_msg(fd, "\r\n");
}