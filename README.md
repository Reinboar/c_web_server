# C web server
This server implements a small subset of the HTTP/1.1 Web protocol, essentially just enough to enable basic document navigation and form submission. Importantly, it does not provide support for HTTPS connections or encryption of any kind, so DO NOT use this software for anything even remotely sensitive.
## Current Features
* __GET__ and __POST__ request methods.

## Planned Features
* Directory traversal prevention.
* Support for the Basic Authentication header.
* Request handler chaining.
* Built-in document templating for __POST__ requests.

## Building and Usage
Execute the make script.

`$ ./make.sh`

An executable named `server` will be created which will host any files present in the `web` directory over HTTP in response to __GET__ requests. __POST__ requests will simply send the request body, as is, in the response body. This is placeholder functionality and will be replaced in subsequent updates.