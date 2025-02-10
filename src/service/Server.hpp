#ifndef WR_SERVICE_SERVER_HPP
#define WR_SERVICE_SERVER_HPP

#include "utils/Result.hpp"

namespace wr
{

class Server
{
public:
    explicit Server(const char* pipe);
    virtual ~Server();

public:
    /**
     * @brief Run server.
     * @return
     */
    Result<int, DWORD> Run();

private:
    struct Data;
    Data* m_data;
};

/**
 * @brief Start as service.
 */
void ServiceServer();

} // namespace wr

#endif // WR_SERVICE_SERVER_HPP
