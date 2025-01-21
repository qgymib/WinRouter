#include <wx/wx.h>
#include <wx/clipbrd.h>
#include <nlohmann/json.hpp>
#include <vector>
#include "utils/win32.hpp"
#include "__init__.hpp"
#include "DebugPanel.hpp"

struct DebugFunc
{
    const char* name;
    std::string (*func)();
};
typedef std::vector<DebugFunc> DebugFuncVec;

struct wr::DebugPanel::Data
{
    explicit Data(DebugPanel* owner);
    void OnSelect(wxCommandEvent& e);
    void OnRefresh(wxCommandEvent& e);
    void OnCopyToClipboard(wxCommandEvent& e);
    void OnOpenURL(wxCommandEvent& e);
    void Refresh();

    DebugPanel* owner;

    DebugFuncVec funcs;
    DebugFunc*   currentFunc;

    wxComboBox* comboBox;
    wxTextCtrl* textData;
};

static std::string s_debug_get_adapters_addresses()
{
    wr::AdaptersAddresses adapters;
    nlohmann::json        json = nlohmann::json::array();
    for (const auto addr : adapters)
    {
        nlohmann::json item;
        item["IfIndex"] = addr->IfIndex;
        item["AdapterName"] = addr->AdapterName;

        nlohmann::json unicastJson = nlohmann::json::array();
        for (const IP_ADAPTER_UNICAST_ADDRESS_LH* pUnicastAddr = addr->FirstUnicastAddress; pUnicastAddr;
             pUnicastAddr = pUnicastAddr->Next)
        {
            nlohmann::json unicastItem;
            unicastItem["Address"] = wr::ToString(&pUnicastAddr->Address);
            unicastItem["PrefixOrigin"] = pUnicastAddr->PrefixOrigin;
            unicastItem["SuffixOrigin"] = pUnicastAddr->SuffixOrigin;
            unicastItem["DadState"] = pUnicastAddr->DadState;
            unicastItem["ValidLifetime"] = pUnicastAddr->ValidLifetime;
            unicastItem["PreferredLifetime"] = pUnicastAddr->PreferredLifetime;
            unicastItem["LeaseLifetime"] = pUnicastAddr->LeaseLifetime;
            unicastItem["OnLinkPrefixLength"] = pUnicastAddr->OnLinkPrefixLength;
            unicastJson.push_back(unicastItem);
        }
        item["UnicastAddresses"] = unicastJson;

        nlohmann::json anycastJson = nlohmann::json::array();
        for (const IP_ADAPTER_ANYCAST_ADDRESS_XP* pAnycastAddr = addr->FirstAnycastAddress; pAnycastAddr;
             pAnycastAddr = pAnycastAddr->Next)
        {
            nlohmann::json anycastItem;
            anycastItem["Address"] = wr::ToString(&pAnycastAddr->Address);
            anycastJson.push_back(anycastItem);
        }
        item["AnycastAddress"] = anycastJson;

        nlohmann::json multicastJson = nlohmann::json::array();
        for (const IP_ADAPTER_MULTICAST_ADDRESS_XP* multicastAddr = addr->FirstMulticastAddress; multicastAddr;
             multicastAddr = multicastAddr->Next)
        {
            nlohmann::json multicastItem;
            multicastItem["Address"] = wr::ToString(&multicastAddr->Address);
            multicastJson.push_back(multicastItem);
        }
        item["MulticastAddress"] = multicastJson;

        nlohmann::json dnsServerJson = nlohmann::json::array();
        for (const IP_ADAPTER_DNS_SERVER_ADDRESS_XP* dnsServerAddr = addr->FirstDnsServerAddress; dnsServerAddr;
             dnsServerAddr = dnsServerAddr->Next)
        {
            nlohmann::json dnsServerItem;
            dnsServerItem["Address"] = wr::ToString(&dnsServerAddr->Address);
            dnsServerJson.push_back(dnsServerItem);
        }
        item["DnsServerAddress"] = dnsServerJson;

        item["DnsSuffix"] = wr::ToString(addr->DnsSuffix);
        item["Description"] = wr::ToString(addr->Description);
        item["FriendlyName"] = wr::ToString(addr->FriendlyName);
        item["PhysicalAddress"] = wr::ToString(addr->PhysicalAddress, addr->PhysicalAddressLength);
        item["DdnsEnabled"] = static_cast<bool>(addr->DdnsEnabled);
        item["RegisterAdapterSuffix"] = static_cast<bool>(addr->RegisterAdapterSuffix);
        item["Dhcpv4Enabled"] = static_cast<bool>(addr->Dhcpv4Enabled);
        item["ReceiveOnly"] = static_cast<bool>(addr->ReceiveOnly);
        item["NoMulticast"] = static_cast<bool>(addr->NoMulticast);
        item["Ipv6OtherStatefulConfig"] = static_cast<bool>(addr->Ipv6OtherStatefulConfig);
        item["NetbiosOverTcpipEnabled"] = static_cast<bool>(addr->NetbiosOverTcpipEnabled);
        item["Ipv4Enabled"] = static_cast<bool>(addr->Ipv4Enabled);
        item["Ipv6Enabled"] = static_cast<bool>(addr->Ipv6Enabled);
        item["Ipv6ManagedAddressConfigurationSupported"] =
            static_cast<bool>(addr->Ipv6ManagedAddressConfigurationSupported);
        item["Mtu"] = addr->Mtu;
        item["IfType"] = addr->IfType;
        item["OperStatus"] = addr->OperStatus;
        item["Ipv6IfIndex"] = addr->Ipv6IfIndex;
        item["ZoneIndices"] = wr::ToString(addr->ZoneIndices, std::size(addr->ZoneIndices));

        nlohmann::json prefixJson = nlohmann::json::array();
        for (const IP_ADAPTER_PREFIX_XP* pPrefix = addr->FirstPrefix; pPrefix; pPrefix = pPrefix->Next)
        {
            nlohmann::json prefixItem;
            prefixItem["Address"] = wr::ToString(&pPrefix->Address);
            prefixItem["PrefixLength"] = pPrefix->PrefixLength;
            prefixJson.push_back(prefixItem);
        }
        item["Prefix"] = prefixJson;

        item["TransmitLinkSpeed"] = addr->TransmitLinkSpeed;
        item["ReceiveLinkSpeed"] = addr->ReceiveLinkSpeed;

        nlohmann::json winServerJson = nlohmann::json::array();
        for (const IP_ADAPTER_WINS_SERVER_ADDRESS_LH* pWinServerAddr = addr->FirstWinsServerAddress; pWinServerAddr;
             pWinServerAddr = pWinServerAddr->Next)
        {
            nlohmann::json winServerItem;
            winServerItem["Address"] = wr::ToString(&pWinServerAddr->Address);
            winServerJson.push_back(winServerItem);
        }
        item["WinServerAddress"] = winServerJson;

        nlohmann::json gatewayJson = nlohmann::json::array();
        for (const IP_ADAPTER_GATEWAY_ADDRESS_LH* pGatewayAddr = addr->FirstGatewayAddress; pGatewayAddr;
             pGatewayAddr = pGatewayAddr->Next)
        {
            nlohmann::json gatewayItem;
            gatewayItem["Address"] = wr::ToString(&pGatewayAddr->Address);
            gatewayJson.push_back(gatewayItem);
        }
        item["GatewayAddress"] = gatewayJson;

        item["Ipv4Metric"] = addr->Ipv4Metric;
        item["Ipv6Metric"] = addr->Ipv6Metric;
        item["Luid"] = addr->Luid.Value;
        item["Dhcpv4Server"] = wr::ToString(&addr->Dhcpv4Server);
        item["CompartmentId"] = addr->CompartmentId;
        item["NetworkGuid"] = wr::ToString(&addr->NetworkGuid);
        item["ConnectionType"] = addr->ConnectionType;
        item["TunnelType"] = addr->TunnelType;
        item["Dhcpv6Server"] = wr::ToString(&addr->Dhcpv6Server);
        item["Dhcpv6ClientDuid"] = wr::ToString(addr->Dhcpv6ClientDuid, addr->Dhcpv6ClientDuidLength);
        item["Dhcpv6Iaid"] = addr->Dhcpv6Iaid;

        json.push_back(item);
    }

    return json.dump(4);
}

