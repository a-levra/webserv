# Configuration file
**Find below the documentation for the webserv configuration file.**
## Directives

### http:
Provides the configuration file context in which the HTTP server directives are specified.
```
Syntax:	http { ... }
Default: —
Context: main
```

### server:
Sets configuration for a virtual server.
```
Syntax:	server { ... }
Default: —
Context: http
```

### listen:
Sets the address and port for IP, or the path for a UNIX-domain socket on which the server will accept requests. Both address and port, or only address or only port can be specified. An address may also be a hostname, for example:

```
listen 127.0.0.1:8000;
listen 127.0.0.1;
listen 8000;
listen *:8000;
listen localhost:8000;
```
```
Syntax: listen address[:port] 
Default: listen *:80;
Context: server
```

### server_name:

Sets names of a virtual server, for example:

```
server {
    server_name example.com www.example.com;
}
```
The first name becomes the primary server name.

```
Syntax:	server_name name ...;
Default: server_name "";
Context: server
```

### location:
Sets configuration depending on a request URI.
```
Syntaxe: location uri { ... }
Défaut:	—
Contexte: server
```

### error_page
Defines the URI that will be shown for the specified errors. Example:

```
error_page 400         /404.html;
error_page 300 400 500 /50x.html;
```
```
Syntax:	error_page code ... [=[response]] uri;
Default:	—
Context:	http, server, location
```

### root
Sets the root directory for requests. For example, with the following configuration
```
location /i/ {
    root /data/w3;
}
```
```
Syntax:	root path;
Default: root html;
Context: http, server, location
```

### index
Defines files that will be used as an index. Files are checked in the specified order. Example:
```
index index.html index0.html something.html;
```
```
Syntax: index file ...;
Default: index index.html;
Context: http, server, location
```

### allow_methods
Allowed methods, for example:
```
allow_methods GET POST;
```
```
Syntax: allow_methods ...
Default: allow_methods GET POST DELETE;
Context: http, server, location
```

### client_max_body_size
Sets the maximum allowed size of the client request body.
```
Syntax: client_max_body_size size;
Default: client_max_body_size 1m;
Context: http, server, location
```

### autoindex
Enables or disables the directory listing output.
```
Syntax:	autoindex on | off;
Default: autoindex off;
Context: http, server, location
```

