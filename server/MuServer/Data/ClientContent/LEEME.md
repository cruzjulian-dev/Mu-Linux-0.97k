# Data/ClientContent

Los `.toml` del cliente que reparte el ConnectServer. Va con la misma estructura
que la carpeta `Content/` del cliente: por ejemplo `interface/textures.toml`,
`item/models.toml`, `monster/models.toml`.

El ConnectServer lee todos los `.toml` de esta carpeta (y subcarpetas), les
calcula el SHA-256 y los manda por el canal cifrado (paquetes `F4:10`..`F4:13`).
El cliente compara esa lista con lo que tiene en su `Content/` y pide los que no
coinciden.

- Para cambiar algo del cliente, se edita el archivo acá. El ConnectServer
  revisa la carpeta cada 5 segundos y toma los cambios sin reiniciar.
- Un archivo que no está acá no se toca en el cliente.
- Sin esta carpeta, o vacía, no se reparte nada.
- Sólo se reparte por el canal cifrado: hace falta la clave del server en
  `Data/Keys/ServerIdentity.key` (la genera el GameServer al arrancar).

Para empezar se puede copiar acá el contenido de `Content/` del cliente que se
distribuye (sólo los `.toml`).

`Data/` es un volumen en Docker: en una instalación que ya existe esta carpeta
no aparece sola al actualizar la imagen, hay que crearla.
