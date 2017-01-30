#ifndef RED_WIFI_MAC_HELPER_H
#define RED_WIFI_MAC_HELPER_H

#include "ns3/nqos-wifi-mac-helper.h"
#include "REDWifiMac.h"

namespace ns3 {

  class REDWifiMacHelper : public NqosWifiMacHelper {

    public:
      REDWifiMacHelper();
      virtual ~REDWifiMacHelper();

    protected:
      ObjectFactory m_mac;

    private:
      virtual Ptr<WifiMac> Create(void) const;
  };

}

#endif