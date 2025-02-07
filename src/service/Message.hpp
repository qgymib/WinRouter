#ifndef WR_SERVICE_MESSAGE_HPP
#define WR_SERVICE_MESSAGE_HPP

#include <string>
#include <nlohmann/json.hpp>
#include "utils/Result.hpp"

namespace wr
{

struct RpcError
{
    /**
     * @brief Error code.
     *
     * List as:
     * + -32700: Parse error
     * + -32600: Invalid Request
     * + -32601: Method not found
     * + -32602: Invalid params
     * + -32603: Internal error
     * + -32000 to -32099: Server error
     * + >0: Pipe error, see GetLastError().
     */
    int         code;
    std::string message;
    std::string data;
};
template <typename T>
using RpcResult = Result<T, RpcError>;

struct IsPrivileged
{
  	static inline const char* NAME = "IsPrivileged";

    struct Req
    {
        /**
         * @brief Placeholder.
         */
        int32_t _;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Req, _)
    };

    struct Rsp
    {
        bool IsPrivileged;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Rsp, IsPrivileged)
    };
};

} // namespace wr

#endif // WR_SERVICE_MESSAGE_HPP
