#ifndef HTTP_PROT_H
#define HTTP_PROT_H

#include <string.h>

typedef struct HttpRequestHeaders {
    char* host;
    char* authorization;
} HttpRequestHeaders;

typedef struct HttpRequest {
    char* method;
    char* url;
    char* httpVersion;
    HttpRequestHeaders headers;
    char* body;
} HttpRequest;

typedef 

HttpRequest* createHttpReq();
int destroyHttpReq(HttpRequest* req);
HttpRequest* readHttpReq(char** buffer);

#endif