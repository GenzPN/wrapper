FROM ubuntu:latest

WORKDIR /app
#COPY --from=builder /app /app
COPY . /app
COPY wrapper /app/wrapper
COPY rootfs /app/rootfs
ENV args ""

# Add execute permission to wrapper
RUN chmod +x /app/wrapper

CMD ["bash", "-c", "./wrapper ${args}"]

EXPOSE 10020 20020

ENTRYPOINT ["./wrapper"]