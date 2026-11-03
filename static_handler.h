#ifndef STATIC_HANDLER_H
#define STATIC_HANDLER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "http_prot.h"
#include "req_handler.h"

#define WEB_DIR "./web"

char* buildFullPath(const char* basePath, const char* filename) {
    int pathLen = strlen(filename) + strlen(basePath);
    char* fullPath = calloc(1, pathLen + 1);
    memcpy(fullPath, "", 1);
    strcat(fullPath, basePath);
    strcat(fullPath, filename);
    return fullPath;
}

int readFileContent(char* filename, char* buffer) {
    char* fullPath = buildFullPath(WEB_DIR, filename);
    char* rdBuffer = calloc(1, 100);
    bzero(rdBuffer, 100);
    FILE* inFile = fopen(fullPath, "r");
    free(fullPath);
    while (feof(inFile) == 0) {
        int bytesNum = fread(rdBuffer, 1, 100, inFile);
        strncat(buffer, rdBuffer, bytesNum);
    }
    free(rdBuffer);
    fclose(inFile);
    return 0;
}

int getFileSize(char* filename) {
    char* fullPath = buildFullPath(WEB_DIR, filename);
    FILE* inFile = fopen(fullPath, "r");
    if (inFile == NULL) {
        printf("File not found: %s\n", fullPath);
        free(fullPath);
        return -1;
    }
    fseek(inFile, 0, SEEK_END);
    int fileSize = ftell(inFile);
    fclose(inFile);

    if (fileSize < 0) {
        printf("Error occured while reading the size of file: %s\n", fullPath);
        free(fullPath);
        return -1;
    }
    free(fullPath);
    return fileSize;
}

bool staticFileHandlerFunc(HttpRequest* req, HttpResponse* resp) {
    int fileSize = getFileSize(req->url);
    if (fileSize < 0) {
        resp->statusCode = 404;
        resp->headers.contentType = "text/html";
        const char* notFoundMsg = "<h1>File Not Found</h1>";
        resp->body = calloc(1,strlen(notFoundMsg));
        strcpy(resp->body, notFoundMsg);
        return false;
    } else {
        char* fileContent = calloc(1, fileSize);
        readFileContent(req->url, fileContent);
        resp->statusCode = 200;
        resp->headers.contentType = "text/html";
        resp->body = fileContent;
        return false;
    }
}

ReqHandler* staticFileHandler() {
    return newReqHandler("static file", staticFileHandlerFunc);
}

#endif