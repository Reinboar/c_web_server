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
HandlerResult defaultReqHandler(HttpRequest* req, HttpResponse* resp, HashTable* ht) {
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
    HashTable* ht = createHashTable(50);
    insertElement(ht, "ip", req->ip);
    while (curH != NULL) {
        printf("Request Handler: %s\n", curH->name);
        HandlerResult res = curH->func(req, resp, ht);
        if (res == REQ_STOP) {
            printf("Stopping request processing early\n");
            return resp;
        } else if (res == REQ_RESTART) {
            printf("Restarting request processing...\n");
            curH = root;
            continue;
        }
        curH = curH->next;
    }
    printf("All handlers finished\n");
    return resp;
}
