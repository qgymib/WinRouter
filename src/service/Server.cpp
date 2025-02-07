#include "utils/win32.hpp"
#include "Server.hpp"
#include "config.h"

struct Method
{
    const char* name;
};

struct ClientConnection
{
    typedef std::shared_ptr<ClientConnection> Ptr;

    explicit ClientConnection(HANDLE pipe);
    ~ClientConnection();

    static Ptr          Make(HANDLE pipe);
    static DWORD WINAPI ThreadProc(LPVOID lpParameter);

    bool   looping;
    HANDLE hPipe;
    HANDLE hThread;
    DWORD  dwThreadId;
};

struct ClientConnectionVec : public std::vector<ClientConnection::Ptr>
{
};

struct Server
{
    ClientConnectionVec connections;
};

ClientConnection::ClientConnection(HANDLE pipe)
{
    looping = true;
    hPipe = pipe;
    dwThreadId = 0;

    hThread = CreateThread(nullptr, 0, ThreadProc, this, 0, &dwThreadId);
    if (hThread == nullptr)
    {
        throw std::exception("Failed to create thread");
    }
}

ClientConnection::~ClientConnection()
{
    looping = false;
    if (hThread != INVALID_HANDLE_VALUE)
    {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        hThread = INVALID_HANDLE_VALUE;
    }

    if (hPipe != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
}

ClientConnection::Ptr ClientConnection::Make(HANDLE pipe)
{
    return std::make_shared<ClientConnection>(pipe);
}

DWORD ClientConnection::ThreadProc(LPVOID lpParameter)
{
    auto* pThis = static_cast<ClientConnection*>(lpParameter);

    while (pThis->looping)
    {
    }

    return 1;
}

void wr::ServiceServer()
{
    Server server;

    const std::wstring pipeName = wr::ToWideString(WR_SERVICE_PIPE_NAME);
    const DWORD        dwOpenMode = PIPE_ACCESS_DUPLEX;
    const DWORD        dwPipeMode = PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT;
    const DWORD        nMaxInstances = PIPE_UNLIMITED_INSTANCES;
    const DWORD        nOutBufferSize = 4096;
    const DWORD        nInBufferSize = 4096;
    const DWORD        nDefaultTimeOut = 0;

    for (;;)
    {
        HANDLE pipe = CreateNamedPipeW(pipeName.c_str(), dwOpenMode, dwPipeMode, nMaxInstances, nOutBufferSize,
                                       nInBufferSize, nDefaultTimeOut, nullptr);
        if (pipe == INVALID_HANDLE_VALUE)
        {
            throw std::exception("CreateNamedPipeW() failed");
        }

        BOOL fConnected = ConnectNamedPipe(pipe, nullptr) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        if (fConnected)
        {
        }
    }
}
