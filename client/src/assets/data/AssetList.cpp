#include "stdafx.h"
#include "assets/data/AssetList.h"
#include "assets/data/TomlLines.h"
#include "functions.h"

#include <cctype>
#include <map>
#include <string>
#include <vector>

extern "C" void DbgLogPublic(const char* msg);

namespace {

struct Asset {
    enum class Kind : unsigned char { Image, Model, ModelWithTextures, ModelTextures } Kind = Kind::Image;
    int         Slot = 0;
    std::string Path;       // Image: archivo; Model: carpeta; ModelTextures: carpeta
    std::string Name;       // Model: nombre del .bmd sin número
    int         Index = 0;  // Model: número del archivo
    int         Filter = 0; // Image
    int         Wrap = 0;   // Image
};

using Sections = std::map<std::string, std::vector<Asset>>;

void Warn(const std::string& message)
{
    DbgLogPublic(("AssetList: " + message).c_str());
}

bool ParseImage(const std::vector<std::string>& items, Asset& asset)
{
    std::string filter, wrap;
    if (items.size() != 3 || !TomlLines::ParseString(items[0], asset.Path)
        || !TomlLines::ParseString(items[1], filter) || !TomlLines::ParseString(items[2], wrap))
        return false;
    asset.Kind = Asset::Kind::Image;
    if (filter == "nearest") asset.Filter = 0x2600;
    else if (filter == "linear") asset.Filter = 0x2601;
    else return false;
    if (wrap == "clamp") asset.Wrap = 0x2900;
    else if (wrap == "repeat") asset.Wrap = 0x2901;
    else return false;
    return true;
}

bool ParseModel(const std::vector<std::string>& items, Asset& asset)
{
    if (items.size() != 3 && items.size() != 4) return false;
    if (!TomlLines::ParseString(items[0], asset.Path) || !TomlLines::ParseString(items[1], asset.Name)
        || !TomlLines::ParseInt(items[2], asset.Index))
        return false;
    asset.Kind = Asset::Kind::Model;
    if (items.size() == 4) {
        std::string option;
        if (!TomlLines::ParseString(items[3], option) || option != "textures") return false;
        asset.Kind = Asset::Kind::ModelWithTextures;
    }
    return true;
}

bool ParseAsset(const std::string& key, const std::string& value, Asset& asset)
{
    if (!TomlLines::ParseInt(key, asset.Slot) || asset.Slot < 0) return false;
    if (TomlLines::ParseString(value, asset.Path)) {
        asset.Kind = Asset::Kind::ModelTextures;
        return true;
    }
    std::vector<std::string> items;
    if (!TomlLines::SplitArray(value, items) || items.size() < 3) return false;
    int number = 0;
    // El tercer elemento distingue: número -> modelo, texto -> textura suelta.
    return TomlLines::ParseInt(items[2], number) ? ParseModel(items, asset) : ParseImage(items, asset);
}

Sections Read(const std::string& file)
{
    Sections sections;
    std::vector<TomlLines::Line> lines, bad;
    const std::string path = "Content/" + file;
    if (!TomlLines::Read(path.c_str(), lines, &bad)) {
        Warn("no se encontró " + path);
        return sections;
    }
    for (const auto& line : bad)
        Warn(file + ": línea " + std::to_string(line.number) + " inválida: " + line.value);
    for (const auto& line : lines) {
        Asset asset;
        if (line.section.empty() || !ParseAsset(line.key, line.value, asset))
            Warn(file + ": línea " + std::to_string(line.number) + " inválida: " + line.key + " = " + line.value);
        else
            sections[line.section].push_back(asset);
    }
    return sections;
}

bool IsTga(const std::string& file)
{
    const auto dot = file.rfind('.');
    if (dot == std::string::npos) return false;
    std::string ext = file.substr(dot);
    for (char& c : ext) c = (char)std::tolower((unsigned char)c);
    return ext == ".tga";
}

void Apply(const Asset& asset)
{
    switch (asset.Kind) {
    case Asset::Kind::Image:
        if (IsTga(asset.Path))
            OpenTGA(asset.Path.c_str(), asset.Slot, asset.Filter, asset.Wrap, 0, '\x01');
        else
            OpenJPG(asset.Path.c_str(), asset.Slot, asset.Filter, asset.Wrap, 0, '\x01');
        break;
    case Asset::Kind::Model:
        AccessModel(asset.Slot, asset.Path.c_str(), asset.Name.c_str(), asset.Index);
        break;
    case Asset::Kind::ModelWithTextures:
        AccessModelWithTextures(asset.Slot, const_cast<char*>(asset.Path.c_str()),
                                const_cast<char*>(asset.Name.c_str()), asset.Index);
        break;
    case Asset::Kind::ModelTextures:
        OpenTexture(asset.Slot, asset.Path.c_str(), 0x2600, '\x01');
        break;
    }
}

std::map<std::string, Sections> files;   // archivos ya leídos

} // namespace

void AssetList_ClearCache()
{
    files.clear();
}

void AssetList_Load(const char* file, const char* section)
{
    auto found = files.find(file);
    if (found == files.end()) found = files.emplace(file, Read(file)).first;
    const auto entries = found->second.find(section);
    if (entries == found->second.end()) {
        Warn(std::string(file) + ": sección [" + section + "] vacía o inexistente");
        return;
    }
    for (const Asset& asset : entries->second)
        Apply(asset);
}
