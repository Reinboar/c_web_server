#ifndef STATIC_HANDLER_H
#define STATIC_HANDLER_H

#include <stdbool.h>

#include "http_prot.h"
#include "req_handler.h"

// Set the directory where the server's web pages are stored
void setWebDir(char* newDir);

// Construct a path consisting of the web directory and the given filename
char* buildFullPath(const char* basePath, const char* filename);

// Read the contents of filename and write them into the given buffer
// Note: buffer must be large enough to contain the file's contents before calling
int readFileContent(char* filename, char* buffer);

// Get the size of the file named by filename
int getFileSize(char* filename);

bool staticFileHandlerFunc(HttpRequest* req, HttpResponse* resp, HashTable* ht);

ReqHandler* staticFileHandler();

#endif