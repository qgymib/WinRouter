#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <winsock2.h>
#include <windows.h>
#include <netioapi.h>
#include <iphlpapi.h>
#include <cinttypes>
#include "utils/win32.hpp"
#include "__init__.hpp"
#include "AdapterPanel.hpp"

struct DetailsPanel : public wxPanel
{
    explicit DetailsPanel(wxWindow* parent);
    wxStaticText* textAdapterName;
    wxStaticText* textLuid;
    wxStaticText* textDescription;
    wxStaticText* textPhysicalAddress;
};

DetailsPanel::DetailsPanel(wxWindow* parent) : wxPanel(parent)
{
    const int        flags = wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL;
    wxFlexGridSizer* fgs = new wxFlexGridSizer(2, 5, 5);

    fgs->Add(new wxStaticText(this, wxID_ANY, _("AdapterName")), 0, flags);
    textAdapterName = new wxStaticText(this, wxID_ANY, "");
    fgs->Add(textAdapterName);

    fgs->Add(new wxStaticText(this, wxID_ANY, "Luid"), 0, flags);
    textLuid = new wxStaticText(this, wxID_ANY, "");
    fgs->Add(textLuid);

    fgs->Add(new wxStaticText(this, wxID_ANY, _("Description")), 0, flags);
    textDescription = new wxStaticText(this, wxID_ANY, "");
    fgs->Add(textDescription);

    fgs->Add(new wxStaticText(this, wxID_ANY, _("PhysicalAddress")), 0, flags);
    textPhysicalAddress = new wxStaticText(this, wxID_ANY, "");
    fgs->Add(textPhysicalAddress);

    SetSizerAndFit(fgs);
}

struct wr::AdapterPanel::Data
{
    explicit Data(AdapterPanel* owner);
    void RefreshAdapter();
    void OnClickListBoxItem(wxCommandEvent&);

    AdapterPanel*              owner;
    wr::AdaptersAddresses::Ptr adapters;

    wxListBox*    listBox;
    DetailsPanel* detailsPanel;
    wxPanel*      identityPanel;
    wxPanel*      ipv4Panel;
    wxPanel*      ipv6Panel;
    wxPanel*      securityPanel;
};

wr::AdapterPanel::Data::Data(AdapterPanel* owner)
{
    this->owner = owner;

    wxSplitterWindow* splitter = new wxSplitterWindow(owner);
    splitter->SetMinimumPaneSize(100);

    /* Left */
    listBox = new wxListBox(splitter, WIDGET_ADAPTER_LIST_BOX);

    /* Right */
    wxPanel* rightPanel = new wxPanel(splitter);
    {
        wxBoxSizer* sizer2 = new wxBoxSizer(wxVERTICAL);
        wxNotebook* noteBook = new wxNotebook(rightPanel, wxID_ANY);
        detailsPanel = new DetailsPanel(noteBook);
        noteBook->AddPage(detailsPanel, _("Details"));
        identityPanel = new wxPanel(noteBook);
        noteBook->AddPage(identityPanel, _("Identity"));
        ipv4Panel = new wxPanel(noteBook);
        noteBook->AddPage(ipv4Panel, _("IPv4"));
        ipv6Panel = new wxPanel(noteBook);
        noteBook->AddPage(ipv6Panel, _("IPv6"));
        securityPanel = new wxPanel(noteBook);
        noteBook->AddPage(securityPanel, _("Security"));
        sizer2->Add(noteBook, 1, wxEXPAND);

        {
            wxBoxSizer* sizer3 = new wxBoxSizer(wxHORIZONTAL);
            sizer3->AddStretchSpacer();
            wxButton* saveButton = new wxButton(rightPanel, wxID_SAVE, _("Save"));
            sizer3->Add(saveButton);
            wxButton* cancelButton = new wxButton(rightPanel, wxID_CANCEL, _("Cancel"));
            sizer3->Add(cancelButton);
            sizer2->Add(sizer3, 0, wxEXPAND);
        }

        rightPanel->SetSizer(sizer2);
    }

    splitter->SplitVertically(listBox, rightPanel);
    splitter->SetSashPosition(200);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(splitter, 1, wxEXPAND);
    owner->SetSizer(mainSizer);

    owner->Bind(wxEVT_LISTBOX, &Data::OnClickListBoxItem, this, WIDGET_ADAPTER_LIST_BOX);

    RefreshAdapter();
}

void wr::AdapterPanel::Data::RefreshAdapter()
{
    adapters = std::make_shared<wr::AdaptersAddresses>();

    listBox->Clear();
    for (const auto addr : *adapters)
    {
        listBox->Append(addr->FriendlyName, addr);
    }
}

void wr::AdapterPanel::Data::OnClickListBoxItem(wxCommandEvent& e)
{
    const IP_ADAPTER_ADDRESSES* addr = static_cast<IP_ADAPTER_ADDRESSES*>(e.GetClientData());
    detailsPanel->textAdapterName->SetLabel(addr->AdapterName);
    detailsPanel->textLuid->SetLabel(wxString::Format("%" PRIu64, addr->Luid.Value));
    detailsPanel->textDescription->SetLabel(addr->Description);
    detailsPanel->textPhysicalAddress->SetLabel(wr::ToString(addr->PhysicalAddress, addr->PhysicalAddressLength));
}

wr::AdapterPanel::AdapterPanel(wxWindow* parent) : wxPanel(parent)
{
    m_data = new wr::AdapterPanel::Data(this);
}

wr::AdapterPanel::~AdapterPanel()
{
    delete m_data;
}
