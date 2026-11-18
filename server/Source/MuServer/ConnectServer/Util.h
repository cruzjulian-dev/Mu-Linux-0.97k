#pragma once

#include "ServerDisplayer.h"
#include "UtilCommon.h"
#include <string>

void ConsoleProtocolLog(int type, BYTE* lpMsg, int size);

void ConnectServerTimeoutProc();

// Carpeta Data del server, con la barra final: la misma que usa el GameServer
// (MU_DATA_PATH, /opt/mu/Data o ../Data).
std::string GetDataPath();

int GetFreeClientIndex();

int SearchFreeClientIndex(int* index, int MinIndex, int MaxIndex, DWORD MinTime);

extern int gClientCount;
