#ifndef WR_UTILS_WIN32_HPP
#define WR_UTILS_WIN32_HPP

#include <wx/wx.h>

namespace wr
{

/**
 * @brief Convert physical address to string.
 * @param[in] PhysicalAddress Physical address.
 * @param[in] PhysicalAddressLength Physical address length.
 * @return Physical address string.
 */
wxString ToString(const BYTE* PhysicalAddress, ULONG PhysicalAddressLength);

}

#endif // WR_UTILS_WIN32_HPP