static std::string s_debug_get_ip_forward_table2()
{
    wr::Pointer<MIB_IPFORWARD_TABLE2> pIpForwardTable(FreeMibTable);
    DWORD                             ret = GetIpForwardTable2(AF_UNSPEC, &pIpForwardTable);
    if (ret != NO_ERROR)
    {
        return "GetIpForwardTable2() failed.";
    }

    nlohmann::json json = nlohmann::json::array();
    for (size_t i = 0; i < pIpForwardTable->NumEntries; i++)
    {
        nlohmann::json            item;
        const MIB_IPFORWARD_ROW2* info = &pIpForwardTable->Table[i];
        item["InterfaceLuid"] = info->InterfaceLuid.Value;
        item["InterfaceIndex"] = info->InterfaceIndex;

        nlohmann::json destinationPrefixJson;
        destinationPrefixJson["Prefix"] = wr::ToString(&info->DestinationPrefix.Prefix);
        destinationPrefixJson["PrefixLength"] = info->DestinationPrefix.PrefixLength;
        item["DestinationPrefix"] = destinationPrefixJson;

        item["NextHop"] = wr::ToString(&info->NextHop);
        item["SitePrefixLength"] = info->SitePrefixLength;
        item["ValidLifetime"] = info->ValidLifetime;
        item["PreferredLifetime"] = info->PreferredLifetime;
        item["Metric"] = info->Metric;
        item["Protocol"] = info->Protocol;
        item["Loopback"] = static_cast<bool>(info->Loopback);
        item["AutoconfigureAddress"] = static_cast<bool>(info->AutoconfigureAddress);
        item["Publish"] = static_cast<bool>(info->Publish);
        item["Immortal"] = static_cast<bool>(info->Immortal);
        item["Age"] = info->Age;
        item["Origin"] = info->Origin;

        json.push_back(item);
    }

    return json.dump(4);
}

