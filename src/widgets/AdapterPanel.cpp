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
#include "AdapterPanel.hpp"

enum
{
    ITEMID_LISTBOX = wxID_HIGHEST + 1,
};

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

    AdapterPanel*  owner;
    wxMemoryBuffer adapterBuff;

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
    listBox = new wxListBox(splitter, ITEMID_LISTBOX);

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

    owner->Bind(wxEVT_LISTBOX, &Data::OnClickListBoxItem, this, ITEMID_LISTBOX);

    RefreshAdapter();
}

void wr::AdapterPanel::Data::RefreshAdapter()
{
    listBox->Clear();
    adapterBuff.Clear();

    IP_ADAPTER_ADDRESSES* addr = nullptr;
    ULONG                 bufLen = adapterBuff.GetBufSize();
    DWORD                 dwRetVal = ERROR_BUFFER_OVERFLOW;
    const ULONG           flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_WINS_INFO | GAA_FLAG_INCLUDE_GATEWAYS |
                        GAA_FLAG_INCLUDE_ALL_INTERFACES | GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER;
    while (true)
    {
        addr = static_cast<IP_ADAPTER_ADDRESSES*>(adapterBuff.GetData());
        dwRetVal = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, addr, &bufLen);
        if (dwRetVal != ERROR_BUFFER_OVERFLOW)
        {
            break;
        }
        adapterBuff.SetBufSize(bufLen);
    }
    if (dwRetVal == ERROR_NO_DATA)
    {
        return;
    }
    if (dwRetVal != ERROR_SUCCESS)
    {
        throw std::runtime_error("GetAdaptersAddresses failed");
    }

    for (; addr; addr = addr->Next)
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
