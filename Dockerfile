FROM ubuntu:jammy

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*


COPY . /app

WORKDIR /app

RUN make

EXPOSE 8000

CMD ["sh", "-c", "make re && ./webserv webserv.conf"]