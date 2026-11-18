#include "stdafx.h"
#include "ClientContent.h"
#include "Sha256.h"
#include "Util.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>

CClientContent gClientContent;

namespace
{
	bool IsToml(const std::filesystem::path& path)
	{
		std::string ext = path.extension().string();

		std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return (char)std::tolower(c); });

		return ext == ".toml";
	}
}

CClientContent::CClientContent()
{
	this->m_Serial = 0;

	this->m_NextCheck = 0;

	this->m_Checked = false;
}

std::vector<CClientContent::Stamp> CClientContent::Scan(const std::string& folder) const
{
	std::vector<Stamp> stamps;

	std::error_code ec;

	if (!std::filesystem::is_directory(folder, ec))
	{
		return stamps;
	}

	for (std::filesystem::recursive_directory_iterator it(folder, ec), end; !ec && it != end; it.increment(ec))
	{
		if (!it->is_regular_file(ec) || !IsToml(it->path()))
		{
			continue;
		}

		Stamp stamp;

		stamp.path = std::filesystem::relative(it->path(), folder, ec).generic_string();

		if (ec || stamp.path.empty() || stamp.path.size() > CLIENT_CONTENT_MAX_PATH)
		{
			LogAdd(LOG_RED, "[ClientContent] Ruta invalida o demasiado larga: %s", it->path().generic_string().c_str());

			ec.clear();

			continue;
		}

		stamp.size = it->file_size(ec);

		stamp.time = (long long)it->last_write_time(ec).time_since_epoch().count();

		ec.clear();

		stamps.push_back(stamp);
	}

	std::sort(stamps.begin(), stamps.end(), [](const Stamp& a, const Stamp& b) { return a.path < b.path; });

	return stamps;
}

void CClientContent::Refresh(bool force)
{
	const DWORD now = GetTickCount();

	if (!force && this->m_Checked && (int)(now - this->m_NextCheck) < 0)
	{
		return;
	}

	this->m_NextCheck = now + 5000;

	const std::string folder = GetDataPath() + "ClientContent";

	std::vector<Stamp> stamps = this->Scan(folder);

	{
		std::lock_guard<std::mutex> lock(this->m_Lock);

		if (this->m_Checked && stamps == this->m_Stamps)
		{
			return;
		}
	}

	std::vector<Entry> entries;

	for (const Stamp& stamp : stamps)
	{
		std::ifstream file(folder + "/" + stamp.path, std::ios::binary);

		if (!file)
		{
			continue;
		}

		Entry entry;

		entry.path = stamp.path;

		entry.data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

		CSha256 sha;

		sha.Update(entry.data.data(), entry.data.size());

		sha.Final(entry.hash);

		entries.push_back(std::move(entry));
	}

	std::lock_guard<std::mutex> lock(this->m_Lock);

	this->m_Entries = std::move(entries);

	this->m_Stamps = std::move(stamps);

	this->m_Serial++;

	if (this->m_Serial == 0)
	{
		this->m_Serial = 1;
	}

	this->m_Checked = true;

	LogAdd(LOG_BLUE, "[ClientContent] %d archivos en %s (version %u)", (int)this->m_Entries.size(), folder.c_str(), this->m_Serial);
}

int CClientContent::WriteEntries(DWORD serial, int first, BYTE* out, int budget, int* written, int* total)
{
	std::lock_guard<std::mutex> lock(this->m_Lock);

	*written = 0;

	*total = (int)this->m_Entries.size();

	if (serial != 0 && serial != this->m_Serial)
	{
		return -1;
	}

	int count = 0;

	for (int n = first; n >= 0 && n < (int)this->m_Entries.size(); n++)
	{
		const Entry& entry = this->m_Entries[n];

		const int size = 1 + (int)entry.path.size() + 4 + 32;

		if (*written + size > budget)
		{
			break;
		}

		BYTE* p = out + *written;

		*p++ = (BYTE)entry.path.size();

		memcpy(p, entry.path.data(), entry.path.size());

		p += entry.path.size();

		const DWORD length = (DWORD)entry.data.size();

		memcpy(p, &length, 4);

		p += 4;

		memcpy(p, entry.hash, 32);

		*written += size;

		count++;
	}

	return count;
}

bool CClientContent::ReadFile(DWORD serial, int index, DWORD offset, BYTE* out, int capacity, int* length, DWORD* size)
{
	std::lock_guard<std::mutex> lock(this->m_Lock);

	if (serial != this->m_Serial || index < 0 || index >= (int)this->m_Entries.size())
	{
		return false;
	}

	const std::vector<BYTE>& data = this->m_Entries[index].data;

	*size = (DWORD)data.size();

	if (offset > *size)
	{
		return false;
	}

	*length = (int)std::min<size_t>((size_t)capacity, *size - offset);

	memcpy(out, data.data() + offset, *length);

	return true;
}

DWORD CClientContent::CurrentSerial()
{
	std::lock_guard<std::mutex> lock(this->m_Lock);

	return this->m_Serial;
}
