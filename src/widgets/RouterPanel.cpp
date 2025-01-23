#include <wx/wx.h>
#include <wx/grid.h>
#include <vector>
#include <cstdint>
#include <cinttypes>
#include "utils/win32.hpp"
#include "__init__.hpp"
#include "NewRouteDialog.hpp"
#include "RouterPanel.hpp"

#include <ws2tcpip.h>

struct ProtocolItem
{
    int         protocol;
    const char* name;
};

struct IpForwardRecord
{
    ULONG64     InterfaceLuid;
    std::string DestinationPrefix;
    UINT8       DestinationPrefixLength;
    std::string NextHop;
    ULONG       Metric;
    int         Protocol;
};
typedef std::vector<IpForwardRecord> IpForwardRecordVec;

struct AdapterAddressRecord
{
    ULONG64       Luid;
    wr::StringVec UnicastAddress;
    ULONG         Ipv4Metric;
    ULONG         Ipv6Metric;
    std::string   FriendlyName;
};
typedef std::vector<AdapterAddressRecord> AdapterAddressRecordVec;

class RouteGrid final : public wxGrid
{
public:
    RouteGrid(wxWindow* parent, wxWindowID id);
    void DrawCellHighlight(wxDC&, const wxGridCellAttr*) override;
};

struct wr::RouterPanel::Data
{
    explicit Data(RouterPanel* owner);
    ~Data();

    void Refresh();

    void OnGridIPv4Scrolled(const wxMouseEvent& e);
    void OnGridIPv4RightClick(const wxGridEvent& e);
    void OnGridIPv4NewRoute(const wxCommandEvent& e);
    void OnGridIPv4DeleteRoute(const wxCommandEvent& e);
    void OnGridIPv6Scrolled(const wxMouseEvent& e);
    void OnRefresh(const wxEvent& e);

    static void OnRouteChange(PVOID CallerContext, MIB_IPFORWARD_ROW2* Row, MIB_NOTIFICATION_TYPE NotificationType);

    RouterPanel* owner;
    HANDLE       routeChangeHandle;
    int          scrollStep;

    wxGrid*           gridIPv4;
    wxScrolledWindow* scrolledWindowIPv4;
    wxGrid*           gridIPv6;
    wxScrolledWindow* scrolledWindowIPv6;

    IpForwardRecordVec      ipv4;
    IpForwardRecordVec      ipv6;
    AdapterAddressRecordVec adapter;
};

namespace wr
{
wxDEFINE_EVENT(ROUTER_REFRESH, wxCommandEvent);
}

RouteGrid::RouteGrid(wxWindow* parent, wxWindowID id)
{
    Init();
    Create(parent, id);

    EnableGridLines(false);
    EnableEditing(false);
    SetRowLabelSize(0);
    EnableDragColSize(true);
    EnableDragRowSize(false);
    DisableDragGridSize();
}

void RouteGrid::DrawCellHighlight(wxDC&, const wxGridCellAttr*)
{
}

void wr::RouterPanel::Data::OnRouteChange(PVOID CallerContext, MIB_IPFORWARD_ROW2*, MIB_NOTIFICATION_TYPE)
{
    auto data = static_cast<Data*>(CallerContext);
    wxQueueEvent(data->owner, new wxCommandEvent(ROUTER_REFRESH));
}

