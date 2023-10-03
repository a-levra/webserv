#include "HttpRequest.hpp"

int main() {
	HttpRequest myRequest;

	myRequest.setRawRequest("DELETE  /./123/./. HTTP/1.1\r\n"
	"Host: localhost:8080\r\n"
	"User-Agent: Mozilla.5/0 (X11; Linux x86_64; rv:78.0) Gecko/20100101 Firefox/78.0\r\n"
	"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
	"Accept-Language: en-US,en;q=0.5\r\n"
	"Accept-Encoding: gzip, deflate\r\n"
	"Connection: keep-alive\r\n"
	"Upgrade-Insecure-Requests: 1\r\n"
	"\r\n"
	"");
	myRequest.parse();
	myRequest.displayRequest();
	return 0;
}
