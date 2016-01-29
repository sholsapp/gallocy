# A gallocy testing container.

FROM debian
MAINTAINER Stephen Holsapple <sholsapp@gmail.com>
RUN apt-get update && apt-get install -y \
  # curl \
  # libcurl3-dev \
  net-tools \
  tree \
  && apt-get clean
RUN mkdir /home/gallocy
RUN mkdir /home/gallocy/etc
RUN mkdir /home/gallocy/var
ADD install /home/gallocy
ENV LD_LIBRARY_PATH /home/gallocy/lib
#VOLUME ["/home/gallocy/etc"]
#VOLUME ["/home/gallocy/var"]
EXPOSE 8080
# Disable Address Space Layout Randomisation (ASLR) before starting the DSM.
CMD echo "/home/gallocy/var/core.server" > /proc/sys/kernel/core_pattern && \
  ulimit -c unlimited && \
  setarch x86_64 -R /home/gallocy/bin/server /home/gallocy/etc/config.json
