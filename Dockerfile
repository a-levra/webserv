FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    python3 \
    && rm -rf /var/lib/apt/lists/* \
    && mkdir -p /app/uploadedFiles

COPY . /app
WORKDIR /app

RUN make re

EXPOSE 4243

CMD ["sh", "-c", "./webserv -l1 -c resources/webserv.conf"]