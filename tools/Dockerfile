# A gallocy testing container.

FROM debian

MAINTAINER Stephen Holsapple <sholsapp@gmail.com>

RUN apt-get update && apt-get install -y \
  curl \
  libcurl3-dev \
  net-tools \
  tree \
  && apt-get clean
RUN mkdir /home/gallocy
RUN mkdir /home/gallocy/etc

ADD install /home/gallocy
ADD tools/pipework /home/gallocy

ENV LD_LIBRARY_PATH /home/gallocy/lib
VOLUME ["/home/gallocy/etc"]

EXPOSE 8080

# Disable Address Space Layout Randomisation (ASLR) before starting the DSM.
CMD ["setarch", "x86_64", "-R", "/home/gallocy/bin/server", "/home/gallocy/etc/config.json"]
