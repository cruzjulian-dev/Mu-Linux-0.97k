#include "stdafx.h"
#include "ConnectServerProtocol.h"
#include "ClientManager.h"
#include "ClientContent.h"
#include "SecureLink.h"
#include "ServerList.h"
#include "SocketManager.h"
#include "Util.h"

void ConnectServerProtocolCore(int index, BYTE head, BYTE* lpMsg, int size)
{
	ConsoleProtocolLog(CON_PROTO_TCP_RECV, lpMsg, size);

	gClientManager[index].m_PacketTime = GetTickCount();

	switch (head)
	{
		case 0xF4:
		{
			switch (lpMsg[3])
			{
				case 0x02:
				{
					CCCustomServerListSend(index);

					CCServerListRecv((PMSG_SERVER_LIST_RECV*)lpMsg, index);

					break;
				}

				case 0x03:
				{
					CCServerInfoRecv((PMSG_SERVER_INFO_RECV*)lpMsg, index);

					break;
				}

				case 0x10:
				{
					CCContentManifestRecv((PMSG_CONTENT_MANIFEST_RECV*)lpMsg, size, index);

					break;
				}

				case 0x12:
				{
					CCContentFileRecv((PMSG_CONTENT_FILE_RECV*)lpMsg, size, index);

					break;
				}
			}

			break;
		}
	}
}

void CCServerInitSend(int index, int result)
{
	PMSG_SERVER_INIT_SEND pMsg;

	pMsg.header.set(0x00, sizeof(pMsg));

	pMsg.result = result;

	gSocketManager.DataSend(index, (BYTE*)&pMsg, pMsg.header.size);
}

void CCCustomServerListSend(int index)
{
	BYTE send[2048];

	PMSG_CUSTOM_SERVER_LIST_SEND pMsg;

	pMsg.header.set(0xF4, 0x04, 0);

	int size = sizeof(pMsg);

	int count = gServerList.GenerateCustomServerList(send, &size);

	pMsg.count[0] = SET_NUMBERHB(count);

	pMsg.count[1] = SET_NUMBERLB(count);

	pMsg.header.size[0] = SET_NUMBERHB(size);

	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	gSocketManager.DataSend(index, send, size);
}

void CCServerListRecv(PMSG_SERVER_LIST_RECV* lpMsg, int index)
{
	BYTE send[2048];

	PMSG_SERVER_LIST_SEND pMsg;

	pMsg.header.set(0xF4, 0x02, 0);

	int size = sizeof(pMsg);

	int count = gServerList.GenerateServerList(send, &size);

	pMsg.count = count;

	pMsg.header.size[0] = SET_NUMBERHB(size);

	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	gSocketManager.DataSend(index, send, size);
}

void CCServerInfoRecv(PMSG_SERVER_INFO_RECV* lpMsg, int index)
{
	if (gServerList.CheckJoinServerState() == 0)
	{
		return;
	}

	SERVER_LIST_INFO* lpServerListInfo = gServerList.GetServerListInfo(lpMsg->ServerCode);

	if (lpServerListInfo == 0)
	{
		return;
	}

	if (lpServerListInfo->ServerShow == 0 || lpServerListInfo->ServerState == 0)
	{
		return;
	}

	PMSG_SERVER_INFO_SEND pMsg;

	pMsg.header.set(0xF4, 0x03, sizeof(pMsg));

	memcpy(pMsg.ServerAddress, lpServerListInfo->ServerAddress, sizeof(pMsg.ServerAddress));

	pMsg.ServerPort = lpServerListInfo->ServerPort;

	gSocketManager.DataSend(index, (BYTE*)&pMsg, pMsg.header.size);
}

void CCContentManifestRecv(PMSG_CONTENT_MANIFEST_RECV* lpMsg, int size, int index)
{
	if (size < (int)sizeof(PMSG_CONTENT_MANIFEST_RECV))
	{
		return;
	}

	BYTE send[MAX_PACKET_SIZE];

	PMSG_CONTENT_MANIFEST_SEND pMsg;

	memset(&pMsg, 0, sizeof(pMsg));

	int written = 0, total = 0;

	if (gSecureLink.IsSecure(index) == false)
	{
		pMsg.result = CONTENT_NOT_SECURE;
	}
	else
	{
		const int count = gClientContent.WriteEntries(lpMsg->serial, lpMsg->first, &send[sizeof(pMsg)], MAX_PACKET_SIZE - (int)sizeof(pMsg), &written, &total);

		if (count < 0)
		{
			pMsg.result = CONTENT_CHANGED;

			written = 0;
		}
		else
		{
			pMsg.result = CONTENT_OK;

			pMsg.count = (BYTE)count;
		}

		pMsg.serial = gClientContent.CurrentSerial();

		pMsg.total = (WORD)total;

		pMsg.first = lpMsg->first;
	}

	const int length = (int)sizeof(pMsg) + written;

	pMsg.header.set(0xF4, 0x11, (WORD)length);

	memcpy(send, &pMsg, sizeof(pMsg));

	gSocketManager.DataSend(index, send, length);
}

void CCContentFileRecv(PMSG_CONTENT_FILE_RECV* lpMsg, int size, int index)
{
	if (size < (int)sizeof(PMSG_CONTENT_FILE_RECV))
	{
		return;
	}

	BYTE send[sizeof(PMSG_CONTENT_FILE_SEND) + CONTENT_FILE_CHUNK];

	PMSG_CONTENT_FILE_SEND pMsg;

	memset(&pMsg, 0, sizeof(pMsg));

	pMsg.serial = lpMsg->serial;

	pMsg.index = lpMsg->index;

	pMsg.offset = lpMsg->offset;

	int length = 0;

	if (gSecureLink.IsSecure(index) == false)
	{
		pMsg.result = CONTENT_NOT_SECURE;
	}
	else if (lpMsg->serial != gClientContent.CurrentSerial())
	{
		pMsg.result = CONTENT_CHANGED;
	}
	else if (gClientContent.ReadFile(lpMsg->serial, lpMsg->index, lpMsg->offset, &send[sizeof(pMsg)], CONTENT_FILE_CHUNK, &length, &pMsg.size) == false)
	{
		// Puede haber cambiado entre las dos consultas: el cliente vuelve a pedir
		// el manifiesto en cualquiera de los dos casos.
		pMsg.result = (BYTE)((lpMsg->serial != gClientContent.CurrentSerial()) ? CONTENT_CHANGED : CONTENT_BAD_REQUEST);

		length = 0;
	}
	else
	{
		pMsg.result = CONTENT_OK;
	}

	pMsg.length = (WORD)length;

	const int total = (int)sizeof(pMsg) + length;

	pMsg.header.set(0xF4, 0x13, (WORD)total);

	memcpy(send, &pMsg, sizeof(pMsg));

	gSocketManager.DataSend(index, send, total);
}
