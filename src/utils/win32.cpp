#include "win32.hpp"

wxString wr::ToString(const BYTE* PhysicalAddress, ULONG PhysicalAddressLength)
{
    wxString result;

    for (ULONG i = 0; i < PhysicalAddressLength; i++)
    {
        result += wxString::Format("%02X", PhysicalAddress[i]);
        if (i < PhysicalAddressLength - 1)
        {
            result += wxString::Format("-");
        }
    }

    return result;
}
