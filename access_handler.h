#ifndef ACCESS_HANDLER_H
#define ACCESS_HANDLER_H

#include "http_prot.h"
#include "req_handler.h"

/*
Receive request
if requested page is restricted:
    if authentication creds are given:
        if creds are valid:
            respond with the contents of the page
        else:
            respond with invalid creds status code
    else:
        respond with need to authenticate status code to prompt for creds
else:
    continue handling the request
*/

// Takes a base64 string and returns its decoded output.
uint8_t* decodeBase64(const char* in, size_t inLen, size_t* outLen);

// Checks if a page with the given URL has access controls on it.
// Returns true if the no error was encountered, false otherwise.
// If no error is encountered, result is populated with whether the page is restricted or not.
bool checkPageRestricted(char* url, bool* result);

// Parses and decodes the credentials given in an Authorization request header.
// Returns true if the credentials were present and able to be decoded.
// Returns false if there were no credentials or they were unable to be decoded.
// *name and *pass are assumed to be buffers of sufficient size to store the decoded
// username and password.
bool decodeCredentials(HttpRequest* req, char** name, char** pass);

// Checks the given credentials.
// Returns true if credentials are valid, false otherwise.
bool validateCredentials(char* name, char* pass, bool* userValidated);

ReqHandler* accessHandler();

#endif