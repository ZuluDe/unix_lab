FROM alpine:3.20

RUN apk add --no-cache flock

WORKDIR /app
COPY script.sh /app/script.sh
RUN chmod +x /app/script.sh

VOLUME ["/data"]

ENTRYPOINT ["/bin/sh", "/app/script.sh"]
