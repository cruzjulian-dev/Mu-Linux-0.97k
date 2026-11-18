# MU Linux 0.97k — server

MU Online 0.97k server for Linux: ConnectServer, JoinServer, DataServer and
GameServer in one image, with MySQL through the C API. `amd64` and `arm64`.

Part of [Mu-Linux](https://github.com/EmanuelCatania/Mu-Linux), the open
reconstruction of the 0.97k client and server. Documentation in Spanish, English
and Portuguese at [mu-linux.com](https://mu-linux.com).

## Tags

| Tag | What it is |
|---|---|
| `{{VERSION}}` | This release |
| `latest` | The latest release (currently `{{VERSION}}`) |

Pin the version tag in production so a `pull` does not move you to a new
protocol by surprise.

## Compatibility

Client and server **share the version and must be updated together**: the
protocol is negotiated on connect, and a client of another version gets
*"wrong version"* at login. Release notes:
[{{VERSION}}](https://github.com/EmanuelCatania/Mu-Linux/releases/tag/{{VERSION}}).

There are no client binaries: each operator builds the client from the
repository with their own server address (see the client README).

## Run it

The easiest way is the repository's compose file, which also brings up MySQL,
the web panel and the editor API:

```bash
git clone https://github.com/EmanuelCatania/Mu-Linux.git
cd Mu-Linux
cp .env.example .env    # set passwords and PUBLIC_IP
docker compose -f docker-compose.images.yml up -d
```

| Port | Service |
|---|---|
| `44405/tcp` | ConnectServer (server list) |
| `55901/tcp` | GameServer |

| Volume | Contents |
|---|---|
| `/opt/mu/Data` | Server data: items, maps, spawns, shops, texts, keys |
| `/opt/mu/GameServer/Data` | GameServer `.dat` / `.ini` files |
| `/opt/mu/GameServer/LOG` | GameServer logs |

## Security

All traffic between the client and the GameServer and ConnectServer is
encrypted and authenticated with the server's own key,
`Data/Keys/ServerIdentity.key`, generated on first start. Keep it with your
`Data/` backup: it is your server's identity for every client that already
connected.

## Upgrading

`docker pull` does **not** update files that live in a volume (`Data/` and the
`.ini` files). Each release's README section *"Upgrading from …"* says which
files are new or changed and whether you need them.

- [Repository and README](https://github.com/EmanuelCatania/Mu-Linux)
- [Releases](https://github.com/EmanuelCatania/Mu-Linux/releases)
- [Documentation](https://mu-linux.com)
