#include "w_wad.h"

#include <cctype>
#include <stdexcept>

WAD::WAD(const QByteArray& data)
	: m_wad_data(data)
{
	W_AddFile();
}

void WAD::W_AddFile()
{
	static const std::array<const std::array<char, 4>, 2> valid_header_type =
	{{
		{{'I', 'W', 'A', 'D'}},
		{{'P', 'W', 'A', 'D'}}
	}};

	wadinfo_t		header;
	std::memcpy(&header, m_wad_data.constData(), sizeof header);
	if (std::find(begin(valid_header_type), end(valid_header_type), header.identification) == end(valid_header_type))
		throw std::runtime_error("Invalid data file");
	m_fileinfo.resize(header.numlumps);
	std::memcpy(m_fileinfo.data(), m_wad_data.constData() + header.infotableofs, sizeof(filelump_t) * m_fileinfo.size());

	// load map
	std::uint32_t idx{ 0 };
	for (auto& i : m_fileinfo)
	{
		m_lumpcache[i.name] = std::make_pair(m_wad_data.constData() + i.filepos, idx++);
	}
}

bool WAD::W_LumpNameExists(const char* name) const
{
	std::array<char, 8> search{};

	for (size_t i = 0; name[i]; ++i)
		search[i] = std::toupper(name[i]);

	auto res = m_lumpcache.find(search);
	return res != std::end(m_lumpcache);
}

WAD::lumpinfo_t::const_iterator WAD::W_CheckNumForName(const char* name) const
{
	std::array<char, 8> key{};

	for (size_t i = 0; i < 8 && name[i]; ++i)
		key[i] = std::toupper(name[i]);

	return m_lumpcache.find(key);
}

WAD::lumpinfo_t::const_iterator WAD::W_GetNumForName(const char* name) const
{
	auto res = W_CheckNumForName(name);
	if (res == std::end(m_lumpcache))
	{
		// TFB. Not found.
		throw std::runtime_error("W_GetNumForName: %s not found!");
	}
	return res;
}
