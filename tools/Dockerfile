# A gallocy testing container.

FROM debian

MAINTAINER Stephen Holsapple <sholsapp@gmail.com>

RUN apt-get update && apt-get install -y libcurl3-dev tree && apt-get clean
RUN mkdir /home/gallocy
RUN mkdir /home/gallocy/etc

ADD install /home/gallocy
ADD tools/pipework /home/gallocy

ENV LD_LIBRARY_PATH /home/gallocy/lib
VOLUME ["/home/gallocy/etc"]

# Execute a shell command when starting an instance because we need time to
# configure the network interfaces after we start an instance.
#CMD ["/bin/bash"]

CMD ["/home/gallocy/pipework --wait -i eth1 && /home/gallocy/bin/httpd"]
