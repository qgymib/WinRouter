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
    wxApp::OnInit();

    wxTranslations::Set(new wxTranslations);
    wxTranslations::Get()->SetLoader(new wxResourceTranslationsLoader);
    wxTranslations::Get()->AddAvailableCatalog("WinRouter");
    wxTranslations::Get()->SetLanguage(wxLANGUAGE_CHINESE);

    auto frame = new wr::MainFrame(nullptr);
    frame->SetIcon(wxIcon("IDI_ICON1"));
    frame->Show(true);
    return true;
}
