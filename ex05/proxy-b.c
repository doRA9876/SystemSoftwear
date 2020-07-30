
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define INFO_SIZE 1024
#define FD_NUM 10

typedef struct HeadderInfo {
  char info[INFO_SIZE];
  struct HeadderInfo* next;
} headderInfo_t;

typedef struct Instance {
  int c2p_fd;
  int p2s_fd;
  headderInfo_t* head;
  char host[256];
  char path[256];
  int port;
} instance_t;

int create_instance(int sockfd, instance_t* inst);
int connect_server(char* host, char* path, int port);
int send_msg(int fd, char* msg);
void parse_uri2host(char* uri, char* host, char* path, int* port);
void send_request(int socket_fd, char path[], char host[], int port);
headderInfo_t* store_headderInfo(char* buf);
void send_httpHeadder(int fd, headderInfo_t* head);
void send_data(instance_t* inst);
void recive_data(instance_t* inst);

int main(void) {
  int listen_fd, nbytes;
  int client2proxy_fd[FD_NUM];
  int proxy2server_fd[FD_NUM];
  instance_t connect_instance[FD_NUM];
  char buf[BUFSIZ];
  struct sockaddr_in servaddr;

  for (int i = 0; i < FD_NUM; i++) {
    client2proxy_fd[i] = -1;
    proxy2server_fd[i] = -1;
  }

  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(10000);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(listen_fd, 5) < 0) {
    perror("listen");
    exit(1);
  }

  fd_set read_fds;
  while (1) {
    struct timeval tv;
    FD_ZERO(&read_fds);
    FD_SET(listen_fd, &read_fds);
    int maxfd = listen_fd;

    for (int i = 0; i < FD_NUM; i++) {
      if (client2proxy_fd[i] != -1) {
        FD_SET(client2proxy_fd[i], &read_fds);
        if (client2proxy_fd[i] > maxfd)
          maxfd = client2proxy_fd[i];
      }
      if (proxy2server_fd[i] != -1) {
        FD_SET(proxy2server_fd[i], &read_fds);
        if (proxy2server_fd[i] > maxfd)
          maxfd = proxy2server_fd[i];
      }
    }

    tv.tv_sec = 10;
    tv.tv_usec = 500000;

    int connect = select(maxfd + 1, &read_fds, NULL, NULL, &tv);
    if (connect < 0) {
      if (errno == EINTR)
        continue;
    } else if (connect == 0) {
      continue;
    } else {
      if (FD_ISSET(listen_fd, &read_fds)) {
        for (int i = 0; i < FD_NUM; i++) {
          if (client2proxy_fd[i] == -1) {
            if ((client2proxy_fd[i] =
                     accept(listen_fd, (struct sockaddr*)NULL, NULL)) < 0) {
              perror("accept");
              exit(1);
            }
            break;
          }
        }
      }

      for (int i = 0; i < FD_NUM; i++) {
        if (FD_ISSET(client2proxy_fd[i], &read_fds)) {
          proxy2server_fd[i] = create_instance(client2proxy_fd[i], connect_instance + i);
          connect_instance[i].c2p_fd = client2proxy_fd[i];
          connect_instance[i].p2s_fd = proxy2server_fd[i];
          client2proxy_fd[i] = -1;
        }
      }

      for (int i = 0; i < FD_NUM; i++) {
        if (FD_ISSET(proxy2server_fd[i], &read_fds)) {
          recive_data(connect_instance + i);
          close(proxy2server_fd[i]);
          send_data(connect_instance + i);
          close(client2proxy_fd[i]);
          proxy2server_fd[i] = -1;
        }
      }
    }
  }

  for (int i = 0; i < FD_NUM; i++) {
    close(client2proxy_fd[i]);
  }

  close(listen_fd);
  return 0;
}

/**
 * @brief Create a connection instance
 * 
 * @param client2proxy_fd file descriptor between client to server
 * @param inst instance structure
 * @return [int] file descriptor from proxy to server
 */
int create_instance(int client2proxy_fd, instance_t* inst) {
  char buf[BUFSIZ];
  char method[16];
  char uri_addr[256];
  char http_ver[64];

  strncpy(inst->host, "localhost", 256);
  strncpy(inst->path, "/", 256);
  inst->port = 80;

  if (read(client2proxy_fd, buf, BUFSIZ) <= 0) {
    fprintf(stderr, "error: reading a request.\n");
    return -1;
  } else {
    sscanf(buf, "%s %s %s", method, uri_addr, http_ver);

    parse_uri2host(uri_addr, inst->host, inst->path, &inst->port);

    printf("method:%s http version:%s host:%s path:%s port:%d\n", method,
           http_ver, inst->host, inst->path, inst->port);
  }

  int proxy2server_fd = -1;
  if ((proxy2server_fd = connect_server(inst->host, inst->path, inst->port)) == -1) {
    return -1;
  }
  inst->p2s_fd = proxy2server_fd;
  return proxy2server_fd;
}

