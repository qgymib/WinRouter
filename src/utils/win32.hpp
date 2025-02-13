#ifndef WR_UTILS_WIN32_HPP
#define WR_UTILS_WIN32_HPP

#include <winsock2.h>
#include <windows.h>
#include <netioapi.h>
#include <iphlpapi.h>
#include <string>
#include <vector>
#include <memory>

namespace wr
{

template <typename T>
class Pointer
{
public:
    typedef void (*TypedReleaseFn)(T*);
    typedef void (*VoidReleaseFn)(void*);

public:
    explicit Pointer(TypedReleaseFn fn)
    {
        m_ptr = nullptr;
        m_fn = fn;
    }
    Pointer(VoidReleaseFn fn)
    {
        m_ptr = nullptr;
        m_fn = fn;
    }
    virtual ~Pointer()
    {
        if (m_ptr != nullptr)
        {
            m_fn(m_ptr);
        }
    }

public:
    explicit Pointer(const Pointer& other) = delete;
    Pointer& operator=(const Pointer& other) = delete;

public:
    T* operator->()
    {
        return m_ptr;
    }

    T** operator&()
    {
        return &m_ptr;
    }

private:
    T*            m_ptr;
    VoidReleaseFn m_fn;
};

class StringVec : public std::vector<std::string>
{
public:
    /**
     * @brief Joins all the string list's strings into a single string with each element separated by the given \p
     * separator.
     * @param[in] separator Separator.
     * @return A string.
     */
    std::string Join(const std::string& separator) const;
};

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

/**
 * @brief Check if we are running as admin.
 * @return boolean.
 */
bool IsRunningAsAdmin();

/**
 * @brief Run self as admin.
 * @return boolean.
 */
bool RunAsAdmin();

class AdaptersAddresses
{
public:
    typedef std::shared_ptr<AdaptersAddresses> Ptr;

public:
    class Iterator
    {
    public:
        Iterator(IP_ADAPTER_ADDRESSES* current);
        Iterator&             operator++();
        bool                  operator!=(const Iterator& other);
        IP_ADAPTER_ADDRESSES* operator*() const;

    private:
        IP_ADAPTER_ADDRESSES* m_curr;
    };

public:
    AdaptersAddresses();
    Iterator begin();
    Iterator end();

public:
    AdaptersAddresses(const AdaptersAddresses& other) = delete;
    AdaptersAddresses& operator=(const AdaptersAddresses& other) = delete;

private:
    IP_ADAPTER_ADDRESSES* m_addr;
    std::vector<uint8_t>  m_buff;
};

/**
 * @brief Show system error dialog.
 * @param[in] parent Parent window.
 * @param[in] errcode System error code.
 * @param[in] title Dialog title.
 */
void SystemErrorDialog(wxWindow* parent, DWORD errcode, const std::string& title);

} // namespace wr

#endif // WR_UTILS_WIN32_HPP
