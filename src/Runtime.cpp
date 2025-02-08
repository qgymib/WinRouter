#include "Runtime.hpp"

wr::Runtime* wr::G = nullptr;

wr::Runtime::Runtime()
{
    client = new Client;
}

wr::Runtime::~Runtime()
{
    if (client != nullptr)
    {
        delete client;
        client = nullptr;
    }
}
