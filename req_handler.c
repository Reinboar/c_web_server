#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

#include "http_prot.h"
#include "req_handler.h"

ReqHandler* root = NULL;
ReqHandler* tail = NULL;

// The default request handler function does not modify the request or response
// it simply returns true to signal that the request should continue to be processed,
// if there are any remaining request handlers.
bool defaultReqHandler(HttpRequest* req, HttpResponse* resp) {
    resp->statusCode = 200;
    const char* defaultMsg = "<h1>IT WORKS</h1>";
    resp->body = calloc(1, strlen(defaultMsg));
    resp->body = strcpy(resp->body, defaultMsg);
    resp->headers.contentType = "text/html";
    return true;
}

bool initReqHandlers() {
    root = calloc(1, sizeof(ReqHandler));
    root->func = defaultReqHandler;
    root->next = NULL;
    root->name = "default";
    tail = root;
}

ReqHandler* newReqHandler(char* name, ReqHandlerFun func) {
    ReqHandler* newH = calloc(1, sizeof(ReqHandler));
    newH->name = name;
    newH->func = func;
    newH->next = NULL;
    return newH;
}

void prependReqHandler(ReqHandler* reqH) {
    reqH->next = root;
    root = reqH;
}

void appendReqHandler(ReqHandler* reqH) {
    tail->next = reqH;
    tail = reqH;
}

bool insertReqHandler(char* name, ReqHandler* reqH) {
    ReqHandler* curH = root;
    while (curH != NULL) {
        if (strcmp(name, curH->name) == 0) {
            reqH->next = curH->next;

            // update the tail pointer if the selected handler is the end of the list
            if (reqH->next == NULL)
                tail = reqH;

            curH->next = reqH;
            return true;
        }
        curH = curH->next;
    }
    return false;
}

HttpResponse* processRequest(HttpRequest* req) {
    HttpResponse* resp = newHttpResp();
    ReqHandler* curH = root;
    while (curH != NULL) {
        printf("Request Handler: %s\n", curH->name);
        if (!curH->func(req, resp)) {
            printf("Stopping request processing early\n");
            return resp;
        }
        curH = curH->next;
    }
    printf("All handlers finished\n");
    return resp;
}
