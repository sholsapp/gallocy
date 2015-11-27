FROM debian
RUN apt-get update
RUN apt-get install -y libcurl3-dev arping
RUN mkdir /home/gallocy
ADD install /home/gallocy
ENV LD_LIBRARY_PATH /home/gallocy/lib
# Execute a shell command when starting an instance because we need time to
# configure the network interfaces after we start an instance.
CMD ["/bin/bash"]
#CMD ["/home/gallocy/bin/httpd"]
