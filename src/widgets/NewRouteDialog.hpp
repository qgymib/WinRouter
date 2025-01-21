#ifndef WR_WIDGET_NEW_ROUTE_DIALOG_HPP
#define WR_WIDGET_NEW_ROUTE_DIALOG_HPP

#include <wx/wx.h>
#include <cstdint>
#include <optional>

namespace wr
{

class NewRouteDialog final : public wxDialog
{
public:
    struct Result
    {
        Result();
        std::string         Destination;
        uint8_t             Netmask;
        std::string         Gateway;
        uint32_t            Metric;
        uint64_t            InterfaceLuid;
        std::optional<bool> Persistent;
    };

public:
    explicit NewRouteDialog(wxWindow* parent, bool ipv6);
    ~NewRouteDialog() override;

    /**
     * @brief Show dialog.
     * @param[out] result Result.
     * @return true if click OK
     */
    bool ShowModalAndGetResult(Result& result);

private:
    struct Data;
    Data* m_data;
};

} // namespace wr

#endif // WR_WIDGET_NEW_ROUTE_DIALOG_HPP
