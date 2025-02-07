#ifndef WR_SERVICE_CLIENT_HPP
#define WR_SERVICE_CLIENT_HPP

#include <string>
#include "Message.hpp"

namespace wr
{

class Client
{
public:
    Client();
    virtual ~Client();

public:
    bool IsConnected() const;

    template <typename T>
    RpcResult<typename T::Rsp> Query(const typename T::Req& req)
    {
        nlohmann::json jRsp;
        DWORD          dwRetVal = Query(T::NAME, req, jRsp);
        if (dwRetVal != 0)
        {
            return RpcError{ dwRetVal, "", "" };
        }

        auto jError = jRsp.find("error");
        if (jError != jRsp.end())
        {
            int         code = jError->at("code");
            std::string message = jError->at("message");
            std::string data = jError->value("data", "");
            return RpcError{ code, message, data };
        }

        nlohmann::json result = jRsp["result"];
        return result.template get<typename T::Rsp>();
    }

private:
    DWORD Query(const char* method, const nlohmann::json& params, nlohmann::json& response);

private:
    struct Data;
    Data* m_data;
};

} // namespace wr

#endif // WR_SERVICE_CLIENT_HPP
