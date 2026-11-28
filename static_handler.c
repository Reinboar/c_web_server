#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "http_prot.h"
#include "req_handler.h"

char* webDir = "./web";

inline void setWebDir(char* newDir) {
    webDir = newDir;
}

char* buildFullPath(const char* basePath, const char* filename) {
    int pathLen = 0;
    bool leadingSlash = true;
    if (filename[0] != '/') {
        leadingSlash = false;
        pathLen = strlen(filename) + strlen(basePath);
    } else {
        pathLen = strlen(filename) + strlen(basePath);
    }
    char* fullPath = calloc(1, pathLen + 1);
    memcpy(fullPath, "", 1);
    strcat(fullPath, basePath);
    if (!leadingSlash)
        strcat(fullPath, "/");
    strcat(fullPath, filename);
    return fullPath;
}

const char* getMimeType(char* filename) {
    char* fileExt = strchr(filename, '.')+1;
    if (strcmp(fileExt, "html") == 0) {
        return "text/html";
    } else if (strcmp(fileExt, "ico") == 0) {
        return "image/x-icon";
    } else if (strcmp(fileExt, "jpg") == 0) {
        return "image/jpg";
    } else if (strcmp(fileExt, "png") == 0) {
        return "image/png";
    } else {
        return "";
    }
}

int readFileContentRaw(char* filename, size_t size, char* buffer) {
    char* fullPath = buildFullPath(webDir, filename);
    FILE* inFile = fopen(fullPath, "rb");
    if (inFile == NULL) {
        free(fullPath);
        return -1;
    }
    free(fullPath);
    fread(buffer, 1, size, inFile);
    fclose(inFile);
    return 0;
}

bool readFileContent(char* filename, size_t size, char* buffer) {
    int result = readFileContentRaw(filename, size, buffer);
    return (result == 0);
}


int getFileSize(char* filename) {
    char* fullPath = buildFullPath(webDir, filename);
    FILE* inFile = fopen(fullPath, "rb");
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

HandlerResult staticFileHandlerFunc(HttpRequest* req, HttpResponse* resp, HashTable* ht) {
    int fileSize = getFileSize(req->url);
    if (fileSize < 0) {
        resp->statusCode = 404;
        resp->headers.contentType = "text/html";
        const char* notFoundMsg = "<h1>404 - File Not Found</h1>";
        resp->body = calloc(1, strlen(notFoundMsg));
        strcpy(resp->body, notFoundMsg);
        return REQ_STOP;
    } else {
        char* fileContent = calloc(1, fileSize);
        readFileContent(req->url, fileSize, fileContent);
        resp->statusCode = 200;
        if (resp->headers.contentType == NULL)
            resp->headers.contentType = getMimeType(req->url);
        resp->body = fileContent;
        if (strcmp(resp->headers.contentType, "text/html") != 0) {
            resp->binaryBodySize = fileSize;
            return REQ_STOP;
        }
        resp->binaryBodySize = -1;
        return REQ_CONTINUE;
    }
}

ReqHandler* staticFileHandler() {
    return newReqHandler("static file", staticFileHandlerFunc);
}