wr::RouterPanel::Data::Data(RouterPanel* owner)
{
    this->owner = owner;
    scrollStep = 20;

    routeChangeHandle = INVALID_HANDLE_VALUE;
    DWORD dwRetVal = NotifyRouteChange2(AF_UNSPEC, &Data::OnRouteChange, this, false, &routeChangeHandle);
    if (dwRetVal != NO_ERROR)
    {
        wr::SystemErrorDialog(owner, dwRetVal, "NotifyRouteChange2()");
    }

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    {
        wxStaticBoxSizer* sizer2 = new wxStaticBoxSizer(wxVERTICAL, owner, "IPv4");
        scrolledWindowIPv4 = new wxScrolledWindow(sizer2->GetStaticBox(), wxID_ANY);
        scrolledWindowIPv4->SetScrollRate(scrollStep, scrollStep);
        gridIPv4 = new RouteGrid(scrolledWindowIPv4, wxID_ANY);
        gridIPv4->CreateGrid(0, 7, wxGrid::wxGridSelectRows);
        gridIPv4->SetColLabelValue(0, _("Destination"));
        gridIPv4->SetColLabelValue(1, _("NetMask"));
        gridIPv4->SetColLabelValue(2, _("Gateway"));
        gridIPv4->SetColLabelValue(3, _("Interface IP"));
        gridIPv4->SetColLabelValue(4, _("Metric"));
        gridIPv4->SetColLabelValue(5, _("Protocol"));
        gridIPv4->SetColLabelValue(6, _("Interface Name"));

        wxBoxSizer* scrollSizer = new wxBoxSizer(wxVERTICAL);
        scrollSizer->Add(gridIPv4, 1, wxEXPAND);
        scrolledWindowIPv4->SetSizer(scrollSizer);
        scrolledWindowIPv4->SetMinSize(wxSize(-1, 200));

        sizer2->Add(scrolledWindowIPv4, 1, wxEXPAND);
        sizer->Add(sizer2, 1, wxEXPAND);
    }

    {
        wxStaticBoxSizer* sizer3 = new wxStaticBoxSizer(wxVERTICAL, owner, "IPv6");
        scrolledWindowIPv6 = new wxScrolledWindow(sizer3->GetStaticBox(), wxID_ANY);
        scrolledWindowIPv6->SetScrollRate(scrollStep, scrollStep);
        gridIPv6 = new RouteGrid(scrolledWindowIPv6, wxID_ANY);
        gridIPv6->CreateGrid(0, 6, wxGrid::wxGridSelectRows);
        gridIPv6->SetColLabelValue(0, _("Destination"));
        gridIPv6->SetColLabelValue(1, _("Gateway"));
        gridIPv6->SetColLabelValue(2, _("Interface"));
        gridIPv6->SetColLabelValue(3, _("Metric"));
        gridIPv6->SetColLabelValue(4, _("Protocol"));
        gridIPv6->SetColLabelValue(5, _("Interface Name"));

        wxBoxSizer* scrollSizer = new wxBoxSizer(wxVERTICAL);
        scrollSizer->Add(gridIPv6, 1, wxEXPAND);
        scrolledWindowIPv6->SetSizer(scrollSizer);
        scrolledWindowIPv6->SetMinSize(wxSize(-1, 200));

        sizer3->Add(scrolledWindowIPv6, 1, wxEXPAND);
        sizer->Add(sizer3, 1, wxEXPAND);
    }

    owner->SetSizer(sizer);

    owner->Bind(ROUTER_REFRESH, &Data::OnRefresh, this);
    gridIPv4->Bind(wxEVT_MOUSEWHEEL, &Data::OnGridIPv4Scrolled, this);
    gridIPv4->Bind(wxEVT_GRID_CELL_RIGHT_CLICK, &Data::OnGridIPv4RightClick, this);
    gridIPv6->Bind(wxEVT_MOUSEWHEEL, &Data::OnGridIPv6Scrolled, this);

    Refresh();
}

wr::RouterPanel::Data::~Data()
{
    if (routeChangeHandle != INVALID_HANDLE_VALUE)
    {
        CancelMibChangeNotify2(routeChangeHandle);
        routeChangeHandle = INVALID_HANDLE_VALUE;
    }
}

void wr::RouterPanel::Data::OnGridIPv4Scrolled(const wxMouseEvent& e)
{
    const int scrollPos = scrolledWindowIPv4->GetScrollPos(wxVERTICAL);
    const int curPos = e.GetWheelRotation() > 0 ? scrollPos - scrollStep : scrollPos + scrollStep;
    scrolledWindowIPv4->Scroll(-1, curPos);
}

