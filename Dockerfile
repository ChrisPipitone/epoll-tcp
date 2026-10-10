FROM debian:13-slim AS dev
RUN apt-get update && apt-get install -y --no-install-recommends \
  build-essential cmake gdb git ca-certificates \
  && rm -rf /var/lib/apt/lists/*
WORKDIR /app

FROM dev AS build 
COPY CMakeLists.txt .
COPY src/ src/
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  && cmake --build build 

FROM debian:13-slim AS runtime
COPY --from=build /app/build/server /usr/local/bin/
CMD ["server"]
