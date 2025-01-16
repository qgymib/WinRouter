#include <wx/notebook.h>
#include <wx/aboutdlg.h>
#include "MainFrame.hpp"
#include "AdapterPanel.hpp"

static void OnAbout(wxCommandEvent&)
{
    wxAboutDialogInfo info;
    info.SetName(CMAKE_PROJECT_NAME);
    info.SetVersion(CMAKE_PROJECT_VERSION);

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
        wxPanel* adapterPanel = new wr::AdapterPanel(noteBook);
        noteBook->AddPage(adapterPanel, _("Adapter"));
        wxPanel* routerPanel = new wxPanel(noteBook);
        noteBook->AddPage(routerPanel, _("Router"));
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
