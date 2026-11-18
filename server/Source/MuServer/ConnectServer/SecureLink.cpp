#include "stdafx.h"
#include "SecureLink.h"
#include "ClientManager.h"
#include "ConnectServerProtocol.h"
#include "SecureRandom.h"
#include "ServerIdentity.h"
#include "Util.h"
#include <string>

CSecureLink gSecureLink;

namespace
{
	// La misma carpeta Data que usa el GameServer (CServerInfo::ReadInit).
	std::string KeyFolder()
	{
		return GetDataPath() + "Keys";
	}
}

CSecureLink::CSecureLink()
{
	this->m_Links.resize(MAX_CLIENT);
}

void CSecureLink::Refresh()
{
	std::lock_guard<std::mutex> lock(this->m_Identity);

	const bool wasLoaded = gServerIdentity.IsLoaded();

	gServerIdentity.Reload(KeyFolder().c_str());

	if (wasLoaded && !gServerIdentity.IsLoaded())
	{
		LogAdd(LOG_RED, "[SecureLink] Se perdio la clave de identidad");
	}
}

CSecureLink::Link* CSecureLink::Get(int index) const
{
	if (CLIENT_RANGE(index) == 0)
	{
		return nullptr;
	}

	return this->m_Links[index].get();
}

bool CSecureLink::IsSecure(int index) const
{
	return this->Get(index) != nullptr;
}

void CSecureLink::Begin(int index)
{
	if (CLIENT_RANGE(index) == 0)
	{
		return;
	}

	this->m_Links[index].reset();

	BYTE hello[SecureChannel::ServerHelloSize];

	{
		std::lock_guard<std::mutex> lock(this->m_Identity);

		BYTE random[SecureChannel::KeySize];

		// Sin la clave (el GameServer todavía no la generó) la conexión sigue en
		// claro, como antes de este canal.
		if (!gServerIdentity.IsLoaded() || !SecureRandomBytes(random, sizeof(random)))
		{
			CCServerInitSend(index, 1);

			return;
		}

		this->m_Links[index].reset(new Link());

		Link* link = this->m_Links[index].get();

		link->plain.size = 0;

		const bool rotation = gServerIdentity.HasRotation();

		link->channel.BeginServer(hello, random, gServerIdentity.SecretKey(), gServerIdentity.PublicKey(),
			rotation ? gServerIdentity.PreviousPublicKey() : nullptr,
			rotation ? gServerIdentity.RotationSignature() : nullptr);
	}

	gSocketManager.DataSendRaw(index, hello, sizeof(hello));
}

void CSecureLink::End(int index)
{
	if (CLIENT_RANGE(index) == 0)
	{
		return;
	}

	this->m_Links[index].reset();
}

bool CSecureLink::Receive(int index, const BYTE* raw, int size)
{
	Link* link = this->Get(index);

	if (link == nullptr)
	{
		return false;
	}

	const bool wasReady = link->channel.IsReady();

	this->m_Plain.clear();

	if (!link->channel.Receive(raw, (size_t)size, this->m_Plain))
	{
		LogAdd(LOG_RED, "[SecureLink] Canal rechazado (IP: %s, motivo %d)", gClientManager[index].m_IpAddr, (int)link->channel.GetFailure());

		return false;
	}

	// El saludo del cliente no lleva respuesta, pero si el canal la pidiera
	// saldría en claro antes que cualquier registro.
	BYTE reply[SecureChannel::ClientHelloSize];

	const size_t replySize = link->channel.TakeReply(reply);

	if (replySize != 0)
	{
		gSocketManager.DataSendRaw(index, reply, (int)replySize);
	}

	// El saludo terminó en esta lectura: recién ahora sale la bienvenida.
	if (!wasReady && link->channel.IsReady())
	{
		CCServerInitSend(index, 1);
	}

	// El texto claro pasa a DataRecv por el buffer de la conexión, de a lo que entre.
	size_t offset = 0;

	while (offset < this->m_Plain.size())
	{
		const int capacity = MAX_MAIN_PACKET_SIZE - link->plain.size;

		if (capacity <= 0)
		{
			return false;
		}

		const size_t remaining = this->m_Plain.size() - offset;

		const int take = (remaining < (size_t)capacity) ? (int)remaining : capacity;

		memcpy(&link->plain.buff[link->plain.size], &this->m_Plain[offset], take);

		link->plain.size += take;

		offset += take;

		if (gSocketManager.DataRecv(index, &link->plain) == false)
		{
			return false;
		}

		// Un paquete pudo cerrar la conexión: lo que quede ya no es de nadie.
		if (this->Get(index) != link || gClientManager[index].CheckState() == false)
		{
			break;
		}
	}

	return true;
}

int CSecureLink::Seal(int index, const BYTE* plain, int size, BYTE* out)
{
	Link* link = this->Get(index);

	if (link == nullptr || size < 0)
	{
		return 0;
	}

	return (int)link->channel.Seal(plain, (size_t)size, out);
}
