#pragma once

// Log de una sesión en User/ que se comporta como el MuError.log del original:
// se reescribe en cada arranque y, en Release, se borra al salir normalmente si
// no se anotó ningún error.  Si el cliente se cierra de golpe o crashea, queda.
// En Debug se conserva siempre.  debug.log tiene su propia versión en
// ClientEntry (escribe también a la salida del depurador).

#include "platform/UserPath.h"
#include <cstdio>
#include <mutex>
#include <vector>

namespace Platform {

class SessionLog {
public:
    explicit SessionLog(const char* name) : m_Name(name) {}

    // Una línea; `error` marca la sesión para que el archivo no se borre.
    void Write(const char* line, bool error)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_Finished) return;
        if (error) m_HadError = true;
        if (!m_File) {
            PrepareUserDir();
            UserPath(m_Path, m_Name);
            m_File = std::fopen(m_Path, "wb");
            if (!m_File) { m_Finished = true; return; }
            Registry().push_back(this);
        }
        std::fprintf(m_File, "%s\n", line);
        std::fflush(m_File);
    }

    // Salida normal: cierra y, en Release, borra si no hubo errores.
    static void FinishAll()
    {
        for (SessionLog* log : Registry()) log->Finish();
    }

private:
    void Finish()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Finished = true;
        if (!m_File) return;
        std::fclose(m_File);
        m_File = nullptr;
#if defined(NDEBUG)
        if (!m_HadError) std::remove(m_Path);
#endif
    }

    static std::vector<SessionLog*>& Registry()
    {
        static std::vector<SessionLog*> logs;
        return logs;
    }

    const char* m_Name;
    char m_Path[96] = {};
    FILE* m_File = nullptr;
    bool m_HadError = false;
    bool m_Finished = false;
    std::mutex m_Mutex;
};

} // namespace Platform
