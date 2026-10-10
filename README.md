# epoll-tcp

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)
![Platform: Linux](https://img.shields.io/badge/platform-Linux-lightgrey)
![Docker](https://img.shields.io/badge/built%20with-Docker-2496ED)
[![License: MIT](https://img.shields.io/badge/license-MIT-green)](LICENSE)

A TCP server built directly on Linux `epoll`, with a terminal dashboard for watching its connections live.

## Status

Early. Build, container and tooling are in place; the event loop is not written yet.

## Running

Requires Docker. `epoll` is Linux-only, so everything builds and runs in a container.

```sh
docker compose build
docker compose run --rm server bash
```

Inside the container:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/server
```

The repository is bind-mounted at `/app`, so host edits are visible immediately and rebuilds are incremental. More commands in [docs/docker-cheatsheet.md](docs/docker-cheatsheet.md).

## Design notes

- **One image, two containers.** Server and client share a development image but run in separate network namespaces, so traffic crosses a virtual link instead of loopback. The client is a test tool behind the `test` Compose profile.
- **Three build stages.** `dev` holds the toolchain, `build` compiles a release, `runtime` ships only the server binary.
- **Debian 13 slim.** glibc rather than musl, so behaviour and tooling match typical Linux server targets.
- **Strict C++20.** Compiler extensions off, warnings as errors.

## License

[MIT](LICENSE)
