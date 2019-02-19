# This is a test Dockerfile.

FROM ubuntu

#MAINTAINER Paul Blischak <blischak.4@osu.edu>

# Get g++ for compiling, wget to download Boost, git to clone source code repo,
# and make to automate program compilation with Makefile provided
RUN apt-get update 
RUN apt-get install  -y git 
RUN apt-get install  -y g++ 
RUN apt-get install  -y make 
RUN apt-get install  -y wget
RUN apt-get install  -y wget
RUN apt-get install -y cmake

RUN apt-get install -y qtbase5-dev  
RUN apt-get install -y libqt5sql5


RUN apt-get install -y libboost-all-dev

RUN apt-get install -y postgresql-client libpq-dev 
RUN apt-get install -y libqt5sql5-psql

# Download boost, untar, setup install with bootstrap and only do the Program Options library,
# and then install

#WORKDIR home
#RUN wget https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.gz 
#RUN tar xfz boost_1_69_0.tar.gz 
#RUN rm boost_1_69_0.tar.gz 
#RUN cd boost_1_69_0 \
#&& ./bootstrap.sh --prefix=/usr/local --with-libraries=program_options \
#&& ./b2 install 
  

RUN pwd
RUN ls

RUN ls
RUN git clone https://github.com/fatholah/tank.io-server.git



#COPY Tank.io/Server/tank-io ./Tank.io/Server/tank-io
#COPY Tank.io/Classes ./Tank.io/Classes
#RUN ls Tank.io/Classes/ImportantCommon/MathShapeUtils
WORKDIR tank.io-server
RUN ls
RUN cmake .
RUN make





#RUN ./tank-io.server
CMD ["./tank-io.server"]
