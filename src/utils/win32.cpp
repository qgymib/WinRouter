#include <wx/wx.h>
#include "win32.hpp"
#include <netioapi.h>
#include <ws2tcpip.h>
#include <cassert>
#include <stdexcept>

static std::wstring GetCurrentExecutablePath()
{
    DWORD                buffSize = MAX_PATH;
    std::vector<wchar_t> buffer(buffSize);

    while (true)
    {
        DWORD length = GetModuleFileNameW(nullptr, buffer.data(), buffSize);
        if (length == 0)
        {
            throw std::runtime_error("GetModuleFileNameW failed");
        }

        if (length < buffSize)
        {
            return buffer.data();
        }

        buffSize *= 2;
        buffer.resize(buffSize);
    }
}

std::string wr::ToString(const wchar_t* str)
{
    const int len = WideCharToMultiByte(CP_UTF8, 0, str, -1, nullptr, 0, nullptr, nullptr);
    if (len == 0)
    {
        return "";
    }

    std::vector<char> buf(len);

    const int r = WideCharToMultiByte(CP_UTF8, 0, str, -1, buf.data(), len, nullptr, nullptr);
    if (r != len)
    {
        throw std::runtime_error("WideCharToMultiByte() failed");
    }

    return buf.data();
}

std::string wr::ToString(const BYTE* PhysicalAddress, ULONG PhysicalAddressLength)
{
    std::string result;

    for (ULONG i = 0; i < PhysicalAddressLength; i++)
    {
        char buff[8];
        snprintf(buff, sizeof(buff), "%02X", PhysicalAddress[i]);
        result += buff;

        if (i < PhysicalAddressLength - 1)
        {
            result += "-";
        }
    }

    return result;
}

std::string wr::ToString(const SOCKET_ADDRESS* addr)
{
    LPSOCKADDR sockaddr = addr->lpSockaddr;
    if (sockaddr == nullptr)
    {
        return "";
    }

    char buff[64];
    if (sockaddr->sa_family == AF_INET)
    {
        const SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(sockaddr);
        inet_ntop(AF_INET, &ipv4->sin_addr, buff, sizeof(buff));
    }
    else
    {
        const SOCKADDR_IN6* ipv6 = reinterpret_cast<SOCKADDR_IN6*>(sockaddr);
        inet_ntop(AF_INET6, &ipv6->sin6_addr, buff, sizeof(buff));
    }
    return buff;
}

std::string wr::ToString(const SOCKADDR_INET* addr)
{
    char buff[64];
    if (addr->si_family == AF_INET)
    {
        inet_ntop(AF_INET, &addr->Ipv4.sin_addr, buff, sizeof(buff));
    }
    else
    {
        inet_ntop(AF_INET6, &addr->Ipv6.sin6_addr, buff, sizeof(buff));
    }
    return buff;
}

std::string wr::ToString(const ULONG* ZoneIndices, size_t ZoneIndicesLength)
{
    std::string result;

    for (size_t i = 0; i < ZoneIndicesLength; i++)
    {
        char buff[8];
        snprintf(buff, sizeof(buff), "%lu", ZoneIndices[i]);
        result += buff;
        if (i < ZoneIndicesLength - 1)
        {
            result += "-";
        }
    }

    return result;
}

std::string wr::ToString(const GUID* guid)
{
    char guid_cstr[64];
    snprintf(guid_cstr, sizeof(guid_cstr), "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", guid->Data1,
             guid->Data2, guid->Data3, guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3], guid->Data4[4],
             guid->Data4[5], guid->Data4[6], guid->Data4[7]);
    return guid_cstr;
}

std::string wr::StringVec::Join(const std::string& separator) const
{
    std::string  result;
    const size_t vec_size = size();
    for (size_t i = 0; i < vec_size; i++)
    {
        result += at(i);
        if (i < vec_size - 1)
        {
            result += separator;
        }
    }
    return result;
}

bool wr::IsRunningAsAdmin()
{
    HANDLE hToken = nullptr;
    HANDLE currProc = GetCurrentProcess();
    if (!OpenProcessToken(currProc, TOKEN_QUERY, &hToken))
    {
        return false;
    }

    TOKEN_ELEVATION elevation;
    DWORD           cbSize = sizeof(TOKEN_ELEVATION);
    const bool      ret = GetTokenInformation(hToken, TokenElevation, &elevation, cbSize, &cbSize);
    CloseHandle(hToken);

    if (!ret)
    {
        return false;
    }

    return elevation.TokenIsElevated;
}

bool wr::RunAsAdmin()
{
    if (IsRunningAsAdmin())
    {
        return true;
    }

    const std::wstring path = GetCurrentExecutablePath();

    SHELLEXECUTEINFOW sei;
    ZeroMemory(&sei, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.lpVerb = L"runas";
    sei.lpFile = path.c_str();
    sei.hwnd = nullptr;
    sei.nShow = SW_NORMAL;

    return ShellExecuteExW(&sei);
}

wr::AdaptersAddresses::Iterator::Iterator(IP_ADAPTER_ADDRESSES* current)
{
    m_curr = current;
}

wr::AdaptersAddresses::Iterator& wr::AdaptersAddresses::Iterator::operator++()
{
    if (m_curr != nullptr)
    {
        m_curr = m_curr->Next;
    }
    return *this;
}

bool wr::AdaptersAddresses::Iterator::operator!=(const Iterator& other)
{
    return m_curr != other.m_curr;
}

IP_ADAPTER_ADDRESSES* wr::AdaptersAddresses::Iterator::operator*() const
{
    return m_curr;
}

wr::AdaptersAddresses::AdaptersAddresses()
{
    IP_ADAPTER_ADDRESSES* addr = nullptr;
    ULONG                 bufLen = static_cast<ULONG>(m_buff.size());
    DWORD                 dwRetVal = ERROR_BUFFER_OVERFLOW;
    const ULONG           flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_WINS_INFO | GAA_FLAG_INCLUDE_GATEWAYS |
                        GAA_FLAG_INCLUDE_ALL_INTERFACES | GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER;
    while (true)
    {
        addr = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(m_buff.data());
        dwRetVal = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, addr, &bufLen);
        if (dwRetVal != ERROR_BUFFER_OVERFLOW)
        {
            break;
        }
        m_buff.resize(bufLen);
    }
    m_addr = nullptr;

    if (dwRetVal == ERROR_NO_DATA)
    {
        return;
    }

    if (dwRetVal != ERROR_SUCCESS)
    {
        throw std::runtime_error("GetAdaptersAddresses() failed");
    }

    m_addr = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(m_buff.data());
}

wr::AdaptersAddresses::Iterator wr::AdaptersAddresses::begin()
{
    return m_addr;
}

wr::AdaptersAddresses::Iterator wr::AdaptersAddresses::end()
{
    return nullptr;
}

void wr::SystemErrorDialog(wxWindow* parent, DWORD errcode, const std::string& title)
{
    const wxString caption = wxString::FromUTF8(title);

    const DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
    wchar_t*    errMsg = nullptr;
    const DWORD dwRetVal = FormatMessageW(dwFlags, nullptr, errcode, 0, reinterpret_cast<LPWSTR>(&errMsg), 0, nullptr);
    if (dwRetVal == 0)
    {
        abort();
    }

    const wxString message = errMsg;
    LocalFree(errMsg);

    wxMessageBox(message, caption, wxOK | wxICON_ERROR, parent);
}
