#include "stdafx.h"
#include "MiniDump.h"
#include "ClientContent.h"
#include "SecureLink.h"
#include "ServerDisplayer.h"
#include "ServerList.h"
#include "SocketManager.h"
#include "SocketManagerUdp.h"
#include "Util.h"

long MaxIpConnection = 0;

int main()
{
	setlocale(LC_ALL, "C");

	CMiniDump::Start();

	gServerDisplayer.Init(nullptr);

	WSADATA wsa {};

	if (WSAStartup(MAKEWORD(2, 2), &wsa) == 0)
	{
		WORD ConnectServerPortTCP = GetPrivateProfileInt("ConnectServerInfo", "ConnectServerPortTCP", 44405, "./ConnectServer.ini");
		WORD ConnectServerPortUDP = GetPrivateProfileInt("ConnectServerInfo", "ConnectServerPortUDP", 55557, "./ConnectServer.ini");
		MaxIpConnection = GetPrivateProfileInt("ConnectServerInfo", "MaxIpConnection", 0, "./ConnectServer.ini");

		// La clave del canal cifrado (la genera el GameServer en Data/Keys).
		gSecureLink.Refresh();

		// Los .toml del cliente que se reparten por el canal (Data/ClientContent).
		gClientContent.Refresh(true);

		if (gSocketManager.Start(ConnectServerPortTCP) != 0)
		{
			if (gSocketManagerUdp.Start(ConnectServerPortUDP) != 0)
			{
				gServerList.Load("ServerList.dat");
			}
		}
	}
	else
	{
		LogAdd(LOG_RED, "WSAStartup() failed with error: %d", WSAGetLastError());
	}

	auto nextList = std::chrono::steady_clock::now();
	auto nextTimeout = std::chrono::steady_clock::now();

	while (true)
	{
		auto now = std::chrono::steady_clock::now();

		if (now >= nextList)
		{
			gServerList.MainProc();
			gSecureLink.Refresh();
			gClientContent.Refresh();
			nextList = now + std::chrono::seconds(1);
		}

		if (now >= nextTimeout)
		{
			ConnectServerTimeoutProc();
			nextTimeout = now + std::chrono::seconds(5);
		}

		Sleep(1);
	}

	CMiniDump::Clean();

	return 0;
}
