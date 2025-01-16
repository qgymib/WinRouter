#include <wx/wx.h>
#include "widgets/MainFrame.hpp"

class MainApp : public wxApp
{
public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(MainApp);

bool MainApp::OnInit()
{
    auto frame = new wr::MainFrame(nullptr);
    frame->SetIcon(wxIcon("IDI_ICON1"));
    frame->Show(true);
    return true;
}
