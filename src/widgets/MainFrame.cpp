#include <wx/notebook.h>
#include <wx/aboutdlg.h>
#include "MainFrame.hpp"
#include "AdapterPanel.hpp"
#include "DebugPanel.hpp"

static void OnAbout(wxCommandEvent&)
{
    wxAboutDialogInfo info;
    info.SetName(CMAKE_PROJECT_NAME);
    info.SetVersion(CMAKE_PROJECT_VERSION);
    info.SetIcon(wxIcon("IDI_ICON1"));

    wxAboutBox(info);
}

wr::MainFrame::MainFrame(wxWindow* parent) : wxFrame(parent, wxID_ANY, CMAKE_PROJECT_NAME)
{
    /* MenuBar */
    {
        wxMenu* menuHelp = new wxMenu;
        menuHelp->Append(wxID_ABOUT);

        wxMenuBar* menuBar = new wxMenuBar;
        menuBar->Append(menuHelp, _("Help"));
        SetMenuBar(menuBar);
    }

    /* Body */
    {
        wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

        wxNotebook* noteBook = new wxNotebook(this, wxID_ANY);
        noteBook->AddPage(new AdapterPanel(noteBook), _("Adapter"));
        noteBook->AddPage(new wxPanel(noteBook), _("Router"));
        noteBook->AddPage(new DebugPanel(noteBook), _("Debug"));
        bSizer->Add(noteBook, 1, wxEXPAND);

        SetSizerAndFit(bSizer);
    }

    /* StatusBar */
    {
        CreateStatusBar();
        SetStatusText("Welcome to wxWidgets!");
    }

    Bind(wxEVT_MENU, &OnAbout, wxID_ABOUT);
}

wr::MainFrame::~MainFrame()
{
}
