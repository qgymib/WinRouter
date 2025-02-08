#ifndef WR_SERVICE_MESSAGE_HPP
#define WR_SERVICE_MESSAGE_HPP

#include <string>
#include <optional>
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
    int                           code;
    std::string                   message;
    std::optional<nlohmann::json> data;
};
template <typename T>
using RpcResult = Result<T, RpcError>;

struct Status
{
    static inline const char* NAME = "Status";

    struct Req
    {
        /**
         * @brief The process ID of who query the status.
         */
        int32_t pid;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Req, pid)
    };

    struct Rsp
    {
        /**
         * @brief The process ID.
         */
        int32_t pid;

        /**
         * @brief Is running in privileged mode.
         */
        bool IsPrivileged;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Rsp, IsPrivileged)
    };
};

} // namespace wr

#endif // WR_SERVICE_MESSAGE_HPP
