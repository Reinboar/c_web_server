#ifndef TEMPLATE_PARSER_H
#define TEMPLATE_PARSER_H

#include "hash_table.h"
#include "req_handler.h"

char* renderTemplate(char* src, HashTable* ht);
ReqHandler* templateHandler();

#endif