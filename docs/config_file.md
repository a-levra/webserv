# Configuration file
**Find below the documentation for the webserv configuration file.**

## Directives

### http:
```
Syntax:	http { ... }
Default: —
Context: main
```
Provides the configuration file context in which the HTTP server directives are specified.

### server:
```
Syntax:	server { ... }
Default: —
Context: http
```
Sets configuration for a virtual server.

### listen:
```
Syntax: listen address[:port] 
Default: listen *:80;
Context: server
```
Sets the address and port for IP, or the path for a UNIX-domain socket on which the server will accept requests. An address may also be "localhost", for example:
```
listen 127.0.0.1:8000;
listen localhost:8000;
```

### server_name:
```
Syntax:	server_name name ...;
Default: server_name "";
Context: server
```
Sets names of a virtual server, for example:
```
server {
    server_name example.com www.example.com;
}
```
The first name becomes the primary server name.

### location:
Sets configuration depending on a request URI.
```
Syntaxe: location uri { ... }
Défaut:	—
Contexte: server
```

### error_page:
```
Syntax:	error_page code ... [=[response]] uri;
Default: —
Context: http, server, location
```
Defines the URI that will be shown for the specified errors. Example:
```
error_page 400         /404.html;
error_page 300 400 500 /50x.html;
```

### root:
```
Syntax:	root path;
Default: root html;
Context: http, server, location
```
Sets the root directory for requests. For example, with the following configuration
```
location /i/ {
    root /data/w3;
}
```

### return:
```
Syntax:	return code URL;
Default: —
Context: server, location
```

### index:
```
Syntax: index file ...;
Default: index index.html;
Context: http, server, location
```
Defines files that will be used as an index. Files are checked in the specified order. Example:
```
index index.html index0.html something.html;
```

### allow_methods:
```
Syntax: allow_methods ...
Default: allow_methods GET POST DELETE;
Context: http, server, location
```
Allowed methods, for example:
```
allow_methods GET POST;
```

### client_max_body_size:
```
Syntax: client_max_body_size size;
Default: client_max_body_size 1m;
Context: http, server, location
```
Sets the maximum allowed size of the client request body.

### autoindex:
```
Syntax:	autoindex on | off;
Default: autoindex off;
Context: http, server, location
```
Enables or disables the directory listing output.

### cgi_path:
```
Syntax: cgi_path extension:path
Default: —
Context: location
```
Defines a location as accepting cgi.
```
location /cgi-bin/ {
    root cgi-bin/;
    cgi_path: .py:/usr/bin/python3 .php:/usr/bin/php;
    index main.py;
}
```
