#include "stdafx.h"
#include "assets/model/MonsterModelTable.h"
#include "assets/data/TomlLines.h"

#include <map>
#include <string>
#include <vector>

extern "C" void DbgLogPublic(const char* msg);

namespace {

constexpr int kFirstMonsterModel = 270;   // OpenMonsterModel(N) usa el slot 270 + N

void Warn(const std::string& message)
{
    DbgLogPublic(("MonsterModel: " + message).c_str());
}

bool ParseInts(const std::string& value, std::vector<int>& out)
{
    std::vector<std::string> items;
    if (!TomlLines::SplitArray(value, items)) return false;
    out.clear();
    for (const auto& item : items) {
        int v = 0;
        if (!TomlLines::ParseInt(item, v)) return false;
        out.push_back(v);
    }
    return true;
}

bool ParseFloats(const std::string& value, std::vector<float>& out)
{
    std::vector<std::string> items;
    if (!TomlLines::SplitArray(value, items)) return false;
    out.clear();
    for (const auto& item : items) {
        float v = 0;
        if (!TomlLines::ParseFloat(item, v)) return false;
        out.push_back(v);
    }
    return true;
}

// Número que sigue a `prefix` en la clave, hasta el próximo punto o el final.
bool KeyIndex(const std::string& key, const std::string& prefix, int& index, std::string* rest = nullptr)
{
    if (key.rfind(prefix, 0) != 0) return false;
    const auto dot = key.find('.', prefix.size());
    if (!TomlLines::ParseInt(key.substr(prefix.size(), dot == std::string::npos ? std::string::npos : dot - prefix.size()), index))
        return false;
    if (rest) *rest = dot == std::string::npos ? std::string() : key.substr(dot + 1);
    else if (dot != std::string::npos) return false;
    return true;
}

bool ParseSlotField(const std::string& key, const std::string& value, MonsterModelSlot& slot)
{
    if (key == "sounds") {
        std::vector<int> sounds;
        if (!ParseInts(value, sounds) || sounds.size() != 5) return false;
        for (int i = 0; i < 5; i++) slot.Sounds[i] = (short)sounds[i];
        slot.HasSounds = true;
        return true;
    }
    if (key == "bone_head") return slot.HasBoneHead = TomlLines::ParseInt(value, slot.BoneHead);
    if (key == "stream_mesh") return slot.HasStreamMesh = TomlLines::ParseInt(value, slot.StreamMesh);
    return false;
}

bool ParseTexture(int slot, const std::string& value, MonsterModelTexture& texture)
{
    std::vector<std::string> items;
    std::string filter, wrap;
    if (!TomlLines::SplitArray(value, items) || items.size() != 3
        || !TomlLines::ParseString(items[0], texture.File)
        || !TomlLines::ParseString(items[1], filter) || !TomlLines::ParseString(items[2], wrap))
        return false;
    if (filter == "nearest") texture.Filter = 0x2600;
    else if (filter == "linear") texture.Filter = 0x2601;
    else return false;
    if (wrap == "clamp") texture.Wrap = 0x2900;
    else if (wrap == "repeat") texture.Wrap = 0x2901;
    else return false;
    texture.Slot = slot;
    return true;
}

bool ParseModelField(int type, const std::string& key, const std::string& value, MonsterModel& model)
{
    int index = 0;
    std::string rest;
    if (key == "speed_scale") return model.HasSpeedScale = TomlLines::ParseFloat(value, model.SpeedScale);
    if (KeyIndex(key, "speed.", index)) {
        float speed = 0;
        if (!TomlLines::ParseFloat(value, speed)) return false;
        model.Speeds.emplace_back(index, speed);
        return true;
    }
    if (KeyIndex(key, "loop.", index)) {
        bool loop = false;
        if (!TomlLines::ParseBool(value, loop)) return false;
        if (loop) model.Loops.push_back(index);
        return true;
    }
    if (KeyIndex(key, "other.", index, &rest)) return ParseSlotField(rest, value, model.Slots[index]);
    if (KeyIndex(key, "texture.", index)) {
        model.Textures.emplace_back();
        return ParseTexture(index, value, model.Textures.back());
    }
    return ParseSlotField(key, value, model.Slots[kFirstMonsterModel + type]);
}

bool ParseDefaultField(const std::string& key, const std::string& value, MonsterModelDefaults& defaults)
{
    if (key == "speed") return ParseFloats(value, defaults.Speeds);
    if (key == "loop") return ParseInts(value, defaults.Loops);
    return false;
}

struct Table {
    MonsterModelDefaults        Defaults;
    std::map<int, MonsterModel> Models;
};

Table Read()
{
    Table table;
    std::vector<TomlLines::Line> lines, bad;
    if (!TomlLines::Read("Content/monster/models.toml", lines, &bad)) {
        Warn("no se encontró Content/monster/models.toml; los monstruos usan las velocidades por defecto del modelo");
        return table;
    }
    for (const auto& line : bad)
        Warn("línea " + std::to_string(line.number) + " inválida: " + line.value);
    for (const auto& line : lines) {
        int type = -1;
        bool ok = false;
        if (line.section == "default")
            ok = ParseDefaultField(line.key, line.value, table.Defaults);
        else if (line.section.rfind("model.", 0) == 0
                 && TomlLines::ParseInt(line.section.substr(6), type) && type >= 0)
            ok = ParseModelField(type, line.key, line.value, table.Models[type]);
        if (!ok)
            Warn("línea " + std::to_string(line.number) + " inválida: " + line.key + " = " + line.value);
    }
    return table;
}

const Table& Get()
{
    static const Table table = Read();
    return table;
}

} // namespace

const MonsterModelDefaults& MonsterModel_Defaults()
{
    return Get().Defaults;
}

const MonsterModel* MonsterModel_Get(int type)
{
    const auto& models = Get().Models;
    const auto found = models.find(type);
    return found == models.end() ? nullptr : &found->second;
}
