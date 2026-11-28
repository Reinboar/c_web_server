#ifndef ROUTE_HANDLER_H
#define ROUTE_HANDLER_H

#include "req_handler.h"

typedef struct RouteEntry {
    const char* url;
    const char* method;
    ReqHandlerFun f;
} RouteEntry;

bool addGetRoute(const char* url, ReqHandlerFun f);
bool addPostRoute(const char* url, ReqHandlerFun f);

ReqHandler* routeHandler();

#endif