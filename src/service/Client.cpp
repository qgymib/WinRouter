#include <wx/wx.h>
#include <atomic>
#include "utils/win32.hpp"
#include "Client.hpp"
#include "config.h"

struct wr::Client::Data
{
    Data();
    ~Data();

    DWORD Recv(std::string& content);

    HANDLE                hPipe;
    std::string           cBuff;
    std::atomic<uint32_t> id;
};

wr::Client::Data::Data() : id(1)
{
    std::wstring pipeName = wr::ToWideString(WR_SERVICE_PIPE_NAME);
    const DWORD  dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
    hPipe = CreateFileW(pipeName.c_str(), dwDesiredAccess, 0, nullptr, OPEN_EXISTING, 0, nullptr);
}

wr::Client::Data::~Data()
{
    if (hPipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
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

    std::string reqData = request.dump();
    size_t      reqDataSize = reqData.size();
    return "Content-Length: " + std::to_string(reqDataSize) + "\r\n\r\n" + reqData;
}

DWORD wr::Client::Data::Recv(std::string& content)
{
    char buff[128];

    /* Read `Content-Length:` */
    while (cBuff.size() < 15)
    {
        DWORD bytesRead = 0;
        if (!ReadFile(hPipe, buff, sizeof(buff), &bytesRead, nullptr))
        {
            return GetLastError();
        }
        cBuff.append(buff, bytesRead);
    }
    /* Check and remove `Content-Length:` */
    if (strncmp(cBuff.data(), "Content-Length:", 15) != 0)
    {
        return ERROR_INVALID_DATA;
    }
    cBuff.erase(0, 15);

    /* Wait for line ending. */
    size_t breakPos = 0;
    while ((breakPos = cBuff.find("\r\n\r\n")) != std::string::npos)
    {
        DWORD bytesRead = 0;
        if (!ReadFile(hPipe, buff, sizeof(buff), &bytesRead, nullptr))
        {
            return GetLastError();
        }
        cBuff.append(buff, bytesRead);
    }
    /* Remove whitespace */
    while (cBuff.size() > 0 && isspace(cBuff[0]))
    {
        cBuff.erase(0, 1);
        breakPos--;
    }

    /* Get content length and remove the header */
    cBuff[breakPos] = '\0';
    unsigned contentLength = 0;
    if (sscanf(cBuff.data(), "%u", &contentLength) != 1)
    {
        return ERROR_INVALID_DATA;
    }
    cBuff.erase(0, breakPos + 4);

    /* Receive whole content */
    while (cBuff.size() < contentLength)
    {
        DWORD bytesRead = 0;
        if (!ReadFile(hPipe, buff, sizeof(buff), &bytesRead, nullptr))
        {
            return GetLastError();
        }
        cBuff.append(buff, bytesRead);
    }

    content = cBuff.substr(0, contentLength);
    cBuff.erase(0, contentLength);

    return 0;
}

wr::RpcResult<nlohmann::json> wr::Client::Query(const char* method, const nlohmann::json& params)
{
    std::string rawReqData = ToRawData(method, params, m_data->id++);

    if (m_data->hPipe == INVALID_HANDLE_VALUE)
    {
        return RpcResult<nlohmann::json>::Err(RpcError{ ERROR_BROKEN_PIPE });
    }

    DWORD dwRetVal = 0;
    BOOL  fSuccess = WriteFile(m_data->hPipe, rawReqData.c_str(), rawReqData.size(), &dwRetVal, nullptr);
    if (!fSuccess)
    {
        int code = GetLastError();
        return RpcResult<nlohmann::json>::Err(RpcError{ code });
    }

    std::string content;
    if ((dwRetVal = m_data->Recv(content)) != 0)
    {
        return RpcResult<nlohmann::json>::Err(RpcError{ static_cast<int>(dwRetVal) });
    }

    nlohmann::json response = nlohmann::json::parse(content);
    if (response.is_discarded())
    {
        return RpcResult<nlohmann::json>::Err(RpcError{ ERROR_INVALID_DATA });
    }
    return RpcResult<nlohmann::json>::Ok(response);
}
