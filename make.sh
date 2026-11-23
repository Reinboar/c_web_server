#!/usr/bin/bash
gcc string_util.c http_prot.c req_handler.c hash_table.c template_parser.c main.c -o server
