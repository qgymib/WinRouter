#ifndef WR_UTILS_WIN32_HPP
#define WR_UTILS_WIN32_HPP

#include <wx/wx.h>
#include <winsock2.h>
#include <windows.h>
#include <netioapi.h>
#include <string>

namespace wr
{

/**
 * @brief Convert wide string to UTF-8 string.
 * @param[in] src   Wide string.
 * @return UTF-8 string.
 */
std::string ToString(const wchar_t* str);

/**
 * @brief Convert physical address to string.
 * @param[in] PhysicalAddress Physical address.
 * @param[in] PhysicalAddressLength Physical address length.
 * @return Physical address string.
 */
std::string ToString(const BYTE* PhysicalAddress, ULONG PhysicalAddressLength);

/**
 * @brief Convert SOCKET_ADDRESS to UTF-8 string.
 * @param[in] addr Socket address.
 * @return UTF-8 string.
 */
std::string ToString(const SOCKET_ADDRESS* addr);

/**
 * @brief Convert SOCKADDR_INET to UTF-8 string.
 * @param[in] addr Socket address.
 * @return UTF-8 string.
 */
std::string ToString(const SOCKADDR_INET* addr);

/**
 * @brief Convert ZoneIndices to UTF-8 string.
 * @param[in] ZoneIndices Zone Indices.
 * @param[in] ZoneIndicesLength Zone Indices Length.
 * @return UTF-8 string.
 */
std::string ToString(const ULONG* ZoneIndices, size_t ZoneIndicesLength);

/**
 * @brief Convert GUID to UTF-8 string.
 * @param[in] guid GUID.
 * @return UTF-8 string.
 */
std::string ToString(const GUID* guid);

} // namespace wr

#endif // WR_UTILS_WIN32_HPP
