#ifndef WR_WIDGETS_DEBUG_PANEL_HPP
#define WR_WIDGETS_DEBUG_PANEL_HPP

#include <wx/wx.h>

namespace wr
{

class DebugPanel : public wxPanel
{
public:
    explicit DebugPanel(wxWindow* parent);
    ~DebugPanel() override;

private:
    struct Data;
    Data* m_data;
};

} // namespace wr

#endif // WR_WIDGETS_DEBUG_PANEL_HPP
