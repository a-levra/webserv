FROM ubuntu:jammy

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    curl \
    telnet \
    cmake \
    gdb \
    && rm -rf /var/lib/apt/lists/* \
    && mkdir -p /app/uploadedFiles


COPY . /app

WORKDIR /app

EXPOSE 4243

CMD ["sh", "-c", "make re && ./webserv -l0 -c resources/webserv.conf"]