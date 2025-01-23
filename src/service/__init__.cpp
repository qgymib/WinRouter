#include "utils/win32.hpp"
#include "__init__.hpp"

#if 0
void wr::ServiceMode()
{
    const wchar_t* name = L"\\\\.\\pipe\\io.github.qgymib-WinRouter";
    const DWORD    dwOpenMode = PIPE_ACCESS_DUPLEX;
    const DWORD    dwPipeMode = PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT;
    const DWORD    nMaxInstances = PIPE_UNLIMITED_INSTANCES;
    const DWORD    nOutBufferSize = 4096;
    const DWORD    nInBufferSize = 4096;
    const DWORD    nDefaultTimeOut = 0;
    HANDLE         pipe = CreateNamedPipeW(name, dwOpenMode, dwPipeMode, nMaxInstances, nOutBufferSize, nInBufferSize,
                                           nDefaultTimeOut, nullptr);
    if (pipe == INVALID_HANDLE_VALUE)
    {
        throw std::exception("CreateNamedPipeW() failed");
    }
}
#endif

void wr::ServiceMode()
{

}
