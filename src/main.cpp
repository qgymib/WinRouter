#include <wx/wx.h>
#include "widgets/MainFrame.hpp"

class MainApp final : public wxApp
{
public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(MainApp); // NOLINT

bool MainApp::OnInit()
{
    auto frame = new wr::MainFrame(nullptr);
    frame->SetIcon(wxIcon("IDI_ICON1"));
    frame->Show(true);
    return true;
}
