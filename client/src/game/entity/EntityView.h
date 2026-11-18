#pragma once
#include "game/entity/EntityName.h"
#include "platform/PlatformHeaders.h"
#include <cstring>

// Vista mínima del layout legado (0x394); no posee ni modifica la entidad.
// IDA: RenderMonsterName (0x004CB6F0), ClearCharacters (layout del viewport).
class EntityView {
public:
    static constexpr int Capacity = 400;
    static constexpr int Stride = CHARACTER_STRIDE;
    static constexpr BYTE PlayerKind = 1;
    static constexpr BYTE MonsterKind = 2;
    static constexpr BYTE SoccerBall = 200;
    explicit EntityView(const void* entity) : m_Data((const BYTE*)entity) {}
    bool IsActive() const { return m_Data && m_Data[0] != 0; }
    BYTE Kind() const { return Character()->Object.Kind; }
    // IDA: CreateMonster (0x0045CCF0) escribe el tipo de Monster.txt como BYTE.
    BYTE MonsterIndex() const { return Character()->MonsterIndex; }
    // IDA: CreateChat (0x00481BA0), índice firmado; -1 indica sin guild.
    short GuildMarkIndex() const { return Character()->GuildMarkIndex; }
    WORD NetworkKey() const { return Character()->Key; }
    const char* Name() const { return EntityName::Get(m_Data); }
    float X() const { return Character()->Object.Position[0]; }
    float Y() const { return Character()->Object.Position[1]; }
    float Z() const { return Character()->Object.Position[2]; }
    float Height() const { return Character()->Object.BoundingBoxMax[2]; }
private:
    const CHARACTER* Character() const { return AsCharacter(m_Data); }
    const BYTE* m_Data;
};
