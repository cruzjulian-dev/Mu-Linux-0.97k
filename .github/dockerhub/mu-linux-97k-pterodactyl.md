# MU Linux 0.97k — Pterodactyl runtime

Minimal runtime image for the MU Linux 0.97k Pterodactyl egg: only the operating
system dependencies (`libmysqlclient21`) and a Pterodactyl-compatible
entrypoint. `amd64` and `arm64`.

It is not a standalone server. The egg's installer downloads the server files of
the chosen release (`MU_RELEASE_TAG`) into `/home/container`.

## Tags

| Tag | What it is |
|---|---|
| `0.97.40` | Current runtime; used by the eggs from 0.97.40 on |
| `latest` | The latest runtime |

The runtime only changes when the server's system dependencies change, so its
tag can be older than the server release.

## Use

Import the eggs from the release (`Pterodactyl-eggs-<version>.zip`, in the
[Releases](https://github.com/EmanuelCatania/Mu-Linux/releases)) and pick the
version in `MU_RELEASE_TAG`.

- [Repository](https://github.com/EmanuelCatania/Mu-Linux)
- [Documentation](https://mu-linux.com)
