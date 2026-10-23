FROM alpine:3.20

RUN apk add --no-cache flock

WORKDIR /app
COPY lab_two.sh /app/lab_two.sh
RUN chmod +x /app/lab_two.sh

VOLUME ["/data"]

ENTRYPOINT ["/bin/sh", "/app/lab_two.sh"]
