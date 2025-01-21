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
    struct Data;
    Data* m_data;
};

} // namespace wr

#endif
