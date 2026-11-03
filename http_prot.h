#ifndef HTTP_PROT_H
#define HTTP_PROT_H

#include <string.h>
#include <stdbool.h>

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

typedef struct HttpResponseHeaders {
    const char* wwwAuthenticate;
    const char* connection;
    int contentLength;
    const char* contentType;
} HttpResponseHeaders;

typedef struct HttpResponse {
    int statusCode;
    const char* statusMsg;
    HttpResponseHeaders headers;
    char* body;

} HttpResponse;

// Allocates a new HttpResponse structure
HttpResponse* newHttpResp();

// Deallocates a HttpResponse structure
bool destroyHttpResp(HttpResponse*);

// Takes in a HttpResponse structure and compiles its fields into a raw string that can be sent to a client
char* compileHttpResp(HttpResponse*);

// Allocates a new HttpRequest structure
HttpRequest* newHttpReq();

// Deallocates a HttpRequest structure
bool destroyHttpReq(HttpRequest*);

// Parses a raw HTTP request string and populates the necessary records in a HttpRequest structure
HttpRequest* readHttpReq(char**);

#endif