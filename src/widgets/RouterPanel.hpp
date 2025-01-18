#ifndef WR_WIDGETS_ROUTER_PANEL_HPP
#define WR_WIDGETS_ROUTER_PANEL_HPP

#include <wx/wx.h>

namespace wr
{

class RouterPanel : public wxPanel
{
public:
    explicit RouterPanel(wxWindow* parent);
    ~RouterPanel() override;

private:
    struct Data;
    Data* m_data;
};

} // namespace wr

#endif // WR_WIDGETS_ROUTER_PANEL_HPP
