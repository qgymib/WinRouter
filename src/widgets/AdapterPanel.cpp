#include <winsock2.h>
#include <windows.h>
#include <netioapi.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include "AdapterPanel.hpp"

enum
{
    ITEMID_LISTBOX = wxID_HIGHEST + 1,
};

class DetailsPanel : public wxPanel
{
public:
    DetailsPanel(wxWindow* parent);

public:
};

DetailsPanel::DetailsPanel(wxWindow* parent) : wxPanel(parent)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    SetSizerAndFit(sizer);
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

    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

    /* Left */
    {
        listBox = new wxListBox(owner, ITEMID_LISTBOX);
        sizer->Add(listBox, 0, wxEXPAND | wxRIGHT);
    }

    /* Right */
    {
        wxBoxSizer* sizer2 = new wxBoxSizer(wxVERTICAL);

        wxNotebook* noteBook = new wxNotebook(owner, wxID_ANY);
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
            wxButton* saveButton = new wxButton(owner, wxID_SAVE, _("Save"));
            sizer3->Add(saveButton);
            wxButton* cancelButton = new wxButton(owner, wxID_CANCEL, _("Cancel"));
            sizer3->Add(cancelButton);
            sizer2->Add(sizer3, 0, wxEXPAND);
        }

        sizer->Add(sizer2, 1, wxEXPAND);
    }

    owner->SetSizerAndFit(sizer);
    owner->Bind(wxEVT_LISTBOX, &OnClickListBoxItem, this, ITEMID_LISTBOX);

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
    const int idx = e.GetInt();
    wxMessageBox(wxString::Format("OnClick %d", idx), "OnClick", wxOK | wxICON_INFORMATION);
}

wr::AdapterPanel::AdapterPanel(wxWindow* parent) : wxPanel(parent)
{
    m_data = new wr::AdapterPanel::Data(this);
}

wr::AdapterPanel::~AdapterPanel()
{
    delete m_data;
}
