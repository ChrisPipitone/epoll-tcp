# Docker cheatsheet

All commands run from the repo root.

## Daily loop

| Command | Does |
|---|---|
| `docker compose run --rm server bash` | Shell in the dev container, repo mounted at `/app` |
| `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug` | (inside) Configure. Once, and again after editing `CMakeLists.txt` |
| `cmake --build build` | (inside) Compile what changed |
| `./build/server` | (inside) Run it |

Most sessions: open one shell, leave it open, repeat the last two.

## Running services

| Command | Does |
|---|---|
| `docker compose up` | Start the server with its configured command, logs in foreground |
| `docker compose up -d` | Same, in the background |
| `docker compose --profile test up` | Server and client |
| `docker compose run --rm client` | One-off client |
| `docker compose down` | Stop and remove the project's containers and network |

`up` vs `run`: `up` starts services as the compose file defines them. `run` starts one throwaway container and lets you replace the command.

## Inspecting

| Command | Does |
|---|---|
| `docker compose ps` | This project's running containers (`-a` includes stopped) |
| `docker compose logs -f server` | Follow the server's output |
| `docker compose exec server bash` | Shell inside the already running server container |
| `docker compose attach server` | Connect the terminal to the running server (needed for the TUI) |

`exec` vs `run`: `exec` enters an existing container, so you see the live process. `run` creates a new one.

## After changing the Dockerfile

| Command | Does |
|---|---|
| `docker compose build` | Rebuild `epoll-tcp:dev` |
| `docker build -t epoll-tcp .` | Build the runtime image (full release compile) |
| `docker run --rm epoll-tcp` | Run the runtime image |

Source edits do not need a rebuild; the bind mount handles that. Only Dockerfile changes do.

## Cleanup

| Command | Does |
|---|---|
| `docker ps -a` | Every container on the machine, any project |
| `docker image ls` | Every image |
| `docker image rm <name>` | Delete an image |
| `docker system df` | Disk used by images, containers, build cache |
| `docker system prune` | Delete stopped containers, unused networks, dangling images |

`prune` is machine-wide, not per project. Read what it lists before confirming.

## Troubleshooting

- `docker compose config` prints the compose file as Docker understands it. First stop for YAML or indentation mistakes.
- `executable file not found`: the `command` points at a binary that does not exist yet. Compile first.
- Daemon connection errors: Docker Desktop is not running.
- Editor LSP shows nothing: `build/compile_commands.json` is missing. Run the configure step once.