void wr::RouterPanel::Data::OnGridIPv4RightClick(const wxGridEvent& e)
{
    const int row = e.GetRow();
    const int col = e.GetCol();
    gridIPv4->ClearSelection();
    gridIPv4->SetGridCursor(row, col);
    gridIPv4->SelectRow(row);

    wxMenu menu;
    menu.Append(wxID_ADD);
    menu.Append(wxID_DELETE);
    menu.AppendSeparator();
    menu.Append(wxID_REFRESH);

    menu.Bind(wxEVT_MENU, &Data::OnGridIPv4NewRoute, this, wxID_ADD);
    menu.Bind(wxEVT_MENU, &Data::OnGridIPv4DeleteRoute, this, wxID_DELETE);
    menu.Bind(wxEVT_MENU, &Data::OnRefresh, this, wxID_REFRESH);

    gridIPv4->PopupMenu(&menu);
}

void wr::RouterPanel::Data::OnGridIPv4NewRoute(const wxCommandEvent&)
{
    wr::NewRouteDialog dialog(owner, false);

    wr::NewRouteDialog::Result result;
    if (!dialog.ShowModalAndGetResult(result))
    {
        return;
    }

    MIB_IPFORWARD_ROW2 route;
    InitializeIpForwardEntry(&route);

    route.DestinationPrefix.Prefix.si_family = AF_INET;
    inet_pton(AF_INET, result.Destination.c_str(), &(route.DestinationPrefix.Prefix.Ipv4.sin_addr));
    route.DestinationPrefix.PrefixLength = result.Netmask;

    route.NextHop.si_family = AF_INET;
    inet_pton(AF_INET, result.Gateway.c_str(), &(route.NextHop.Ipv4.sin_addr));

    route.InterfaceLuid.Value = result.InterfaceLuid;
    route.Metric = result.Metric;

    if (result.Persistent.has_value())
    {
        route.Immortal = result.Persistent.value();
    }

    DWORD dwRet = CreateIpForwardEntry2(&route);
    if (dwRet == 0)
    {
        Refresh();
        return;
    }

    wr::SystemErrorDialog(owner, dwRet, "CreateIpForwardEntry2()");
}

void wr::RouterPanel::Data::OnGridIPv4DeleteRoute(const wxCommandEvent&)
{
    const wxString msg = _("Do you want to delete the selected routing entry?");
    if (wxMessageBox(msg, _("Confirm"), wxYES_NO | wxICON_WARNING, owner) != wxYES)
    {
        return;
    }

    const int       row = gridIPv4->GetGridCursorRow();
    IpForwardRecord record = ipv4[row];

    MIB_IPFORWARD_ROW2 route;
    ZeroMemory(&route, sizeof(route));
    InitializeIpForwardEntry(&route);

    route.DestinationPrefix.Prefix.si_family = AF_INET;
    inet_pton(AF_INET, record.DestinationPrefix.c_str(), &(route.DestinationPrefix.Prefix.Ipv4.sin_addr));
    route.DestinationPrefix.PrefixLength = record.DestinationPrefixLength;

    route.NextHop.si_family = AF_INET;
    inet_pton(AF_INET, record.NextHop.c_str(), &(route.NextHop.Ipv4.sin_addr));

    route.InterfaceLuid.Value = record.InterfaceLuid;

    DWORD dwRet = DeleteIpForwardEntry2(&route);
    if (dwRet == 0)
    {
        Refresh();
        return;
    }

    wr::SystemErrorDialog(owner, dwRet, "DeleteIpForwardEntry2()");
}

void wr::RouterPanel::Data::OnGridIPv6Scrolled(const wxMouseEvent& e)
{
    const int scrollPos = scrolledWindowIPv6->GetScrollPos(wxVERTICAL);
    const int curPos = e.GetWheelRotation() > 0 ? scrollPos - scrollStep : scrollPos + scrollStep;
    scrolledWindowIPv6->Scroll(-1, curPos);
}

static void s_router_fetch_route(IpForwardRecordVec& ipv4, IpForwardRecordVec& ipv6)
{
    wr::Pointer<MIB_IPFORWARD_TABLE2*, void (*)(void*)> pIpForwardTable(FreeMibTable);
    const DWORD                                         ret = GetIpForwardTable2(AF_UNSPEC, &pIpForwardTable);
    if (ret != NO_ERROR)
    {
        throw std::runtime_error("GetIpForwardTable2() failed");
    }

    for (ULONG i = 0; i < pIpForwardTable->NumEntries; i++)
    {
        const MIB_IPFORWARD_ROW2* entry = &pIpForwardTable->Table[i];

        IpForwardRecord record = {
            entry->InterfaceLuid.Value,
            wr::ToString(&entry->DestinationPrefix.Prefix),
            entry->DestinationPrefix.PrefixLength,
            wr::ToString(&entry->NextHop),
            entry->Metric,
            entry->Protocol,
        };

        IpForwardRecordVec& vec = (entry->DestinationPrefix.Prefix.si_family == AF_INET) ? ipv4 : ipv6;
        vec.push_back(record);
    }
}

