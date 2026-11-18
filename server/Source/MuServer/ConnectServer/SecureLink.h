#pragma once

// Canal cifrado con cada cliente del ConnectServer (Fase 5): el mismo
// Common/SecureChannel que usa el GameServer, con la misma clave de identidad
// (Data/Keys/ServerIdentity.key).  La clave la genera y la rota el GameServer;
// el ConnectServer sólo la lee, y la vuelve a leer si cambia (Refresh).
//
// Al aceptar una conexión:
//   - con la clave cargada, el server manda su saludo en claro y recién cuando
//     el canal queda listo manda la bienvenida (C1 04 00 01).  Todo lo demás va
//     cifrado.
//   - sin la clave (el GameServer todavía no la generó), la conexión sigue en
//     claro como antes, con la bienvenida de entrada.
//
// Todas las llamadas por conexión se hacen con el m_critical del SocketManager
// tomado.

#include "SecureChannel.h"
#include "SocketManager.h"
#include <memory>
#include <mutex>
#include <vector>

class CSecureLink
{
public:

	CSecureLink();

	// Carga o recarga la clave de Data/Keys.  Se llama al arrancar y cada segundo.
	void Refresh();

	// En el accept: decide si la conexión va cifrada y manda el saludo (o la
	// bienvenida, si va en claro).
	void Begin(int index);

	// Al desconectar: borra las claves de la conexión.
	void End(int index);

	bool IsSecure(int index) const;

	// Bytes crudos del socket de una conexión cifrada.  Descifra y le pasa el
	// texto claro a CSocketManager::DataRecv.  false = hay que desconectar.
	bool Receive(int index, const BYTE* raw, int size);

	// Cifra un paquete ya armado.  `out` necesita size + SecureChannel::Overhead.
	// Devuelve 0 si el canal todavía no terminó el saludo: el paquete no se manda.
	int Seal(int index, const BYTE* plain, int size, BYTE* out);

private:

	struct Link
	{
		SecureChannel channel;

		IO_MAIN_BUFFER plain;
	};

	Link* Get(int index) const;

	std::vector<std::unique_ptr<Link>> m_Links;

	std::vector<BYTE> m_Plain;

	std::mutex m_Identity;   // Refresh puede cambiar la clave mientras se acepta una conexión
};

extern CSecureLink gSecureLink;
