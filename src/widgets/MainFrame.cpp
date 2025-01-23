#include <wx/notebook.h>
#include <wx/aboutdlg.h>
#include "utils/win32.hpp"
#include "MainFrame.hpp"
#include "AdapterPanel.hpp"
#include "DebugPanel.hpp"
#include "RouterPanel.hpp"
#include "__init__.hpp"

struct wr::MainFrame::Data
{
    explicit Data(MainFrame* owner);
    void OnInstallAsService(const wxCommandEvent& e);
    void OnRunAsAdmin(const wxCommandEvent& e);
    void onShowLogWindow(const wxCommandEvent& e);

    MainFrame*   owner;
    wxLogWindow* logWindow;
};

static void OnAbout(wxCommandEvent&)
{
    wxAboutDialogInfo info;
    info.SetName(CMAKE_PROJECT_NAME);
    info.SetVersion(CMAKE_PROJECT_VERSION);
    info.SetIcon(wxIcon("IDI_ICON1"));

    wxAboutBox(info);
}

void wr::MainFrame::Data::OnInstallAsService(const wxCommandEvent&)
{
    wr::InstallAsService();
}

void wr::MainFrame::Data::OnRunAsAdmin(const wxCommandEvent&)
{
    if (wr::IsRunningAsAdmin())
    {
        return;
    }

    if (wr::RunAsAdmin())
    {
        owner->Close();
    }
}

void wr::MainFrame::Data::onShowLogWindow(const wxCommandEvent&)
{
    wxLogDebug("ShowLogWindow");
    logWindow->Show(true);
}

wr::MainFrame::Data::Data(MainFrame* owner)
{
    this->owner = owner;

    {
        logWindow = new wxLogWindow(owner, _("Log"), false);
        wxLog::SetActiveTarget(logWindow);
    }

    /* MenuBar */
    {
        wxMenu* menuHelp = new wxMenu;
        menuHelp->Append(WIDGET_MAIN_MENU_RUN_AS_ADMIN_MENU, _("Run as admin"));
        menuHelp->Append(WIDGET_MAIN_MENU_INSTALL_AS_SERVICE, _("Install as service"));
        menuHelp->Append(WIDGET_MAIN_MENU_SHOW_LOG, _("Log"));
        menuHelp->Append(wxID_ABOUT);

        wxMenuBar* menuBar = new wxMenuBar;
        menuBar->Append(menuHelp, _("Help"));
        owner->SetMenuBar(menuBar);
    }

    /* Body */
    {
        wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

        wxNotebook* noteBook = new wxNotebook(owner, wxID_ANY);
        noteBook->AddPage(new AdapterPanel(noteBook), _("Adapter"));
        noteBook->AddPage(new RouterPanel(noteBook), _("Router"));
        noteBook->AddPage(new DebugPanel(noteBook), _("Debug"));
        bSizer->Add(noteBook, 1, wxEXPAND);

        owner->SetSizerAndFit(bSizer);
    }

    /* StatusBar */
    {
        owner->CreateStatusBar();
        owner->SetStatusText("Welcome to wxWidgets!");
    }

    owner->Bind(wxEVT_MENU, &OnAbout, wxID_ABOUT);
    owner->Bind(wxEVT_MENU, &Data::OnInstallAsService, this, WIDGET_MAIN_MENU_INSTALL_AS_SERVICE);
    owner->Bind(wxEVT_MENU, &Data::OnRunAsAdmin, this, WIDGET_MAIN_MENU_RUN_AS_ADMIN_MENU);
    owner->Bind(wxEVT_MENU, &Data::onShowLogWindow, this, WIDGET_MAIN_MENU_SHOW_LOG);
}

wr::MainFrame::MainFrame(wxWindow* parent) : wxFrame(parent, wxID_ANY, CMAKE_PROJECT_NAME)
{
    m_data = new Data(this);
}

wr::MainFrame::~MainFrame()
{
    delete m_data;
}
