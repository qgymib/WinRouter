#include <wx/wx.h>
#include <wx/valnum.h>
#include <wx/validate.h>
#include "utils/win32.hpp"
#include "NewRouteDialog.hpp"

struct InterfaceItem
{
    uint64_t    InterfaceLuid;
    std::string Ipv4Name;
    ULONG       Ipv4Metric;
    std::string Ipv6Name;
    ULONG       Ipv6Metric;
};

class InterfaceItemVec : public std::vector<InterfaceItem>
{
public:
    wxArrayString JoinName(bool ipv6);
};

wxArrayString InterfaceItemVec::JoinName(bool ipv6)
{
    wxArrayString result;

    for (auto it = begin(); it != end(); ++it)
    {
        wxString name = wxString::FromUTF8(ipv6 ? it->Ipv6Name : it->Ipv4Name);
        result.push_back(name);
    }

    return result;
}

wr::NewRouteDialog::Result::Result()
{
    Netmask = 0;
    Metric = 0;
    Persistent = false;
    InterfaceLuid = 0;
}

struct wr::NewRouteDialog::Data
{
    typedef wxIntegerValidator<uint32_t> U32Validator;

    explicit Data(NewRouteDialog* owner, bool ipv6);
    ~Data();

    NewRouteDialog*  owner;
    bool             ipv6;
    InterfaceItemVec interfaceVec;

    wxTextCtrl* textDestination;

    wxTextCtrl* textMask;
    uint32_t    valueMask;

    wxTextCtrl* textGateway;

    wxTextCtrl*   textMetric;
    U32Validator* validatorMetric;
    uint32_t      valueMetric;

    wxChoice* choiceInterface;
    wxChoice* choicePersistent;
    wxButton* buttonOk;
    wxButton* buttonCancel;
};

static std::string s_get_interface_unicast_addr(const IP_ADAPTER_ADDRESSES* addr, bool ipv6)
{
    const int family = ipv6 ? AF_INET6 : AF_INET;
    for (const IP_ADAPTER_UNICAST_ADDRESS_LH* item = addr->FirstUnicastAddress; item; item = item->Next)
    {
        if (item->Address.lpSockaddr->sa_family == family)
        {
            return wr::ToString(&item->Address);
        }
    }

    return ipv6 ? "::" : "0.0.0.0";
}

static InterfaceItemVec s_get_interfaces()
{
    InterfaceItemVec result;

    wr::AdaptersAddresses adapters;
    for (const auto addr : adapters)
    {
        std::string adapter = wr::ToString(addr->Description);

        std::string ipv4name;
        ipv4name += "[" + s_get_interface_unicast_addr(addr, false) + "] " + adapter;

        std::string ipv6name;
        ipv6name += "[" + s_get_interface_unicast_addr(addr, true) + "] " + adapter;

        InterfaceItem item = {
            addr->Luid.Value, ipv4name, addr->Ipv4Metric, ipv6name, addr->Ipv6Metric,
        };
        result.push_back(item);
    }

    return result;
}

wr::NewRouteDialog::Data::Data(NewRouteDialog* owner, bool ipv6)
{
    this->owner = owner;
    this->ipv6 = ipv6;
    interfaceVec = s_get_interfaces();
    valueMetric = 0;

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    {
        auto gridSizer = new wxFlexGridSizer(2, 5, 5);

        gridSizer->Add(new wxStaticText(owner, wxID_ANY, _("Destination")), 0, wxALL);
        textDestination = new wxTextCtrl(owner, wxID_ANY, "0.0.0.0");
        gridSizer->Add(textDestination, 0, wxALL);

        gridSizer->Add(new wxStaticText(owner, wxID_ANY, _("Mask")), 0, wxALL);
        textMask = new wxTextCtrl(owner, wxID_ANY);
        gridSizer->Add(textMask, 0, wxALL);

        gridSizer->Add(new wxStaticText(owner, wxID_ANY, _("Gateway")), 0, wxALL);
        textGateway = new wxTextCtrl(owner, wxID_ANY, "0.0.0.0");
        gridSizer->Add(textGateway, 0, wxALL);

        gridSizer->Add(new wxStaticText(owner, wxID_ANY, _("Metric")), 0, wxALL);
        validatorMetric = new U32Validator(&valueMetric);
        validatorMetric->SetRange(0, 65535);
        textMetric =
            new wxTextCtrl(owner, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, *validatorMetric);
        gridSizer->Add(textMetric, 0, wxALL);

        gridSizer->Add(new wxStaticText(owner, wxID_ANY, _("Interface")), 0, wxALL);
        choiceInterface = new wxChoice(owner, wxID_ANY, wxDefaultPosition, wxDefaultSize, interfaceVec.JoinName(ipv6));
        gridSizer->Add(choiceInterface, 0, wxALL);

        wxArrayString persistent = { _("Don't Change"), _("Yes"), _("No") };
        gridSizer->Add(new wxStaticText(owner, wxID_ANY, _("Persistent")), 0, wxALL);
        choicePersistent = new wxChoice(owner, wxID_ANY, wxDefaultPosition, wxDefaultSize, persistent);
        choicePersistent->SetSelection(0);
        gridSizer->Add(choicePersistent, 0, wxALL);

        mainSizer->Add(gridSizer, 1, wxEXPAND);
    }

    {
        wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

        buttonOk = new wxButton(owner, wxID_OK);
        buttonSizer->Add(buttonOk, 0, wxALL);
        buttonCancel = new wxButton(owner, wxID_CANCEL);
        buttonSizer->Add(buttonCancel, 0, wxALL);

        mainSizer->Add(buttonSizer, 0, wxEXPAND);
    }

    owner->SetSizer(mainSizer);
}

wr::NewRouteDialog::Data::~Data()
{
    delete validatorMetric;
}

wr::NewRouteDialog::NewRouteDialog(wxWindow* parent, bool ipv6) : wxDialog(parent, wxID_ANY, _("Add New Route"))
{
    m_data = new Data(this, ipv6);
}

wr::NewRouteDialog::~NewRouteDialog()
{
    delete m_data;
}

bool wr::NewRouteDialog::ShowModalAndGetResult(Result& result)
{
    const int ret = ShowModal();
    if (ret != wxID_OK)
    {
        return false;
    }

    int                 choice = m_data->choiceInterface->GetSelection();
    const InterfaceItem adapter = m_data->interfaceVec.at(choice);

    wxString value = m_data->textDestination->GetValue();
    result.Destination = value.ToStdString(wxConvUTF8);

    unsigned v = 0;
    if (m_data->textMask->GetValue().ToUInt(&v))
    {
        result.Netmask = v;
    }

    result.Gateway = m_data->textGateway->GetValue().ToStdString(wxConvUTF8);

    if (m_data->textMetric->GetValue().ToUInt(&v))
    {
        result.Metric = v - adapter.Ipv4Metric;
    }

    result.InterfaceLuid = adapter.InterfaceLuid;

    choice = m_data->choicePersistent->GetSelection();
    result.Persistent = (choice == 0) ? std::nullopt : std::make_optional(choice == 1);

    return true;
}
