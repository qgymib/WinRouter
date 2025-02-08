#ifndef WR_SERVICE_SESSION_HPP
#define WR_SERVICE_SESSION_HPP

#include "utils/win32.hpp"
#include "utils/Result.hpp"

namespace wr
{

class RpcSession
{
public:
    /**
     * @brief Create a RPC session.
     * @param[in] pipe Pipe handle. It takes the ownership of the pipe.
     */
    explicit RpcSession(HANDLE pipe);
    ~RpcSession();

    /**
     * @brief Receive data from pipe.
     * @return On success return the received data. On error return the error code.
     */
    Result<std::string, DWORD> Recv();

    /**
     * @brief Send data to peer.
     * @param[in] msg Data to send.
     * @return On success return the number of bytes sent. On error return the error code.
     */
    Result<DWORD, DWORD> Send(const std::string& msg);

private:
    struct Data;
    Data* m_data;
};

} // namespace wr

#endif // WR_SERVICE_SESSION_HPP
