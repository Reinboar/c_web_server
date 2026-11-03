#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

#include "http_prot.h"
#include "req_handler.h"

#define WEB_DIR "./web"

char* buildFullPath(const char* basePath, const char* filename) {
    int pathLen = strlen(filename) + strlen(basePath);
    char* fullPath = malloc(pathLen + 1);
    memcpy(fullPath, "", 1);
    strcat(fullPath, basePath);
    strcat(fullPath, filename);
    return fullPath;
}

int readFileContent(char* filename, char* buffer) {
    char* fullPath = buildFullPath(WEB_DIR, filename);
    char* rdBuffer = malloc(100);
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

int sendResp(int fd, int status, const char* msg, int len, char* body) {
    const char* respFmt = "HTTP/1.1 %d %s\r\nContent-Type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n";
    char* respHeader = malloc(strlen(respFmt) + len + 1024);
    sprintf(respHeader, respFmt, status, msg, len);
    send(fd, respHeader, strlen(respHeader), 0);
    if (body != NULL) {
        send(fd, body, len, 0);
    }
    close(fd);
    free(respHeader);
    return 0;
}

int processRequest(HttpRequest* req, int fd) {
    if (strcmp(req->method, "GET") == 0) {
        int contentLength = getFileSize(req->url);
        char* bodyContent;
        if (contentLength < 0) { // if requested file does not exist
            contentLength = getFileSize("/404.html");
            if (contentLength < 0) { // if custom 404 document does not exist
                sendResp(fd, 404, "Not Found", 0, NULL);
            } else { // if custom 404 document does exist
                bodyContent = calloc(1, contentLength);
                readFileContent("/404.html", bodyContent);
                sendResp(fd, 404, "Not Found", contentLength, bodyContent);
                free(bodyContent);
            }
            return 0;
        }
        bodyContent = calloc(1, contentLength);
        readFileContent(req->url, bodyContent);
        sendResp(fd, 200, "OK", contentLength, bodyContent);
        free(bodyContent);
        return 0;
    } else if (strcmp(req->method, "POST") == 0) {
        sendResp(fd, 200, "OK", strlen(req->body), req->body);
        return 0;
    } else {
        char* errorMsg = "UNSUPPORTED HTTP METHOD";
        sendResp(fd, 500, "Server Error", strlen(errorMsg), errorMsg);
        return 0;
    }
}
