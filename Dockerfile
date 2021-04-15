# --- Commented out lines are for centos:7 based build
# FROM centos:7 as SourceBuilder
FROM ubuntu:latest as SourceBuilder

# RUN yum install gcc zlib-devel make -y
RUN apt update
RUN apt install gcc zlib1g-dev make -y

COPY ./tgpub /gathering

WORKDIR /gathering/src
RUN /usr/bin/make clean
RUN /usr/bin/make

WORKDIR /gathering
RUN find . -type d | xargs -I {} mkdir -p /build/gathering/{}

RUN cp -R /gathering/area \
          /gathering/log \
          /gathering/license \
          /gathering/doc \
          /gathering/player \
          /gathering/saveroom \
            /build/gathering
RUN cp /gathering/src/Gathering /build/gathering/area/

FROM scratch
COPY --from=SourceBuilder /build/gathering /gathering
COPY --from=SourceBuilder /lib/x86_64-linux-gnu/libz.so.1 /lib/x86_64-linux-gnu/
COPY --from=SourceBuilder /lib/x86_64-linux-gnu/libcrypt.so.1 /lib/x86_64-linux-gnu/
COPY --from=SourceBuilder /lib/x86_64-linux-gnu/libpthread.so.0 /lib/x86_64-linux-gnu/
COPY --from=SourceBuilder /lib/x86_64-linux-gnu/libc.so.6 /lib/x86_64-linux-gnu/
COPY --from=SourceBuilder /lib/x86_64-linux-gnu/libgcc_s.so.1 /lib/x86_64-linux-gnu/
COPY --from=SourceBuilder /lib64/ld-linux-x86-64.so.2 /lib64/

# COPY --from=SourceBuilder /lib/ld-linux-aarch64.so.1 /lib/
# COPY --from=SourceBuilder /lib64/libz.so.1 /lib64/
# COPY --from=SourceBuilder /lib64/libcrypt.so.1 /lib64/
# COPY --from=SourceBuilder /lib64/libpthread.so.0 /lib64/
# COPY --from=SourceBuilder /lib64/libc.so.6 /lib64/
# COPY --from=SourceBuilder /lib64/libfreebl3.so /lib64/
# COPY --from=SourceBuilder /lib64/libdl.so.2 /lib64/
# COPY --from=SourceBuilder /lib64/libgcc_s.so.1 /lib64/

WORKDIR /gathering/area
EXPOSE 9100
ENTRYPOINT ["./gathering", "9100"]
