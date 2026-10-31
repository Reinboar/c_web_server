#ifndef HTTP_PROT_H
#define HTTP_PROT_H

#include <string.h>

typedef struct HttpRequestHeaders {
    char* host;
    char* contentType;
    char* contentLength;

} HttpRequestHeaders;

typedef struct HttpRequest {
    char* method;
    char* url;
    char* httpVersion;
    HttpRequestHeaders headers;
    char* body;
} HttpRequest;

HttpRequest* createHttpReq();
int destroyHttpReq(HttpRequest* req);
HttpRequest* readHttpReq(char** buffer);

#endif