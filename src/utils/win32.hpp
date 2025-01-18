#ifndef WR_UTILS_WIN32_HPP
#define WR_UTILS_WIN32_HPP

#include <winsock2.h>
#include <windows.h>
#include <netioapi.h>
#include <string>
#include <vector>

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

class StringVec
{
public:
    typedef std::vector<std::string> StrVec;

public:
    StringVec();
    virtual ~StringVec();

public:
    StrVec::iterator       begin();
    StrVec::iterator       end();
    StrVec::const_iterator begin() const;
    StrVec::const_iterator end() const;

public:
    /**
     * @brief Inserts value at the end of the vector.
     * @param[in] str String to insert.
     * @return Self.
     */
    StringVec& PushBack(const std::string& str);

    /**
     * @brief Joins all the string list's strings into a single string with each element separated by the given \p
     * separator.
     * @param[in] separator Separator.
     * @return A string.
     */
    std::string Join(const std::string& separator) const;

private:
    StrVec m_vec;
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

} // namespace wr

#endif // WR_UTILS_WIN32_HPP
