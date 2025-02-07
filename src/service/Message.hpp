#ifndef WR_SERVICE_MESSAGE_HPP
#define WR_SERVICE_MESSAGE_HPP

#include <string>
#include <nlohmann/json.hpp>

namespace wr
{

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
        bool is_privileged;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Rsp, is_privileged)
    };
};

} // namespace wr

#endif // WR_SERVICE_MESSAGE_HPP