static std::string s_debug_get_ip_interface_table()
{
    wr::Pointer<MIB_IPINTERFACE_TABLE> pipTable(FreeMibTable);
    DWORD                              ret = GetIpInterfaceTable(AF_UNSPEC, &pipTable);
    if (ret != NO_ERROR)
    {
        return "GetIpInterfaceTable() failed.";
    }

    nlohmann::json json = nlohmann::json::array();
    for (size_t i = 0; i < pipTable->NumEntries; i++)
    {
        nlohmann::json             item;
        const MIB_IPINTERFACE_ROW* entry = &pipTable->Table[i];

        item["Family"] = entry->Family;
        item["InterfaceLuid"] = entry->InterfaceLuid.Value;
        item["InterfaceIndex"] = entry->InterfaceIndex;
        item["MaxReassemblySize"] = entry->MaxReassemblySize;
        item["InterfaceIdentifier"] = entry->InterfaceIdentifier;
        item["MinRouterAdvertisementInterval"] = entry->MinRouterAdvertisementInterval;
        item["MaxRouterAdvertisementInterval"] = entry->MaxRouterAdvertisementInterval;
        item["AdvertisingEnabled"] = static_cast<bool>(entry->AdvertisingEnabled);
        item["ForwardingEnabled"] = static_cast<bool>(entry->ForwardingEnabled);
        item["WeakHostSend"] = static_cast<bool>(entry->WeakHostSend);
        item["WeakHostReceive"] = static_cast<bool>(entry->WeakHostReceive);
        item["UseAutomaticMetric"] = static_cast<bool>(entry->UseAutomaticMetric);
        item["UseNeighborUnreachabilityDetection"] = static_cast<bool>(entry->UseNeighborUnreachabilityDetection);
        item["ManagedAddressConfigurationSupported"] = static_cast<bool>(entry->ManagedAddressConfigurationSupported);
        item["OtherStatefulConfigurationSupported"] = static_cast<bool>(entry->OtherStatefulConfigurationSupported);
        item["AdvertiseDefaultRoute"] = static_cast<bool>(entry->AdvertiseDefaultRoute);
        item["RouterDiscoveryBehavior"] = entry->RouterDiscoveryBehavior;
        item["DadTransmits"] = entry->DadTransmits;
        item["BaseReachableTime"] = entry->BaseReachableTime;
        item["RetransmitTime"] = entry->RetransmitTime;
        item["PathMtuDiscoveryTimeout"] = entry->PathMtuDiscoveryTimeout;
        item["LinkLocalAddressBehavior"] = entry->LinkLocalAddressBehavior;
        item["LinkLocalAddressTimeout"] = entry->LinkLocalAddressTimeout;
        item["ZoneIndices"] = wr::ToString(entry->ZoneIndices, std::size(entry->ZoneIndices));
        item["SitePrefixLength"] = entry->SitePrefixLength;
        item["Metric"] = entry->Metric;
        item["NlMtu"] = entry->NlMtu;
        item["Connected"] = static_cast<bool>(entry->Connected);
        item["SupportsWakeUpPatterns"] = static_cast<bool>(entry->SupportsWakeUpPatterns);
        item["SupportsNeighborDiscovery"] = static_cast<bool>(entry->SupportsNeighborDiscovery);
        item["SupportsRouterDiscovery"] = static_cast<bool>(entry->SupportsRouterDiscovery);
        item["ReachableTime"] = entry->ReachableTime;

        nlohmann::json transmitOffloadJson;
        transmitOffloadJson["NlChecksumSupported"] = static_cast<bool>(entry->TransmitOffload.NlChecksumSupported);
        transmitOffloadJson["NlOptionsSupported"] = static_cast<bool>(entry->TransmitOffload.NlOptionsSupported);
        transmitOffloadJson["TlDatagramChecksumSupported"] =
            static_cast<bool>(entry->TransmitOffload.TlDatagramChecksumSupported);
        transmitOffloadJson["TlStreamChecksumSupported"] =
            static_cast<bool>(entry->TransmitOffload.TlStreamChecksumSupported);
        transmitOffloadJson["TlStreamOptionsSupported"] =
            static_cast<bool>(entry->TransmitOffload.TlStreamOptionsSupported);
        transmitOffloadJson["FastPathCompatible"] = static_cast<bool>(entry->TransmitOffload.FastPathCompatible);
        transmitOffloadJson["TlLargeSendOffloadSupported"] =
            static_cast<bool>(entry->TransmitOffload.TlLargeSendOffloadSupported);
        transmitOffloadJson["TlGiantSendOffloadSupported"] =
            static_cast<bool>(entry->TransmitOffload.TlGiantSendOffloadSupported);
        item["TransmitOffload"] = transmitOffloadJson;

        nlohmann::json receiveOffloadJson;
        receiveOffloadJson["NlChecksumSupported"] = static_cast<bool>(entry->ReceiveOffload.NlChecksumSupported);
        receiveOffloadJson["NlOptionsSupported"] = static_cast<bool>(entry->ReceiveOffload.NlOptionsSupported);
        receiveOffloadJson["TlDatagramChecksumSupported"] =
            static_cast<bool>(entry->ReceiveOffload.TlDatagramChecksumSupported);
        receiveOffloadJson["TlStreamChecksumSupported"] =
            static_cast<bool>(entry->ReceiveOffload.TlStreamChecksumSupported);
        receiveOffloadJson["TlStreamOptionsSupported"] =
            static_cast<bool>(entry->ReceiveOffload.TlStreamOptionsSupported);
        receiveOffloadJson["FastPathCompatible"] = static_cast<bool>(entry->ReceiveOffload.FastPathCompatible);
        receiveOffloadJson["TlLargeSendOffloadSupported"] =
            static_cast<bool>(entry->ReceiveOffload.TlLargeSendOffloadSupported);
        receiveOffloadJson["TlGiantSendOffloadSupported"] =
            static_cast<bool>(entry->ReceiveOffload.TlGiantSendOffloadSupported);
        item["TransmitOffload"] = receiveOffloadJson;

        item["DisableDefaultRoutes"] = static_cast<bool>(entry->DisableDefaultRoutes);

        json.push_back(item);
    }

    return json.dump(4);
}

