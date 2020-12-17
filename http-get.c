/**
 * 使用C语言实现简易的HTTP中GET方法
 * 仅仅工作在win下面
 * 
 * by dreamer2q
**/

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vendor/sds/sds.h"
#include "http.h"

#pragma execution_character_set("utf-8")
#pragma comment(lib, "ws2_32")

enum Method {
    GET,
    POST,
};

typedef struct Header {
    const char* name;
    const char* value;
} Header;

typedef struct HttpHeader {
    struct Header* headers;
    int len;
    int cap;
} HttpHeader;

HttpHeader
makeHttpHeader(int cap) {
    HttpHeader header;
    header.headers = malloc(cap * sizeof(struct Header));
    header.cap = cap;
    header.len = 0;
    return header;
}

//currently, dynamic alloc not implemented
//so, make sure not to overflow
HttpHeader
headerAppend(HttpHeader header, const Header item) {
    header.headers[header.len] = item;
    header.len++;
    return header;
}

const char*
headerGET(HttpHeader header, const char* name) {
    const char* ret = NULL;
    for (int i = 0; i < header.len; i++) {
        if (EQ(header.headers[i].name, name)) {
            ret = header.headers[i].value;
            break;
        }
    }
    return ret;
}

int writeHeader(HttpHeader header, char* buf) {
    for (int i = 0; i < header.len; i++) {
        Header* h = header.headers + i;
        sprintf(buf, "%s: %s\r\n", h->name, h->value);
    }
    return header.len;
}

typedef struct HttpUrl {
    const char* url;
    const char* scheme;
    const char* host;
    const char* path;
} Url;

Url parseUrl(const char* url) {
    Url ret = {NULL};
    ret.url = url;
    const char* sc = strstr(url, "//");
    panic_if(sc == NULL, "invalid url scheme");
    int schemelen = sc - url - 1;
    char* scheme = (char*)malloc(sizeof(char) * schemelen);
    strncpy(scheme, url, schemelen);
    scheme[schemelen] = '\0';
    ret.scheme = scheme;
    sc += 2;
    char* sp = strchr(sc, '/');
    panic_if(sp == NULL, "url must contains /");
    int hostlen = sp - sc + 1;
    int pathlen = strlen(sp) + 1;
    char* host = malloc(sizeof(char) * hostlen);
    char* path = malloc(sizeof(char) * pathlen);
    strncpy(host, sc, hostlen);
    strncpy(path, sp, pathlen);
    host[hostlen - 1] = '\0';
    path[pathlen - 1] = '\0';
    ret.host = host;
    ret.path = path;
    return ret;
}

typedef struct HttpRequest {
    enum Method method;
    Url url;
    HttpHeader headers;
    char* body;
} HttpRequest;

HttpRequest
makeRequest(const char* url) {
    HttpRequest req;
    req.url = parseUrl(url);
    req.headers = makeHttpHeader(10);
    Header headers[] = {
        {"Host", req.url.host},
        {"User-Agent", "C-http-client-demo"},
        {"Accept-Type", "*/*"},
        {"Connection", "Close"},
    };
    for (int i = 0; i < sizeof(headers) / sizeof(headers[0]); i++) {
        req.headers = headerAppend(req.headers, headers[i]);
    }
    return req;
}

typedef struct HttpResponse {
    int status_code;
    const char* status;
    HttpHeader header;
    const char* body;
} HttpResponse;

typedef struct HttpClient {
    struct HttpRequest request;
    struct HttpResponse response;
} HttpClient;

HttpClient
newHttpClient(const char* url) {
    HttpClient client;
    client.request = makeRequest(url);
    return client;
}

HttpResponse
clientDo(HttpRequest req, enum Method method) {
    HttpResponse resp;

    SOCKET fd = socket(PF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN addr;
    int i;
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = 0;
    addr.sin_port = htons(0);
    i = bind(fd, (const struct sockaddr*)&addr, sizeof(SOCKADDR_IN));
    const char* hostaddr = getAddrByHost(req.url.host);
    addr.sin_addr.S_un.S_addr = inet_addr(hostaddr);
    addr.sin_port = htons(80);
    i = connect(fd, (struct sockaddr*)&addr, sizeof(SOCKADDR_IN));
    panic_if(i != NOERROR, "failed to connect to host");

    char reqbuf[2048] = {0};
    int wlen = sprintf(reqbuf, "GET %s HTTP/1.1\r\n", req.url.path);
    writeHeader(req.headers, reqbuf + wlen);
    strcat(reqbuf, "\r\n");
    int reqlen = strlen(reqbuf);
    int sendlen = send(fd, reqbuf, reqlen, 0);
    panic_if(sendlen != reqlen, "send len error");

    //receive response
    char recvheader[4 * BUFSIZE] = {0};
    int recvlen;
    recvlen = recv(fd, recvheader, sizeof(recvheader), 0);
    recvheader[recvlen] = '\0';

    char resp_ver[BUFSIZE] = {0}, resp_status[BUFSIZE] = {0};
    sscanf(recvheader, "%s %d %s", resp_ver, &resp.status_code, resp_status);
    resp.status = sdsnew(resp_status);

    resp.header = makeHttpHeader(48);  //make it large
    const char* pheader = strstr(recvheader, "\r\n") + 2;
    while (strncmp(pheader, "\r\n", 2) != 0) {
        char bufname[BUFSIZE] = {0};
        char bufvalue[4 * BUFSIZE] = {0};
        gets_sep(pheader, bufname, ": ");
        const char* pvalue = strstr(pheader, ": ") + 2;
        getline(pvalue, bufvalue);
        Header item = {sdsnew(bufname), sdsnew(bufvalue)};
        resp.header = headerAppend(resp.header, item);
        pheader = strstr(pheader, "\r\n") + 2;
    }

    sds body = sdsnew(strstr(recvheader, "\r\n\r\n") + 4);
    char recvbuf[BUFSIZE];
    while ((recvlen = recv(fd, recvbuf, BUFSIZE, 0)) > 0) {
        body = sdscatlen(body, recvbuf, recvlen);
    }
    resp.body = body;
    return resp;
}

int httpGet(const char* url) {
    HttpClient client = newHttpClient(url);
    client.response = clientDo(client.request, GET);
    printf("%s\n", client.response.body);
}

void init() {
    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0) {
        fprintf(stderr, "init wsa error");
        exit(-1);
    }
    system("@echo off");
    system("chcp 65001");
}
void postinit() {
    WSACleanup();
    system("@echo on");
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        panic("please specify http link");
        printf("test string");
    }
    init();
    httpGet(argv[1]);
    postinit();
}
