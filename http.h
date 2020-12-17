#ifndef __HTTP__H_
#define __HTTP__H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

#define EQ(a, b) (strcmp(a, b) == 0)
#define BUFSIZE 512

void panic(const char* str) {
    fprintf(stderr, "line %d: %s\n", __LINE__, str);
    exit(-1);
}

void panic_if(boolean cond, const char* str) {
    if (cond) panic(str);
}

//get a line ended by '\r\n'
void gets_sep(const char* in, char* out, const char* sep) {
    const char* end = strstr(in, sep);
    while (in != end) *out++ = *in++;
}
void getline(const char* in, char* out) {
    return gets_sep(in, out, "\r\n");
}

// HTTP REQUEST HEADER
//---------------------------
// GET %s HTTP/1.1\r\n
// Host: %s\r\n
// User-Agent: IE or Chrome\r\n
// Accept-Type: */*\r\n
// Connection: Close\r\n
// \r\n
//---------------------------

const char*
getAddrByHost(const char* host) {
    struct hostent* p = gethostbyname(host);
    panic_if(p == NULL, "gethostbyname failed to resolve");
    return inet_ntoa(*(struct in_addr*)p->h_addr_list[0]);
}

#endif
