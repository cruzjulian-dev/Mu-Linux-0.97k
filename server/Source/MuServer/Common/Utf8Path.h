// Utf8Path.h — helpers to cross std::filesystem paths and std::string APIs.
//
// Keep paths crossing the string-based server APIs in UTF-8. In C++20 (and in
// newer libstdc++ versions with __cpp_char8_t), filesystem::path::u8string()
// returns std::u8string, which does not convert implicitly to std::string, and
// fs::u8path() is deprecated. These helpers do the conversion explicitly.
#pragma once

#include <filesystem>
#include <string>

inline std::filesystem::path Utf8Path(const std::string& value)
{
#if defined(__cpp_char8_t)
	return std::filesystem::path(std::u8string(reinterpret_cast<const char8_t*>(value.data()), value.size()));
#else
	return std::filesystem::u8path(value);
#endif
}

inline std::string PathToUtf8(const std::filesystem::path& path)
{
	auto value = path.u8string();
#if defined(__cpp_char8_t)
	return std::string(reinterpret_cast<const char*>(value.data()), value.size());
#else
	return value;
#endif
}
