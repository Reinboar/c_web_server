#include <stdint.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <stdio.h>
#include <openssl/sha.h>

#include "access_handler.h"
#include "string_util.h"
#include "template_parser.h"

static sqlite3* accessDb = NULL;

/* -1 = invalid, -2 = padding ('='). Index by unsigned byte value. */
static const int8_t b64_dec_table[256] = {
    ['A']= 0,['B']= 1,['C']= 2,['D']= 3,['E']= 4,['F']= 5,['G']= 6,['H']= 7,
    ['I']= 8,['J']= 9,['K']=10,['L']=11,['M']=12,['N']=13,['O']=14,['P']=15,
    ['Q']=16,['R']=17,['S']=18,['T']=19,['U']=20,['V']=21,['W']=22,['X']=23,
    ['Y']=24,['Z']=25,['a']=26,['b']=27,['c']=28,['d']=29,['e']=30,['f']=31,
    ['g']=32,['h']=33,['i']=34,['j']=35,['k']=36,['l']=37,['m']=38,['n']=39,
    ['o']=40,['p']=41,['q']=42,['r']=43,['s']=44,['t']=45,['u']=46,['v']=47,
    ['w']=48,['x']=49,['y']=50,['z']=51,['0']=52,['1']=53,['2']=54,['3']=55,
    ['4']=56,['5']=57,['6']=58,['7']=59,['8']=60,['9']=61,['+']=62,['/']=63,
    ['=']=-2,
};

/* Decodes a null-terminated Base64 string. On success returns a malloc'd
 * buffer and writes the byte count to *out_len. Returns NULL on error. */
uint8_t* decodeBase64(const char* in, size_t in_len, size_t* out_len) {
    if (in_len % 4 != 0) return NULL;          /* must be a multiple of 4 */
    if (in_len == 0) { *out_len = 0; return malloc(1); }

    size_t pad = 0;
    if (in[in_len - 1] == '=') pad++;
    if (in[in_len - 2] == '=') pad++;

    size_t cap = (in_len / 4) * 3 - pad;
    uint8_t *out = calloc(1, cap ? cap + 1 : 1);
    if (!out) return NULL;

    size_t i, j = 0;
    for (i = 0; i < in_len; i += 4) {
        int8_t a = b64_dec_table[(uint8_t)in[i]];
        int8_t b = b64_dec_table[(uint8_t)in[i + 1]];
        int8_t c = b64_dec_table[(uint8_t)in[i + 2]];
        int8_t d = b64_dec_table[(uint8_t)in[i + 3]];

        if (a == -1 || b == -1) { free(out); return NULL; }

        uint32_t n = ((uint32_t)a << 18) | ((uint32_t)b << 12);
        out[j++] = (n >> 16) & 0xFF;

        if (c != -2) {                          /* not padding */
            n |= (uint32_t)c << 6;
            out[j++] = (n >> 8) & 0xFF;
            if (d != -2) {
                n |= (uint32_t)d;
                out[j++] = n & 0xFF;
            }
        }
    }

    *out_len = j;
    return out;
}

bool connectDb() {
    int result = sqlite3_open("./access.db", &accessDb);
    return (result == SQLITE_OK);
}

int checkPageRestrictedCallback(void* pageRestricted, int numCols, char** vals, char** names) {
    *((bool*)pageRestricted) = true;
    return 0;
}

bool checkPageRestricted(char* url, bool* result) {
    const char* sqlFmtStr = "SELECT url FROM pages WHERE url = \"%s\" LIMIT 1;";
    size_t finalLen = strlen(sqlFmtStr) + strlen(url) + 1;
    char* sqlFinal = calloc(1, finalLen);
    int actualBytes = snprintf(sqlFinal, finalLen, sqlFmtStr, url);
    printf("SQL Statement: %s\n", sqlFinal);
    if (actualBytes >= finalLen)
        return false; // return false if the SQL string ended up being truncated during formatting
    int sqlResult = sqlite3_exec(accessDb, sqlFinal, checkPageRestrictedCallback, result, NULL);
    if (sqlResult != SQLITE_OK)
        return false;
    return true; 
}

bool decodeCredentials(HttpRequest* req, char** user, char** pass) {
    printf("Decoding credentials...\n");
    if (!req->headers.authorization)
        return false;
    if (strncmp(req->headers.authorization, "Basic ", 6) != 0) // check that the authorization header begins properly
        return false;

    size_t encodedCredsLen = strlen(req->headers.authorization) - 6; // calculate the length of the buffer for credentials minus the "Basic " portion
    char* encodedCreds = calloc(1, encodedCredsLen); // allocate memory for credentials
    strncpy(encodedCreds, &(req->headers.authorization[6]), encodedCredsLen); // copy the credentials from the header to the buffer
    printf("Authorization Header: '%s'\n", req->headers.authorization);
    printf("Encoded Credentials: '%s' Length: %d\n", encodedCreds, encodedCredsLen);
    size_t decodedCredsLen = 0;
    char* decodedCreds = decodeBase64(encodedCreds, encodedCredsLen, &decodedCredsLen); // decode the credentials and store in a new buffer
    if (decodedCreds == NULL)
        return false; // indicate error if the credentials could not be decoded
    printf("Decoded Credentials: '%s' Length: %d\n", decodedCreds, decodedCredsLen);

    char* passPtr = strchr(decodedCreds, ':');
    passPtr++;
    printf("Creds Pass: '%s'\n", passPtr);
    size_t passPtrLen = strlen(passPtr);
    *pass = calloc(1, passPtrLen+1);
    strncpy(*pass, passPtr, passPtrLen-1);
    printf("True Pass: '%s' Length: %d\n", *pass, strlen(*pass));
    size_t userLen = passPtr - decodedCreds - 1;
    printf("True User Length: %d\n", userLen);
    *user = calloc(1, userLen + 1);
    strncpy(*user, decodedCreds, userLen);
    printf("USER: '%s' PASS: '%s'\n", *user, *pass);
    return true;
}

