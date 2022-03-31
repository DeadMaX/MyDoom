#ifndef W_WAD_H
#define W_WAD_H

#include <array>
#include <cstdint>
#include <type_traits>
#include <vector>
#include <map>

#include <QByteArray>

#include "w_wad.h"

class WAD
{
    struct wadinfo_t
    {
        // Should be "IWAD" or "PWAD".
        std::array<char,4>	identification;
        std::uint32_t		numlumps;
        std::uint32_t		infotableofs;
    };
    static_assert(std::has_unique_object_representations_v<wadinfo_t>, "Invalid compiller");
    static_assert(sizeof(wadinfo_t) == 12, "Invalid compiller");

    struct filelump_t
    {
        std::uint32_t		filepos;
        std::uint32_t		size;
        std::array<char, 8>	name;

    };
    static_assert(std::has_unique_object_representations_v<filelump_t>, "Invalid compiller");
    static_assert(sizeof(filelump_t) == 16, "Invalid compiller");
    using lumpinfo_t = std::map<std::array<char, 8>, std::pair<const void*, std::uint32_t>>;

	void W_AddFile();
    lumpinfo_t::const_iterator W_CheckNumForName(const char* name) const;
    const void* W_CacheLumpNum(lumpinfo_t::const_iterator lump) const
    {
        return lump->second.first;
    }

    QByteArray m_wad_data;
    std::vector<filelump_t> m_fileinfo;
    lumpinfo_t m_lumpcache;

public:
    using wad_index_t = lumpinfo_t::const_iterator;

	WAD(const QByteArray&);

    bool W_LumpNameExists(const char* name) const;

    wad_index_t W_GetNumForName(const char* name) const;

    template <typename T, typename = std::enable_if_t<std::has_unique_object_representations_v<T>>>
    const T& W_CacheLumpName(const char *name) const
    {
        return *reinterpret_cast<const T*>(W_CacheLumpNum(W_GetNumForName(name)));
    }

    template <typename T, typename = std::enable_if_t<std::has_unique_object_representations_v<T>>>
    const T& W_CacheLumpName(const std::array<char, 8> &name) const
    {
        return *reinterpret_cast<const T*>(W_CacheLumpNum(W_GetNumForName(name.data())));
    }
};

inline bool operator<(const WAD::wad_index_t& lhs, const WAD::wad_index_t& rhs)
{
    return lhs->second.second < rhs->second.second;
}

inline WAD::wad_index_t::difference_type operator-(const WAD::wad_index_t& lhs, const WAD::wad_index_t& rhs)
{
    return lhs->second.second - rhs->second.second;
}

#endif
