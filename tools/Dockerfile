FROM debian
RUN apt-get update
RUN apt-get install -y libcurl3-dev
RUN mkdir /home/gallocy
ADD install /home/gallocy
ENV LD_LIBRARY_PATH /home/gallocy/lib
CMD ["/home/gallocy/bin/httpd"]