int retrieveUserSaltCallback(void* salt, int numCols, char** vals, char** names) {
    size_t saltLen = strlen(vals[0]);
    *(char**)salt = calloc(1, saltLen + 1);
    strncpy(*(char**)salt, vals[0], saltLen);
    return 0;
}

bool retrieveUserSalt(char* name, char** salt) {
    const char* sqlFmtStr = "SELECT salt FROM users WHERE name = \"%s\" LIMIT 1;";
    size_t finalLen = strlen(sqlFmtStr) + strlen(name) + 1;
    char* sqlFinal = calloc(1, finalLen);
    int actualBytes = snprintf(sqlFinal, finalLen, sqlFmtStr, name);
    printf("SQL Statement: %s\n", sqlFinal);
    if (actualBytes >= finalLen)
        return false; // return false if the SQL string ended up being truncated during formatting
    int sqlResult = sqlite3_exec(accessDb, sqlFinal, retrieveUserSaltCallback, salt, NULL);
    if (sqlResult != SQLITE_OK)
        return false;
    return true;
}

int validateCredentialsCallback(void* userValidated, int numCols, char** vals, char** names) {
    *(bool*)userValidated = true;
    return 0;
}

bool validateCredentials(char* name, char* pass, bool* userValidated) {
    // Get salt from database and calculate the SHA1 hash of the given password
    char* salt = NULL;
    if (!retrieveUserSalt(name, &salt))
        return false;
    printf("Successfully retrieved salt\n");
    printf("User Salt: '%s'\n", salt);
    size_t finalPayloadLen = strlen(pass) + strlen(salt);
    char* finalPayload = calloc(1, finalPayloadLen + 1);
    unsigned char* rawHash = calloc(1, 20); // SHA1 hashes are always 20 characters long
    char* finalHash = calloc(1, 41);
    strncat(finalPayload, pass, strlen(pass));
    strncat(finalPayload, salt, strlen(salt));
    printf("Payload: '%s'\n", finalPayload);
    SHA1(finalPayload, finalPayloadLen, rawHash);
    for (int i=0; i < 20; i++) {
        sprintf(finalHash + i * 2,"%02x", rawHash[i]);
    }

    // Create SQL statement and search for user with given name and password
    const char* sqlFmtStr = "SELECT * FROM users WHERE name = \"%s\" AND hash = \"%s\" LIMIT 1;";
    size_t finalLen = strlen(sqlFmtStr) + strlen(name) + strlen(finalHash) + 1;
    char* sqlFinal = calloc(1, finalLen);
    int actualBytes = snprintf(sqlFinal, finalLen, sqlFmtStr, name, finalHash);
    printf("SQL Statement: %s\n", sqlFinal);
    if (actualBytes >= finalLen)
        return false; // return false if the SQL string ended up being truncated during formatting
    printf("Constructed SQL statement\n");
    int sqlResult = sqlite3_exec(accessDb, sqlFinal, validateCredentialsCallback, userValidated, NULL);
    printf("SQL ERROR: '%s'\n", sqlite3_errmsg(accessDb));
    if (sqlResult != SQLITE_OK)
        return false;
    return true;
}

bool accessHandlerFunc(HttpRequest* req, HttpResponse* resp, HashTable* ht) {
    bool pageRestricted = false;
    if (!checkPageRestricted(req->url, &pageRestricted)) { // if error is encountered while checking access controls
        resp->statusCode = 500;
        resp->body = copyString("<h1>500 - Internal Server Error - A</h1>");
        return false;
    }

    char* user = NULL;
    char* pass = NULL;
    bool userValidated = false;
    if (pageRestricted) {
        if (decodeCredentials(req, &user, &pass)) {
            if (!validateCredentials(user, pass, &userValidated)) {
                resp->statusCode = 500;
                resp->body = copyString("<h1>500 - Error while validating credentials</h1>");
                return false;
            }
            if (userValidated) {
                return true;
            } else {
                resp->statusCode = 401;
                resp->body = copyString("<h1>401 - Unauthenticated</h1>");
                resp->headers.wwwAuthenticate = "Basic realm=site";
            }
        } else {
            resp->statusCode = 401;
            resp->body = copyString("<h1>401 - Unauthenticated</h1>");
            resp->headers.wwwAuthenticate = "Basic realm=site";
            return false;
        }
    } else {
        return true;
    }
}

ReqHandler* accessHandler() {
    connectDb();
    return newReqHandler("access verification", accessHandlerFunc);
}