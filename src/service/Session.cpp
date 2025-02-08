#include "Session.hpp"

struct wr::RpcSession::Data
{
    Data(HANDLE pipe);
    ~Data();

    DWORD Recv(std::string& content);

    HANDLE      hPipe;
    std::string cBuff;
};

wr::RpcSession::Data::Data(HANDLE pipe)
{
    this->hPipe = pipe;
}

wr::RpcSession::Data::~Data()
{
    if (hPipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
}

DWORD wr::RpcSession::Data::Recv(std::string& content)
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

wr::RpcSession::RpcSession(HANDLE pipe)
{
    m_data = new Data(pipe);
}

wr::RpcSession::~RpcSession()
{
    delete m_data;
}

wr::Result<std::string, DWORD> wr::RpcSession::Recv()
{
    std::string content;
    const DWORD dwRetVal = m_data->Recv(content);
    if (dwRetVal != 0)
    {
        return wr::Result<std::string, DWORD>::Err(dwRetVal);
    }
    return Result<std::string, DWORD>::Ok(content);
}

wr::Result<DWORD, DWORD> wr::RpcSession::Send(const std::string& msg)
{
    std::string data = "Content-Length: " + std::to_string(msg.size()) + "\r\n\r\n" + msg;

    DWORD numberOfBytesWritten = 0;
    BOOL  fSuccess = WriteFile(m_data->hPipe, data.c_str(), data.length(), &numberOfBytesWritten, nullptr);
    return fSuccess ? Result<DWORD, DWORD>::Ok(numberOfBytesWritten) : Result<DWORD, DWORD>::Err(GetLastError());
}
