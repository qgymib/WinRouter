#include <wx/wx.h>
#include <wx/cmdline.h>
#include "service/__init__.hpp"
#include "widgets/MainFrame.hpp"

class MainApp final : public wxApp
{
public:
    bool OnInit() override;
    void OnInitCmdLine(wxCmdLineParser& parser) override;
    bool OnCmdLineParsed(wxCmdLineParser& parser) override;
};

wxIMPLEMENT_APP(MainApp); // NOLINT

bool MainApp::OnInit()
{
    if (!wxApp::OnInit())
    {
        return false;
    }

    auto frame = new wr::MainFrame(nullptr);
    frame->SetIcon(wxIcon("IDI_ICON1"));
    frame->Show(true);
    return true;
}

void MainApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    static const wxCmdLineEntryDesc cmds[] = {
        {
         wxCMD_LINE_SWITCH, "s",
         "service", "Run in service mode.",
         wxCMD_LINE_VAL_NONE, wxCMD_LINE_HIDDEN,
         },
        { wxCMD_LINE_NONE },
    };

    parser.SetDesc(cmds);
    parser.SetSwitchChars(wxT("-"));

    wxApp::OnInitCmdLine(parser);
}

bool MainApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    const bool serviceMode = parser.Found("s");
    if (serviceMode)
    {
        wr::ServiceMode();
        return false;
    }

    return wxApp::OnCmdLineParsed(parser);
}
