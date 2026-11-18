#pragma once

// Listas de carga: qué archivo va en cada slot de modelo o de textura y en qué
// orden se cargan.  Cada archivo de Content tiene una sección por momento de
// carga, y cada línea es un slot:
//
//   slot = ["archivo.jpg", filtro, borde]       textura suelta (OpenJPG / OpenTGA)
//   slot = ["carpeta", "nombre", número]        modelo <carpeta><nombre><número>.bmd
//   slot = ["carpeta", "nombre", número, "textures"]   modelo y sus texturas
//   slot = "carpeta"                            texturas de un modelo ya cargado
//
// Las líneas se aplican en el orden del archivo.

// Carga la sección `section` de Content/<file>.
void AssetList_Load(const char* file, const char* section);

// Olvida los archivos ya leídos: la próxima carga los vuelve a leer (después
// de que la sincronización de la intro reemplaza alguno).
void AssetList_ClearCache();

// Texturas de la interfaz (Content/interface/textures.toml).
inline void TextureList_Load(const char* section)
{
    AssetList_Load("interface/textures.toml", section);
}
