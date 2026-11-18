# MU Linux 0.97k — editor API

Internal API that the MU Linux web panel uses to edit the server data safely:
it only touches files inside the server's data folders, keeps a backup of each
file it writes and can take and restore full snapshots. `amd64` and `arm64`.

Part of [Mu-Linux](https://github.com/EmanuelCatania/Mu-Linux). Documentation at
[mu-linux.com](https://mu-linux.com).

## Tags

| Tag | What it is |
|---|---|
| `{{VERSION}}` | Editor of the {{VERSION}} release |
| `latest` | The latest release (currently `{{VERSION}}`) |

## Use

Optional. It runs next to `mu-linux-97k-web` and shares the server's data
volumes (`/opt/mu/Data` and `/opt/mu/GameServer/Data`); backups go to
`/backups`. It does not need a public port: only the web panel talks to it.

| Variable | Use |
|---|---|
| `EDITOR_ENABLED` | Turns the API on |
| `EDITOR_PORT` | Internal port |
| `EDITOR_MAX_BACKUPS`, `EDITOR_MAX_SNAPSHOTS` | Backup retention |

- [Repository](https://github.com/EmanuelCatania/Mu-Linux)
- [Releases](https://github.com/EmanuelCatania/Mu-Linux/releases)
