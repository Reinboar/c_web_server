# C web server
This server implements a small subset of the HTTP/1.1 Web protocol, essentially just enough to enable basic document navigation and form submission. Importantly, it does not provide support for HTTPS connections or encryption of any kind, so DO NOT use this software for anything even remotely sensitive.
## Current Features
* __GET__ and __POST__ request methods.

## Planned Features
* Directory traversal prevention.
* Support for the Basic Authentication header.
* Request handler chaining.
* Built-in document templating for __POST__ requests.