# C web server
This server implements a small subset of the HTTP/1.1 Web protocol, essentially just enough to enable basic document navigation and form submission. Importantly, it does not provide support for HTTPS connections or encryption of any kind, so DO NOT use this software for anything even remotely sensitive.

## Dependencies
* SQLite 3
* OpenSSL

## Current Features
* __GET__ and __POST__ request methods.
* Request handler chaining.
* Built-in page templating.
* Support for the Basic Authentication header.

## Planned Features
* Directory traversal prevention.

## Building and Usage
Execute the make script.

`$ ./make.sh`

Run the server.
`$ ./server`