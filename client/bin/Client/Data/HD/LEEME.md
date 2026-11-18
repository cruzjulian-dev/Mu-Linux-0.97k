# Pack de texturas HD

Pack opcional que el cliente usa con **Opciones → Pantalla → Avanzado →
Gráficos → Texturas HD** (se aplica al reiniciar). No reemplaza ningún archivo:
cada textura `Data/X/nombre.OZJ` se toma de `Data/HD/X/nombre.ozj.jpg` (o
`.ozj.png`) y cada `.OZT` de `Data/HD/X/nombre.ozt.png`, si existe y mide un
múltiplo entero del original; si no, se usa la original. Las OZJ van en JPEG
porque no tienen transparencia y así ocupan varias veces menos.

**El pack incluido es un reescalado de prueba**, hecho automáticamente con IA
(Real-ESRGAN, modelo UltraSharp, al doble) para mostrar cómo funciona la
opción. No es un remaster: algunas texturas salen mejor que otras, y las que
tienen brillos suaves o detalles finos pueden quedar con bordes o líneas que
el original no tenía (por eso se dejaron afuera los logos de MU). Cada
archivo se puede reemplazar por el que uno quiera, o borrar para volver a la
textura original.

Esta herramienta no está en el repo del cliente: el pack es un ejemplo y
cualquiera puede armar el suyo con otro reescalador o retocado a mano, con
tal de respetar los nombres y que el tamaño sea un múltiplo entero.

## Generarlo

Hace falta Python 3 con Pillow (`pip install Pillow`) y un reescalador
compatible con Real-ESRGAN ncnn-vulkan, por ejemplo el `upscayl-bin` de
[Upscayl](https://github.com/upscayl/upscayl), que ya trae modelos:

```
python generar_pack_hd.py --data bin/Client/Data --reescalador realesrgan \
    --exe "C:/Program Files/Upscayl/resources/bin/upscayl-bin.exe" \
    --modelos "C:/Program Files/Upscayl/resources/models" \
    --modelo ultrasharp-4x --escala 2
```

- `--escala 2` deja las texturas al doble (4 = cuádruple, 16 veces la memoria).
- `--modelo`: `ultrasharp-4x` da más detalle; `remacri-4x` es más natural.
- `--carpetas World1 Object1` limita a algunas carpetas (por defecto, todas,
  interfaz incluida). Los logos de MU (`Logo/Logo`, `Logo2`, `Mulogo_01`) se
  saltean: son brillos difusos y el reescalador los endurece.
- `--reescalador lanczos` amplía sin IA, sólo para probar el pack.
- Las que ya existen se saltean; `--forzar` las rehace.

La herramienta reescala de a una imagen: procesando una carpeta entera, el
reescalador a veces devuelve negro o cruza resultados entre texturas. Cada
resultado se reduce y se compara con su original; si no coincide se reintenta
y, si sigue fallando, se usa Lanczos.
