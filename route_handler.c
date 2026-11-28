#include <string.h>
#include <stdlib.h>

#include "req_handler.h"
#include "route_handler.h"

static HashTable* routeTable = NULL;

bool addGetRoute(const char* url, ReqHandlerFun f) {
    RouteEntry* entry = calloc(1, sizeof(RouteEntry));
    entry->url = url;
    entry->method = "GET";
    entry->f = f;
    insertElement(routeTable, url, entry);
    return true;
}

bool addPostRoute(const char* url, ReqHandlerFun f) {
    RouteEntry* entry = calloc(1, sizeof(RouteEntry));
    entry->url = url;
    entry->method = "POST";
    entry->f = f;
    insertElement(routeTable, url, entry);
    return true;
}

HandlerResult routeHandlerFunc(HttpRequest* req, HttpResponse* resp, HashTable* ht) {
    RouteEntry* entry = getElement(routeTable, req->url);
    if (entry == NULL)
        return REQ_CONTINUE;
    
    if (strcmp(entry->method, req->method) == 0) {
        return entry->f(req, resp, ht);
    }
    return REQ_CONTINUE;
}

ReqHandler* routeHandler() {
    routeTable = createHashTable(20);
    return newReqHandler("route handler", routeHandlerFunc);
}