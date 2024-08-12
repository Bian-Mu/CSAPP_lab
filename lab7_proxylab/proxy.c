#include <stdio.h>
#include "csapp.h"

// bianmu: 23224
/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *connection_hdr = "connection: close\r\n";
static const char *pxy_connection_hdr = "proxy-connection: close\r\n";

// define functions
void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *host, char *port, char *path);
void sigpipe_handler(int sig);
void *thread(void *var);

int main(int argc, char **argv)
{
    int listenfd,*connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    Signal(SIGPIPE, sigpipe_handler);
    listenfd = Open_listenfd(argv[1]);
    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = malloc(sizeof(int));
        *connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);
        Pthread_create(&tid, NULL, thread, connfd);
    }

    return 0;
}

void sigpipe_handler(int sig)
{
    printf("sigpipe handled %d\n", sig);
    return;
}

void doit(int fd)
{
    char buf[MAXLINE], response_buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char host[MAXLINE], path[MAXLINE], port[MAXLINE];
    char cache[MAX_OBJECT_SIZE];
    rio_t rio, response_io;
    int response_fd;
    size_t n;

    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE))
        return;
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET"))
    {
        return;
    }
    if (!parse_uri(uri, host, port, path))
    {
        return;
    }

    // 请求有效，建立连接并请求指定对象
    response_fd = Open_clientfd(host, port);
    Rio_readinitb(&response_io, response_fd);

    sprintf(response_buf, "GET %s HTTP/1.0\r\nHost: %s\r\n%s%s%s\r\n", path, host, user_agent_hdr, connection_hdr, pxy_connection_hdr);
    Rio_writen(response_fd, response_buf, strlen(response_buf));

    while ((n = Rio_readlineb(&response_io, response_buf, MAXLINE)) != 0)
    {
        Rio_writen(fd, response_buf, n);
    }

    Close(response_fd);
}

int parse_uri(char *uri, char *host, char *port, char *path)
{
    const char *ptr;
    const char *tmp;
    char scheme[10];
    int len;
    int i;

    ptr = uri;

    tmp = strchr(ptr, ':');
    if (tmp == NULL)
        return 0;

    len = tmp - ptr;
    (void)strncpy(scheme, ptr, len);
    scheme[len] = '\0';
    for (i = 0; i < len; i++)
        scheme[i] = tolower(scheme[i]);
    if (strcasecmp(scheme, "http"))
        return 0;
    tmp++;
    ptr = tmp;

    for (i = 0; i < 2; i++)
    {
        if ('/' != *ptr)
        {
            return 0;
        }
        ptr++;
    }

    tmp = ptr;
    while ('\0' != *tmp)
    {
        if (':' == *tmp || '/' == *tmp)
            break;
        tmp++;
    }
    len = tmp - ptr;
    (void)strncpy(host, ptr, len);
    host[len] = '\0';

    ptr = tmp;

    if (':' == *ptr)
    {
        ptr++;
        tmp = ptr;

        while ('\0' != *tmp && '/' != *tmp)
            tmp++;
        len = tmp - ptr;
        (void)strncpy(port, ptr, len);
        port[len] = '\0';
        ptr = tmp;
    }
    else
    {
        port = "80";
    }

    if ('\0' == *ptr)
    {
        strcpy(path, "/");
        return 1;
    }

    tmp = ptr;
    while ('\0' != *tmp)
        tmp++;
    len = tmp - ptr;
    (void)strncpy(path, ptr, len);
    path[len] = '\0';

    return 1;
}

void *thread(void *vargp)
{
    int connfd = *((int *)vargp);
    Pthread_detach(Pthread_self());
    Free(vargp);
    doit(connfd);
    Close(connfd);
    return NULL;
}