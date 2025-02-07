#ifndef WR_SERVICE_CLIENT_HPP
#define WR_SERVICE_CLIENT_HPP

#include <string>
#include <variant>
#include "Message.hpp"

namespace wr
{

template <typename T, typename E>
class Result
{
public:
    explicit Result(const T& r) : m_ok(true), m_data(r)
    {
    }
    explicit Result(const E& e) : m_ok(false), m_data(e)
    {
    }

    const T& unwrap() const&
    {
        if (!m_ok)
        {
            throw std::runtime_error("Result::unwrap() called on non-ok result");
        }
        return std::get<T>(m_data);
    }

    T unwrap() &&
    {
        if (!m_ok)
        {
            throw std::runtime_error("Result::unwrap() called on non-ok result");
        }
        return std::get<T>(std::move(m_data));
    }

private:
    bool               m_ok;
    std::variant<T, E> m_data;
};

class Client
{
public:
    Client();
    virtual ~Client();

public:
    bool IsConnected() const;

    template <typename T>
    DWORD Query(const typename T::Req& req, typename T::Rsp& rsp)
    {
        nlohmann::json jRsp;
        DWORD          dwRetVal = Query(T::NAME, req, jRsp);
        if (dwRetVal != 0)
        {
            return dwRetVal;
        }
    }

private:
    DWORD Query(const char* method, const nlohmann::json& params, nlohmann::json& response);

private:
    struct Data;
    Data* m_data;
};

} // namespace wr

#endif // WR_SERVICE_CLIENT_HPP
