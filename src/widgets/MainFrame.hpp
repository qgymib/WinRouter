#ifndef WR_WIDGETS_MAINFRAME_HPP
#define WR_WIDGETS_MAINFRAME_HPP

#include <wx/wx.h>

namespace wr
{

class MainFrame : public wxFrame
{
public:
    explicit MainFrame(wxWindow* parent);
    ~MainFrame() override;

private:
};

} // namespace wr

#endif
