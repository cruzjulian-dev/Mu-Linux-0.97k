#pragma once

#include "CriticalSection.h"
#include "Queue.h"
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

// Un paquete del protocolo (C1/C2) entra en la cola del server: hasta 2048 bytes.
#define MAX_PACKET_SIZE 2048
// Los buffers del socket llevan además el registro del canal cifrado
// (SecureLink): un paquete más SecureChannel::Overhead.
#define MAX_MAIN_PACKET_SIZE 4096
#define MAX_SIDE_PACKET_SIZE 16384
#define MAX_SERVER_WORKER_THREAD 8
#define MAX_IO_OPERATION 2
#define IO_RECV 0
#define IO_SEND 1

struct IO_MAIN_BUFFER
{
	BYTE buff[MAX_MAIN_PACKET_SIZE];
	int size;
};

struct IO_SIDE_BUFFER
{
	BYTE buff[MAX_SIDE_PACKET_SIZE];
	int size;
};

struct IO_CONTEXT
{
	WSAOVERLAPPED overlapped;
	WSABUF wsabuf;
	int IoType;
	int IoSize;
};

struct IO_RECV_CONTEXT
{
	WSAOVERLAPPED overlapped;
	WSABUF wsabuf;
	int IoType;
	int IoSize;
	IO_MAIN_BUFFER IoMainBuffer;
};

struct IO_SEND_CONTEXT
{
	WSAOVERLAPPED overlapped;
	WSABUF wsabuf;
	int IoType;
	int IoSize;
	IO_MAIN_BUFFER IoMainBuffer;
	IO_SIDE_BUFFER IoSideBuffer;
};

class CSocketManager
{
public:

	CSocketManager();

	~CSocketManager();

	bool Start(WORD port);

	void Clean();

	bool CreateListenSocket();

	bool CreateCompletionPort();

	bool CreateAcceptThread();

	bool CreateWorkerThread();

	bool CreateServerQueue();

	bool DataRecv(int index, IO_MAIN_BUFFER* lpIoBuffer);

	// Paquete del protocolo: va cifrado si la conexión tiene canal (SecureLink).
	bool DataSend(int index, BYTE* lpMsg, int size);

	// Bytes tal cual al socket, sin cifrar: sólo para el saludo del canal.
	bool DataSendRaw(int index, const BYTE* data, int size);

	bool SendBytes(int index, const BYTE* data, int size);

	void Disconnect(int index);

	void OnRecv(int index, DWORD IoSize, IO_RECV_CONTEXT* lpIoContext);

	void OnSend(int index, DWORD IoSize, IO_SEND_CONTEXT* lpIoContext);

	static int CALLBACK ServerAcceptCondition(IN LPWSABUF lpCallerId, IN LPWSABUF lpCallerData, IN OUT LPQOS lpSQOS, IN OUT LPQOS lpGQOS, IN LPWSABUF lpCalleeId, OUT LPWSABUF lpCalleeData, OUT GROUP FAR* g, DWORD_PTR callbackData);

	static DWORD WINAPI ServerAcceptThread(CSocketManager* lpSocketManager);

	static DWORD WINAPI ServerWorkerThread(CSocketManager* lpSocketManager);

	static DWORD WINAPI ServerQueueThread(CSocketManager* lpSocketManager);

	DWORD GetQueueSize();

private:

	SOCKET m_listen;

	HANDLE m_CompletionPort;

	WORD m_port;

	HANDLE m_ServerAcceptThread;

	HANDLE m_ServerWorkerThread[MAX_SERVER_WORKER_THREAD];

	DWORD m_ServerWorkerThreadCount;

	CQueue m_ServerQueue;

	HANDLE m_ServerQueueSemaphore;

	HANDLE m_ServerQueueThread;

	CCriticalSection m_critical;

#ifndef _WIN32
	int m_epollFd;
	std::atomic<bool> m_running;
	std::thread m_acceptThread;
	std::vector<std::thread> m_workerThreads;
	std::thread m_queueThread;
	std::condition_variable m_queueCv;
	std::mutex m_queueMutex;
	bool m_queueStop;
#endif
};

extern CSocketManager gSocketManager;
