#include "win32.hpp"
#include <netioapi.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <cassert>

std::string wr::ToString(const wchar_t* str)
{
    std::string ret;

    int target_len = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    if (target_len == 0)
    {
        return ret;
    }

    char* buf = (char*)malloc(target_len);
    if (buf == NULL)
    {
        return ret;
    }

    int r = WideCharToMultiByte(CP_UTF8, 0, str, -1, buf, target_len, NULL, NULL);
    assert(r == target_len);
    (void)r;

    ret = buf;
    free(buf);

    return ret;
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

wr::StringVec::StringVec()
{
}

wr::StringVec::~StringVec()
{
}

wr::StringVec& wr::StringVec::PushBack(const std::string& str)
{
    m_vec.push_back(str);
    return *this;
}

std::string wr::StringVec::Join(const std::string& separator) const
{
    std::string result;
    const size_t vec_size = m_vec.size();
    for (size_t i = 0; i < vec_size; i++)
    {
        result += m_vec[i];
        if (i < vec_size - 1)
        {
            result += separator;
        }
    }
    return result;
}

wr::StringVec::StrVec::iterator wr::StringVec::begin()
{
    return m_vec.begin();
}

wr::StringVec::StrVec::iterator wr::StringVec::end()
{
    return m_vec.end();
}

wr::StringVec::StrVec::const_iterator wr::StringVec::begin() const
{
    return m_vec.begin();
}

wr::StringVec::StrVec::const_iterator wr::StringVec::end() const
{
    return m_vec.end();
}
