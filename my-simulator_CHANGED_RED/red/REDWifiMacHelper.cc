#include "REDWifiMacHelper.h"

namespace ns3 {

  REDWifiMacHelper::REDWifiMacHelper() {
    m_mac.SetTypeId ("ns3::REDWifiMac");
  }

  REDWifiMacHelper::~REDWifiMacHelper() {}

  Ptr<WifiMac> REDWifiMacHelper::Create (void) const {
    Ptr<WifiMac> mac = m_mac.Create<REDWifiMac>();
    return mac;
  }

}