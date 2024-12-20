// SPDX-License-Identifier: LGPL-3.0-or-later

#include <algorithm>
#include <sstream>
#include <vector>

#include "InterfaceInformationApi.hpp"
#include "JsonConverter.hpp"
#include "NetconfdDbusClient.hpp"
#include "CollectionUtils.hpp"

/*
 * This code is separated in order to allow link seams in unit tests.
 */

namespace netconf {
namespace api {

InterfaceInformations GetInterfaceInformation() {
  NetconfdDbusClient client;
  JsonConverter jc;
  InterfaceInformations iis;
  auto result = client.GetDeviceInterfaces();
  auto status = jc.FromJsonString(result.value_json_, iis);
  if (status.IsNotOk()) {
    /* TODO PND: appropriate status response */
  }
  return iis;
}

InterfaceInformations GetInterfaceInformation(const DeviceTypes& types) {
  NetconfdDbusClient client;
  JsonConverter jc;
  InterfaceInformations iis;
  auto result = client.GetDeviceInterfaces();
  auto status = jc.FromJsonString(result.value_json_, iis);
  if (status.IsNotOk()) {
    /* TODO PND: appropriate status response */
    return iis;
  }

  iis.erase(std::remove_if(iis.begin(), iis.end(), [&types = types](const auto &i) {
    return IsNotIncluded(i.GetType(), types);
  }), iis.end());

  return iis;
}

}  // namespace api
}  // namespace netconf
