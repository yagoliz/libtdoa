FROM debian:sid
LABEL authors="Yago Lizarribar"
ARG PACKAGES="ca-certificates uuid-dev zlib1g-dev ninja-build libeigen3-dev libceres-dev libgtest-dev libboost-dev \
nlohmann-json3-dev libjsoncpp-dev git cmake build-essential libboost-program-options-dev libdrogon-dev postgresql-all \
libsqlite3-dev libmysql++-dev libbrotli-dev libhiredis-dev libyaml-cpp-dev"
ARG BUILD_TYPE=Release

ENV PORT 8095
ENV IP "0.0.0.0"
ENV API "locate"
ENV THREADS "4"
ENV LOG "/tmp"

# Basic stuff
RUN apt update && apt install --no-install-recommends -y $PACKAGES

# Build our project
RUN rm -rf /tdoapp
ADD ./ /tdoapp
WORKDIR /tdoapp
RUN rm -rf build cmake-build-*
RUN ls -al
RUN mkdir build
RUN cmake -G Ninja -B ./build -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DBUILD_TESTS=ON -DBUILD_EXECUTABLES=ON
RUN cmake --build ./build --config $BUILD_TYPE
RUN cd build && ctest -C BUILD_TYPE

# Install and run
RUN cd build && ninja install
WORKDIR /
RUN rm -rf tdoapp

ENTRYPOINT TdoaRest -p $PORT -i $IP -e "/$API" -t $THREADS -l $LOG