static std::string s_debug_is_running_as_admin()
{
    nlohmann::json item;
    item["IsAdmin"] = wr::IsRunningAsAdmin();
    return item.dump(4);
}

wr::DebugPanel::Data::Data(DebugPanel* owner)
{
    this->owner = owner;
    funcs.push_back(DebugFunc{ "GetAdaptersAddresses", s_debug_get_adapters_addresses });
    funcs.push_back(DebugFunc{ "GetIpForwardTable2", s_debug_get_ip_forward_table2 });
    funcs.push_back(DebugFunc{ "GetIpInterfaceTable", s_debug_get_ip_interface_table });
    funcs.push_back(DebugFunc{ "IsRunningAsAdmin", s_debug_is_running_as_admin });

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    {
        wxBoxSizer* sizer2 = new wxBoxSizer(wxHORIZONTAL);

        comboBox = new wxComboBox(owner, WIDGET_DEBUG_COMBOBOX_FUNC);
        comboBox->SetEditable(false);
        for (const auto& func : funcs)
        {
            comboBox->Append(func.name);
        }
        comboBox->SetSelection(0);
        currentFunc = &funcs[0];
        sizer2->Add(comboBox);

        sizer2->Add(new wxButton(owner, wxID_REFRESH));
        sizer2->Add(new wxButton(owner, WIDGET_DEBUG_BUTTON_COPY_TO_CLIPBOARD, _("Copy to clipboard")));
        sizer2->Add(new wxButton(owner, WIDGET_DEBUG_BUTTON_OPEN_ONLINE_JSON_EDITOR, _("Open Online Json Editor")));

        sizer->Add(sizer2);
    }
    {
        textData = new wxTextCtrl(owner, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                  wxTE_MULTILINE | wxTE_READONLY);
        sizer->Add(textData, 1, wxEXPAND);
    }

    owner->SetSizer(sizer);

    owner->Bind(wxEVT_COMBOBOX, &Data::OnSelect, this, WIDGET_DEBUG_COMBOBOX_FUNC);
    owner->Bind(wxEVT_BUTTON, &Data::OnRefresh, this, wxID_REFRESH);
    owner->Bind(wxEVT_BUTTON, &Data::OnCopyToClipboard, this, WIDGET_DEBUG_BUTTON_COPY_TO_CLIPBOARD);
    owner->Bind(wxEVT_BUTTON, &Data::OnOpenURL, this, WIDGET_DEBUG_BUTTON_OPEN_ONLINE_JSON_EDITOR);

    Refresh();
}

void wr::DebugPanel::Data::Refresh()
{
    const std::string data = currentFunc->func();
    textData->SetValue(wxString::FromUTF8(data));
}

void wr::DebugPanel::Data::OnSelect(wxCommandEvent& e)
{
    const int idx = e.GetInt();
    currentFunc = &funcs[idx];
    Refresh();
}

void wr::DebugPanel::Data::OnRefresh(wxCommandEvent&)
{
    Refresh();
}

void wr::DebugPanel::Data::OnCopyToClipboard(wxCommandEvent&)
{
    wxString data = textData->GetValue();

    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(data));
        wxTheClipboard->Close();
    }
}

void wr::DebugPanel::Data::OnOpenURL(wxCommandEvent&)
{
    wxLaunchDefaultBrowser("https://jsoneditoronline.org");
}

wr::DebugPanel::DebugPanel(wxWindow* parent) : wxPanel(parent)
{
    m_data = new Data(this);
}

wr::DebugPanel::~DebugPanel()
{
    delete m_data;
}
