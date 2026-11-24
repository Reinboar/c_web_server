#!/usr/bin/bash
gcc string_util.c http_prot.c static_handler.c req_handler.c hash_table.c template_parser.c access_handler.c main.c -o server -lsqlite3 -lssl -lcrypto -lrt
