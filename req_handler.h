#ifndef REQ_HANDLER_H
#define REQ_HANDLER_H

#include <stdbool.h>

#include "http_prot.h"

// function signature for the request handlers
typedef bool (*ReqHandlerFun)(HttpRequest*, HttpResponse*);

typedef struct ReqHandler {
    ReqHandlerFun func;
    struct ReqHandler* next;
    char* name;
} ReqHandler;

// Adds a new ReqHandler to the beginning of the linked list
void prependReqHandler(ReqHandler*);

// Adds a new ReqHandler to the end of the linked list
void appendReqHandler(ReqHandler*);

// Inserts a new ReqHandler AFTER an existing ReqHandler in the linked list with a name that matches the given string
// Returns true if successful, false if the given name does not match an existing ReqHandler
bool insertReqHandler(char*, ReqHandler*);

// Allocates a new ReqHandler and initializes its name and function
ReqHandler* newReqHandler(char*, ReqHandlerFun);

// Creates a linked list of ReqHandlers
// Returns true if successful, false if the linked list could not be created
// Note: this must be called before ReqHandlers can be added to the list
bool initReqHandlers();

HttpResponse* processRequest(HttpRequest* req);

#endif