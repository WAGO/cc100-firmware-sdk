// SPDX-License-Identifier: LGPL-3.0-or-later

#include "JsonConverter.hpp"

#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/network_v4.hpp>
#include <boost/system/error_code.hpp>
#include <nlohmann/json.hpp>
#include <utility>

#include "CollectionUtils.hpp"
#include "JsonHelper.hpp"
#include "Status.hpp"
#include "Types.hpp"
#include "TypesHelper.hpp"

namespace netconf {

using namespace ::std::literals;

::std::string JsonConverter::ToJsonString(const netconf::DipSwitchConfig &obj, JsonFormat format) const {
  json json_config =
      json{{"ipaddr", obj.ip_config_.address_}, {"netmask", obj.ip_config_.netmask_}, {"value", obj.value_}};
  to_json(json_config["mode"], obj.mode_);
  try {
    return json_config.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
  } catch (...) {
    return std::string{};
  }
}

Status JsonConverter::FromJsonString(const ::std::string &str, netconf::DipSwitchConfig &out_obj) const {
  try {
    return JsonToDipSwitchConfig(str, out_obj);
  } catch (std::exception &e) {
    return Status {StatusCode::JSON_CONVERT, e.what()};
  }
  return {};
}

::std::string JsonConverter::ToJsonString(const netconf::DipSwitchIpConfig &obj, JsonFormat format) const {
  return json{{"ipaddr", obj.address_}, {"netmask", obj.netmask_}}.dump(
      format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, netconf::DipSwitchIpConfig &out_obj) const {
  json json_object;
  auto status = JsonToNJson(str, json_object);
  if (status.IsOk()) {
    status = GetToOrError("ipaddr", json_object, out_obj.address_);
  }
  if (status.IsOk()) {
    status = GetToOrError("netmask", json_object, out_obj.netmask_);
  }
  return status;
}

::std::string JsonConverter::ToJsonString(const BridgeConfig &obj, JsonFormat format) const {
  json bridge_config_json = BridgeConfigToNJson(obj);
  return bridge_config_json.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, BridgeConfig &out_obj) const {
  try {
    json whole_bridge_json;
    Status status = JsonToNJson(str, whole_bridge_json);
    if (status.IsNotOk()) {
      return status;
    }
    return NJsonToBridgeConfig(whole_bridge_json, out_obj);
  } catch (std::exception &e) {
    out_obj.clear();
    return Status {StatusCode::JSON_CONVERT, e.what()};
  }
}

::std::string JsonConverter::ToJsonString(const IPConfigs &obj, JsonFormat format) const {
  try {
    return IPConfigsToNJson(obj).dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
  } catch (std::exception &ex) {
    return ::std::string{};
  }
}

Status JsonConverter::FromJsonString(const ::std::string &str, IPConfigs &out_obj) const {
  try {
    nlohmann::json j;
    auto status = JsonToNJson(str, j);
    if (status.IsNotOk()) {
      return status;
    }
    return NJsonToIPConfigs(j, out_obj);
  } catch (std::exception &e) {
    out_obj.clear();
    return Status {StatusCode::JSON_CONVERT, e.what()};
  }
}

::std::string JsonConverter::ToJsonString(const InterfaceConfigs &obj, JsonFormat format) const {
  if (obj.size() > 1) {
    nlohmann::json jarray{};
    for (const auto &config : obj) {
      jarray.push_back(InterfaceConfigToNJson(config));
    }
    return jarray.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
  }

  return (obj.size() == 1 ? InterfaceConfigToNJson(obj.at(0)) : nlohmann::json({}))
      .dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

::std::string JsonConverter::ToJsonString(const InterfaceStatuses &obj, JsonFormat format) const {
  if (obj.size() > 1) {
    nlohmann::json jarray{};
    for (const auto &config : obj) {
      jarray.push_back(InterfaceStatusToNJson(config));
    }
    return jarray.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
  }

  return (obj.size() == 1 ? InterfaceStatusToNJson(obj.at(0)) : nlohmann::json({}))
      .dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, InterfaceConfigs &out_obj) const {
  json json_out;
  auto status = JsonToNJson(str, json_out);
  if (status.IsNotOk()) {
    return status;
  }
  return NJsonToInterfaceConfigs(json_out, out_obj);
}

Status JsonConverter::FromJsonString(const ::std::string &str, InterfaceStatuses &out_obj) const {
  json json_out;
  auto status = JsonToNJson(str, json_out);
  if (status.IsNotOk()) {
    return status;
  }
  return NJsonToInterfaceStatuses(json_out, out_obj);
}

::std::string JsonConverter::ToJsonString(const IPConfig &obj, JsonFormat format) const {
    return json{{Interface::LabelFromName(obj.interface_), {{"ipaddr", obj.address_}, {"netmask", obj.netmask_}, {"source", obj.source_}, {"dhcp-client-id", obj.dhcp_client_id_}}}}.dump(
      format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, IPConfig &out_obj) const {
  json j;
  auto status = JsonToNJson(str, j);
  if (status.IsNotOk()) {
    return status;
  }
  out_obj = ParseIpConfig(j.items().begin());
  return {};
}

::std::string JsonConverter::ToJsonString(const InterfaceConfig &obj, JsonFormat format) const {
  return InterfaceConfigToNJson(obj).dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, InterfaceConfig &out_obj) const {
  json j;
  auto status = JsonToNJson(str, j);
  if (status.IsNotOk()) {
    return status;
  }
  out_obj = InterfaceConfigFromJson(j);
  return {};
}

::std::string JsonConverter::ToJsonString(const InterfaceInformation &obj, JsonFormat format) const {
  return InterfaceInformationToNJson(obj).dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, InterfaceInformation &out_obj) const {
  json j;
  auto status = JsonToNJson(str, j);
  if (status.IsNotOk()) {
    return status;
  }
  return NJsonToInterfaceInformation(j, out_obj);
}

::std::string JsonConverter::ToJsonString(const InterfaceInformations &obj, JsonFormat format) const {
  auto j = json::array();
  for (const auto &o : obj) {
    j.push_back(InterfaceInformationToNJson(o));
  }
  return j.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, InterfaceInformations &out_obj) const {
  json j_array;
  auto status = JsonToNJson(str, j_array);
  if (status.IsNotOk()) {
    return status;
  }

  if (j_array.is_array()) {
    for (const auto &j_item : j_array) {
      InterfaceInformation ii;
      status = NJsonToInterfaceInformation(j_item, ii);
      if (status.IsNotOk()) {
        return status;
      }
      out_obj.push_back(ii);
    }
    if (status.IsNotOk()) {
      out_obj.clear();
    }
  } else {
    return Status {StatusCode::JSON_CONVERT};
  }
  return {};
}

::std::string JsonConverter::ToJsonString(const Interface &obj, JsonFormat format) const {
  auto j = InterfaceToNJson(obj);
  return j.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

::std::string JsonConverter::ToJsonString(const Interfaces &obj, JsonFormat format) const {
  auto j = InterfacesToNJson(obj);
  return j.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, Interface &out_obj) const {

  json j;
  auto status = JsonToNJson(str, j);
  if (status.IsNotOk()) {
    return status;
  }

  return NJsonToInterface(j, out_obj);
}

Status JsonConverter::FromJsonString(const ::std::string &str, Interfaces &out_obj) const {

  json j;
  auto status = JsonToNJson(str, j);
  if (status.IsNotOk()) {
    return status;
  }

  return NJsonToInterfaces(j, out_obj);
}


::std::string JsonConverter::ToJsonString(const Status &obj, JsonFormat format) const {
  json j{};

  j["errorcode"] = static_cast<uint32_t>(obj.GetStatusCode());

  auto &params = obj.GetParameter();
  if (not params.empty()) {
    j["parameter"] = params;
  }

  return j.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, Status &out_obj) const {
  json j;
  auto status = JsonToNJson(str, j);
  if (status.IsNotOk()) {
    return status;
  }
  StatusCode code;
  status = GetToOrError("errorcode", j, code);
  if (status.IsNotOk()) {
    return status;
  }
  Status::Parameters params;
  GetToIfExists("parameter", j, params);

  out_obj.Set(code, ::std::move(params));
  return status;
}

::std::string JsonConverter::ToJsonString(const ::std::string &itf_name, DynamicIPEventAction action,
                                          JsonFormat format) const {
  json j{{"interface", itf_name}, {"action", action}};


  return j.dump(format == JsonFormat::COMPACT ? JSON_DUMP : JSON_PRETTY_DUMP);
}

Status JsonConverter::FromJsonString(const ::std::string &str, ::std::string &itf_name,
                                     DynamicIPEventAction &action) const {
  itf_name = "";
  action   = DynamicIPEventAction::UNKNOWN;

  json j;
  auto status = JsonToNJson(str, j);

  if (status.IsOk()) {
    status = GetToOrError("interface", j, itf_name);
  }
  if (status.IsOk()) {
    status = GetToOrError("action", j, action);
  }

  return status;
}

}  // namespace netconf
