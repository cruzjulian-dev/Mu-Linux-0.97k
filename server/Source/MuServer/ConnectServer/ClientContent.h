#pragma once

// Contenido del cliente que reparte el ConnectServer (los .toml de Content/).
//
// El operador los edita en Data/ClientContent/, con la misma estructura que el
// Content/ del cliente.  El ConnectServer los lee enteros, les calcula el
// SHA-256 y los sirve por el canal cifrado: el cliente compara el manifiesto
// con lo que tiene y pide los archivos que no coinciden (paquetes F4:10..13).
//
// La carpeta se vuelve a revisar cada 5 segundos: si algún archivo cambió
// (fecha, tamaño, uno nuevo o uno menos) se relee todo y el número de versión
// del manifiesto sube.  Un cliente que pide con una versión vieja recibe
// "cambió" y vuelve a empezar.  Sin la carpeta el manifiesto queda vacío.

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#define CLIENT_CONTENT_MAX_PATH 255

class CClientContent
{
public:

	struct Entry
	{
		std::string path;              // relativa a Content/, con '/'
		std::vector<BYTE> data;
		BYTE hash[32];
	};

	CClientContent();

	// Revisa la carpeta (como mucho cada 5 segundos, salvo force).
	void Refresh(bool force = false);

	// Versión actual del manifiesto (sube cada vez que cambia algún archivo).
	DWORD CurrentSerial();

	// Entradas desde `first` que entran en `budget` bytes del paquete, con el
	// formato de PMSG_CONTENT_MANIFEST_SEND.  Devuelve cuántas escribió, o -1 si
	// `serial` (0 = la actual) ya no es la versión actual.
	int WriteEntries(DWORD serial, int first, BYTE* out, int budget, int* written, int* total);

	// Pedazo de un archivo.  false si la versión o el índice no valen.
	bool ReadFile(DWORD serial, int index, DWORD offset, BYTE* out, int capacity, int* length, DWORD* size);

private:

	struct Stamp
	{
		std::string path;
		std::uintmax_t size;
		long long time;

		bool operator==(const Stamp& other) const
		{
			return path == other.path && size == other.size && time == other.time;
		}
	};

	std::vector<Stamp> Scan(const std::string& folder) const;

	std::mutex m_Lock;

	std::vector<Entry> m_Entries;

	std::vector<Stamp> m_Stamps;

	DWORD m_Serial;

	DWORD m_NextCheck;

	bool m_Checked;
};

extern CClientContent gClientContent;