static void s_router_fetch_adapter(AdapterAddressRecordVec& vec)
{
    wr::AdaptersAddresses adapters;
    for (const auto addr : adapters)
    {
        wr::StringVec unicastAddr;
        for (const IP_ADAPTER_UNICAST_ADDRESS_LH* pUnicastAddr = addr->FirstUnicastAddress; pUnicastAddr;
             pUnicastAddr = pUnicastAddr->Next)
        {
            unicastAddr.push_back(wr::ToString(&pUnicastAddr->Address));
        }

        AdapterAddressRecord record = {
            addr->Luid.Value, unicastAddr, addr->Ipv4Metric, addr->Ipv6Metric, wr::ToString(addr->FriendlyName),
        };

        vec.push_back(record);
    }
}

static std::string s_prefix_length_to_ipv4_string(UINT8 DestinationPrefixLength)
{
    uint32_t mask = 0;
    if (DestinationPrefixLength > 0)
    {
        mask = (~0u) << (32 - DestinationPrefixLength);
    }
    return std::to_string((mask >> 24) & 0xFF) + "." + std::to_string((mask >> 16) & 0xFF) + "." +
           std::to_string((mask >> 8) & 0xFF) + "." + std::to_string(mask & 0xFF);
}

static AdapterAddressRecord s_get_interface(const AdapterAddressRecordVec& vec, ULONG64 luid)
{
    for (const auto& item : vec)
    {
        if (item.Luid == luid)
        {
            return item;
        }
    }

    throw std::runtime_error("No interface found.");
}

static std::string s_get_interface_ipv4(const AdapterAddressRecordVec& vec, ULONG64 luid)
{
    auto item = s_get_interface(vec, luid);

    for (const auto& addr : item.UnicastAddress)
    {
        if (addr.find(":") == std::string::npos)
        {
            return addr;
        }
    }
    return "";
}

static ULONG s_get_interface_metric(const AdapterAddressRecordVec& vec, ULONG64 luid, bool ipv6)
{
    for (const auto& item : vec)
    {
        if (item.Luid == luid)
        {
            return ipv6 ? item.Ipv6Metric : item.Ipv4Metric;
        }
    }
    return 0;
}

static std::string s_protocol_name(int protocol)
{
    static ProtocolItem protocols[] = {
        { MIB_IPPROTO_LOCAL,             "Local interface"                                           },
        { MIB_IPPROTO_NETMGMT,           "Static route"                                              },
        { MIB_IPPROTO_ICMP,              "ICMP redirect"                                             },
        { MIB_IPPROTO_EGP,               "Exterior Gateway Protocol"                                 },
        { MIB_IPPROTO_GGP,               "Gateway-to-Gateway Protocol"                               },
        { MIB_IPPROTO_HELLO,             "Hellospeak protocol"                                       },
        { MIB_IPPROTO_RIP,               "Berkeley Routing Information Protocol"                     },
        { MIB_IPPROTO_IS_IS,             "Intermediate System-to-Intermediate System"                },
        { MIB_IPPROTO_ES_IS,             "End System-to-Intermediate System"                         },
        { MIB_IPPROTO_CISCO,             "Cisco Interior Gateway Routing Protocol"                   },
        { MIB_IPPROTO_BBN,               "Bolt, Beranek, and Newman (BBN) Interior Gateway Protocol" },
        { MIB_IPPROTO_OSPF,              "Open Shortest Path First (OSPF)"                           },
        { MIB_IPPROTO_BGP,               "Border Gateway Protocol"                                   },
        { MIB_IPPROTO_NT_AUTOSTATIC,     "MIB_IPPROTO_NT_AUTOSTATIC"                                 },
        { MIB_IPPROTO_NT_STATIC,         "MIB_IPPROTO_NT_STATIC"                                     },
        { MIB_IPPROTO_NT_STATIC_NON_DOD, "MIB_IPPROTO_NT_STATIC_NON_DOD"                             }
    };

    for (const auto& item : protocols)
    {
        if (protocol == item.protocol)
        {
            return item.name;
        }
    }
    return "Unknown protocol";
}

