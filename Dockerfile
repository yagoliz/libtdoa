FROM ubuntu:latest
LABEL authors="Yago Lizarribar"
ENV PACKAGES ca-certificates uuid-dev zlib1g-dev ninja-build libeigen3-dev libceres-dev libgtest-dev libboost-dev nlohmann-json3-dev libjsoncpp-dev git cmake build-essential libboost-program-options-dev
ENV BUILD_TYPE Release

ENV PORT 8095
ENV IP "0.0.0.0"
ENV API "locate"
ENV THREADS "4"
ENV LOG "/tmp"

# Basic stuff
RUN apt update && apt install --no-install-recommends -y $PACKAGES

# Get Drogon
RUN git clone https://github.com/drogonframework/drogon.git
RUN cd drogon && ./build.sh && cd ..

# Build our project
RUN rm -rf $HOME/tdoapp
ADD ./ $HOME/tdoapp/
WORKDIR $HOME/tdoapp
RUN rm -rf build cmake-build-*
RUN ls -al
RUN mkdir build
RUN cmake -G Ninja -B ./build -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DBUILD_TESTS=ON -DBUILD_EXECUTABLES=ON
RUN cmake --build ./build --config $BUILD_TYPE
RUN cd build && ctest -C BUILD_TYPE

# Install and run
RUN cd build && ninja install

ENTRYPOINT TdoaRest -p $PORT -i $IP -e "/$API" -t $THREADS -l $LOG