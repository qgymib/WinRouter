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
    template <typename T>
    RpcResult<typename T::Rsp> Query(const typename T::Req& req)
    {
        RpcResult<nlohmann::json> rsp = Query(T::NAME, req);
        if (!rsp.IsOk())
        {
            return RpcResult<typename T::Rsp>::Err(rsp.UnwrapErr());
        }
        nlohmann::json jRsp = rsp.Unwrap();

        auto jError = jRsp.find("error");
        if (jError != jRsp.end())
        {
            int         code = jError->at("code");
            std::string message = jError->at("message");
            std::string data = jError->value("data", "");
            return RpcResult<typename T::Rsp>::Err(RpcError{ code, message, data });
        }

        nlohmann::json jResult = jRsp["result"];
        return RpcResult<typename T::Rsp>::Ok(jResult.get<typename T::Rsp>());
    }

private:
    RpcResult<nlohmann::json> Query(const char* method, const nlohmann::json& params);

private:
    struct Data;
    Data* m_data;
};

} // namespace wr

#endif // WR_SERVICE_CLIENT_HPP
