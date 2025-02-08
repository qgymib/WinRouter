#ifndef WR_RUNTIME_HPP
#define WR_RUNTIME_HPP

#include "service/Client.hpp"

namespace wr
{

struct Runtime
{
    Runtime();
    ~Runtime();

    Client* client;
};

extern Runtime* G;

} // namespace wr

#endif // WR_RUNTIME_HPP
