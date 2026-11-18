# MU Linux 0.97k — web panel

Web panel for MU Linux 0.97k: registration, login, rankings, news and an admin
panel with editors for the server data (maps and spawns, custom items,
translations). Node.js 24. `amd64` and `arm64`.

Part of [Mu-Linux](https://github.com/EmanuelCatania/Mu-Linux). Documentation at
[mu-linux.com](https://mu-linux.com).

## Tags

| Tag | What it is |
|---|---|
| `{{VERSION}}` | Panel of the {{VERSION}} release |
| `latest` | The latest release (currently `{{VERSION}}`) |

## Use

It runs next to MySQL and, for the data editors, the editor API
(`mu-linux-97k-mu-editor`), through the repository's compose file
(`docker-compose.images.yml`). It listens on port `8080` inside the container.

| Variable | Use |
|---|---|
| `DB_HOST`, `DB_PORT`, `DB_USER`, `DB_PASS`, `DB_NAME` | MySQL connection |
| `SESSION_SECRET` | Session signing secret |
| `ADMIN_USER`, `ADMIN_PASS` | Admin panel account |
| `TURNSTILE_SITE_KEY`, `TURNSTILE_SECRET_KEY` | Cloudflare Turnstile (optional) |
| `EDITOR_ENABLED`, `EDITOR_API_URL` | Data editors through the editor API |
| `TRUST_PROXY`, `WEB_RATE_LIMIT_PER_MINUTE` | Behind a reverse proxy |
| `TZ`, `TIMEZONE` | Time zone |

The full list, with defaults, is in `.env.example` in the repository.

- [Repository](https://github.com/EmanuelCatania/Mu-Linux)
- [Releases](https://github.com/EmanuelCatania/Mu-Linux/releases)