static void s_update_ipv4(const AdapterAddressRecordVec& adapter, const IpForwardRecordVec& ipv4, wxGrid* gridIPv4)
{
    const int numberRows = gridIPv4->GetNumberRows();
    if (numberRows > 0)
    {
        gridIPv4->DeleteRows(0, numberRows);
    }

    const int ipv4_size = static_cast<int>(ipv4.size());
    gridIPv4->AppendRows(ipv4_size);
    for (int i = 0; i < ipv4_size; i++)
    {
        const auto&   item = ipv4[i];
        const ULONG64 luid = item.InterfaceLuid;

        gridIPv4->SetCellValue(i, 0, item.DestinationPrefix);
        gridIPv4->SetCellValue(i, 1, s_prefix_length_to_ipv4_string(item.DestinationPrefixLength));
        gridIPv4->SetCellValue(i, 2, item.NextHop);
        gridIPv4->SetCellValue(i, 3, s_get_interface_ipv4(adapter, luid));

        const ULONG metric = item.Metric + s_get_interface_metric(adapter, luid, false);
        gridIPv4->SetCellValue(i, 4, wxString::Format("%lu", metric));

        gridIPv4->SetCellValue(i, 5, s_protocol_name(item.Protocol));

        wxString friendlyName = wxString::FromUTF8(s_get_interface(adapter, luid).FriendlyName);
        gridIPv4->SetCellValue(i, 6, friendlyName);
    }
}

static void s_update_ipv6(const AdapterAddressRecordVec& adapter, const IpForwardRecordVec& ipv6, wxGrid* gridIPv6)
{
    const int numberRows = gridIPv6->GetNumberRows();
    if (numberRows > 0)
    {
        gridIPv6->DeleteRows(0, numberRows);
    }

    const int ipv6_size = static_cast<int>(ipv6.size());
    gridIPv6->AppendRows(ipv6_size);
    for (int i = 0; i < ipv6_size; i++)
    {
        const auto&   item = ipv6[i];
        const ULONG64 luid = item.InterfaceLuid;

        const unsigned DestinationPrefixLength = item.DestinationPrefixLength;
        const wxString dest = wxString::Format("%s/%u", item.DestinationPrefix, DestinationPrefixLength);
        gridIPv6->SetCellValue(i, 0, dest);
        gridIPv6->SetCellValue(i, 1, item.NextHop);
        gridIPv6->SetCellValue(i, 2, wxString::Format("%" PRIu64, luid));

        const ULONG metric = item.Metric + s_get_interface_metric(adapter, luid, false);
        gridIPv6->SetCellValue(i, 3, wxString::Format("%lu", metric));

        gridIPv6->SetCellValue(i, 4, s_protocol_name(item.Protocol));

        wxString friendlyName = wxString::FromUTF8(s_get_interface(adapter, luid).FriendlyName);
        gridIPv6->SetCellValue(i, 5, friendlyName);
    }
}

void wr::RouterPanel::Data::Refresh()
{
    ipv4.clear();
    ipv6.clear();
    adapter.clear();

    s_router_fetch_route(ipv4, ipv6);
    s_router_fetch_adapter(adapter);

    s_update_ipv4(adapter, ipv4, gridIPv4);
    s_update_ipv6(adapter, ipv6, gridIPv6);

    gridIPv4->AutoSizeColumns();
    gridIPv6->AutoSizeColumns();
}

void wr::RouterPanel::Data::OnRefresh(const wxEvent&)
{
    Refresh();
}

wr::RouterPanel::RouterPanel(wxWindow* parent) : wxPanel(parent)
{
    m_data = new Data(this);
}

wr::RouterPanel::~RouterPanel()
{
    delete m_data;
}