/**
 * @brief send message using file descriptor
 * 
 * @param fd file descriptor 
 * @param msg message to send
 * @return [int] length of message sent
 */
int send_msg(int fd, char* msg) {
  int length;
  length = strlen(msg);

  if (write(fd, msg, length) != length) {
    fprintf(stderr, "error: writing.\n");
  }

  return length;
}

/**
 * @brief connect to server using host, path and port number.
 * 
 * @param host destination host name 
 * @param path file path
 * @param port port number
 * @return [int] file descriptor between proxy to server
 */
int connect_server(char* host, char* path, int port) {
  int sockfd;
  struct hostent* servhost;
  struct sockaddr_in server;

  servhost = gethostbyname(host);
  if (servhost == NULL) {
    fprintf(stderr, "Failed to convert [%s] to IP address.\n", host);
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
    fprintf(stderr, "Failed to generate a socket.\n");
    return -1;
  }
  if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
    fprintf(stderr, "Failed to connect.\n");
    return -1;
  }

  printf("Gets http://%s %s %d.\n\n", host, path, port);
  send_request(sockfd, path, host, port);

  return sockfd;
}

/**
 * @brief convert URI to host, path and port
 * 
 * @param uri destination URI 
 * @param host destination host name 
 * @param path file path 
 * @param port port number
 */
void parse_uri2host(char* uri, char* host, char* path, int* port) {
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

/**
 * @brief send a request to server
 * 
 * @param fd file descriptor between proxy to server 
 * @param path file path
 * @param host destination host 
 * @param port port number
 */
void send_request(int fd, char path[], char host[], int port) {
  char send_buf[BUFSIZ];

  sprintf(send_buf, "GET %s HTTP/1.0\r\n", path);
  printf("%s\n", send_buf);
  write(fd, send_buf, strlen(send_buf));

  sprintf(send_buf, "Host: %s:%d\r\n", host, port);
  printf("%s\n", send_buf);
  write(fd, send_buf, strlen(send_buf));

  sprintf(send_buf, "\r\n");
  printf("%s\n", send_buf);
  write(fd, send_buf, strlen(send_buf));
}

/**
 * @brief store packet hedder
 * 
 * @param buf read buffer
 * @return [headderInfo_t*] created list head  
 */
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

/**
 * @brief send HTTP headder to client
 * 
 * @param fd file descriptor 
 * @param head list head
 */
void send_httpHeadder(int fd, headderInfo_t* head) {
  headderInfo_t* h = head;
  while (h != NULL) {
    send_msg(fd, h->info);
    send_msg(fd, "\r\n");
    h = h->next;
  }
  send_msg(fd, "\r\n");
}

/**
 * @brief recive data from server 
 * 
 * @param inst a instance structure 
 */
void recive_data(instance_t* inst) {
  char buf[BUFSIZ];
  int ofd = open(inst->path + 1, (O_WRONLY | O_CREAT | O_TRUNC), 0644);

  int read_size;
  read_size = read(inst->p2s_fd, buf, BUFSIZ);
  char* p2 = strstr(buf, "\r\n\r\n");
  p2 += 4;
  read_size = read_size - ((p2 - buf) / sizeof(char));
  inst->head = store_headderInfo(buf);

  headderInfo_t* h = inst->head;
  while (h != NULL) {
    printf("%s\n", h->info);
    h = h->next;
  }

  if (read_size > 0) {
    write(ofd, p2, read_size);
    while (1) {
      read_size = read(inst->p2s_fd, buf, BUFSIZ);
      if (read_size > 0) {
        write(ofd, buf, read_size);
      } else {
        break;
      }
    }
  }
}

/**
 * @brief send data to client
 * 
 * @param inst instance structure 
 */
void send_data(instance_t* inst) {
  char buf[BUFSIZ];
  send_httpHeadder(inst->c2p_fd, inst->head);

  int read_fd = open(inst->path + 1, O_RDONLY, 0666);
  if (read_fd == -1) {
    send_msg(inst->c2p_fd, "HTTP/1.0 404 Not Found");
    close(read_fd);
    return;
  }

  int length = 0;
  while ((length = read(read_fd, buf, BUFSIZ)) > 0) {
    if (write(inst->c2p_fd, buf, length) != length) {
      fprintf(stderr, "error: writing a response.\n");
      break;
    }
  }

  close(read_fd);
}