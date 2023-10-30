FROM ubuntu:jammy

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    curl \
    telnet \
    && rm -rf /var/lib/apt/lists/* \
    && mkdir -p /app/uploadedFiles


COPY . /app

WORKDIR /app

EXPOSE 8000

CMD ["sh", "-c", "make re && ./webserv resources/webserv.conf"]