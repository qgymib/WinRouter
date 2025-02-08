#include <wx/wx.h>
#include <atomic>
#include "utils/win32.hpp"
#include "Session.hpp"
#include "Client.hpp"
#include "config.h"

struct wr::Client::Data
{
    Data();
    ~Data();

    RpcSession*           session;
    std::atomic<uint32_t> id;
};

wr::Client::Data::Data() : id(1)
{
    std::wstring pipeName = wr::ToWideString(WR_SERVICE_PIPE_NAME);
    const DWORD  dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
    HANDLE       hPipe = CreateFileW(pipeName.c_str(), dwDesiredAccess, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hPipe != INVALID_HANDLE_VALUE)
    {
        session = new RpcSession(hPipe);
    }
    else
    {
        session = nullptr;
    }
}

wr::Client::Data::~Data()
{
    if (session != nullptr)
    {
        delete session;
        session = nullptr;
    }
}

wr::Client::Client()
{
    m_data = new Data;
}

wr::Client::~Client()
{
    delete m_data;
}

static std::string ToRawData(const char* method, const nlohmann::json& params, uint32_t id)
{
    nlohmann::json request;
    request["jsonrpc"] = "2.0";
    request["method"] = method;
    request["params"] = params;
    request["id"] = id;

    return request.dump();
}

wr::RpcResult<nlohmann::json> wr::Client::Query(const char* method, const nlohmann::json& params)
{
    if (m_data->session == nullptr)
    {
        return RpcResult<nlohmann::json>::Err(RpcError{ ERROR_BROKEN_PIPE });
    }

    /* Send request */
    {
        std::string rawReqData = ToRawData(method, params, m_data->id++);
        const auto  result = m_data->session->Send(rawReqData);
        if (!result.IsOk())
        {
            DWORD code = result.UnwrapErr();
            return RpcResult<nlohmann::json>::Err(RpcError{ static_cast<int>(code) });
        }
    }

    auto result = m_data->session->Recv();
    if (!result.IsOk())
    {
        int code = result.UnwrapErr();
        return RpcResult<nlohmann::json>::Err(RpcError{ code });
    }

    std::string    content = result.Unwrap();
    nlohmann::json response = nlohmann::json::parse(content);
    if (response.is_discarded())
    {
        return RpcResult<nlohmann::json>::Err(RpcError{ ERROR_INVALID_DATA });
    }
    return RpcResult<nlohmann::json>::Ok(response);
}
