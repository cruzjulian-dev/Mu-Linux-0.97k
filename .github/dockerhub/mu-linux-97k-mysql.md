# MU Linux 0.97k — MySQL

MySQL 8.4 LTS with the MU Linux 0.97k schema and its initialization scripts
(`CreateDatabase.sql`, `Procedures.sql` and, optionally, test data).
`amd64` and `arm64`.

Part of [Mu-Linux](https://github.com/EmanuelCatania/Mu-Linux). Documentation at
[mu-linux.com](https://mu-linux.com).

## Tags

| Tag | What it is |
|---|---|
| `{{VERSION}}` | Schema of the {{VERSION}} release |
| `latest` | The latest release (currently `{{VERSION}}`) |

## Use

It is meant to run through the repository's compose file
(`docker-compose.images.yml`), next to the server and the web panel. The
scripts only run the first time, on an empty data volume.

| Variable | Use |
|---|---|
| `MYSQL_ROOT_PASSWORD`, `MYSQL_DATABASE`, `MYSQL_USER`, `MYSQL_PASSWORD` | Standard MySQL image variables |
| `SEED_TEST_DATA` | `1` loads the example accounts and characters |

Data is stored in utf8mb4. To upgrade an existing database (from MySQL 5.7 or
8.0, or from an older schema), follow the migration guides in the repository's
`docs/` folder and the *"Upgrading from …"* section of the README: a new image
does not touch an existing volume.

- [Repository](https://github.com/EmanuelCatania/Mu-Linux)
- [Releases](https://github.com/EmanuelCatania/Mu-Linux/releases)
