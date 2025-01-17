#ifndef WR_WIDGETS_ADAPTER_PANEL_HPP
#define WR_WIDGETS_ADAPTER_PANEL_HPP

#include <wx/wx.h>

namespace wr
{

class AdapterPanel : public wxPanel
{
public:
    explicit AdapterPanel(wxWindow* parent);
    ~AdapterPanel() override;

private:
    struct Data;
    Data* m_data;
};

} // namespace wr

#endif // WR_WIDGETS_ADAPTER_PANEL_HPP
