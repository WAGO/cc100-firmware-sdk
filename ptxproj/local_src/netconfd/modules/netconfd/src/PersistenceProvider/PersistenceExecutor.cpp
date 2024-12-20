// SPDX-License-Identifier: GPL-2.0-or-later

#include "PersistenceExecutor.hpp"

#include <boost/format.hpp>
#include "TypesHelper.hpp"
#include <iostream>

#include "JsonConverter.hpp"
#include "JsonBuilder.hpp"
#include "JsonRestorer.hpp"
#include "Logger.hpp"
#include "FirmwareVersion.hpp"
#include "KeyValueParser.hpp"
#include "BaseTypes.hpp"

namespace netconf {

constexpr auto bridge_backup_key = "bridge-config";
constexpr auto ip_backup_key = "ip-config";
constexpr auto dip_ip_backup_key = "dip-ip-config";
constexpr auto interface_backup_key = "interfaces";
constexpr auto interface_config_backup_key = "interface-config";
constexpr auto switch_fast_aging_backup_key = "switch-fast-aging";

namespace {

void RestoreMacLearningFromLegacyBackup(InterfaceConfigs &interface_configs, ::std::string &backup_data) {
  auto fast_aging = GetValueByKey(backup_data, switch_fast_aging_backup_key);

  for (auto &c : interface_configs) {
    if (c.interface_.GetName() == "ethX1" || c.interface_.GetName() == "ethX2") {
      if (c.mac_learning_ == MacLearning::UNKNOWN) {
        c.mac_learning_ = fast_aging == "1" ? MacLearning::OFF : MacLearning::ON;
      }
    } else {
      c.mac_learning_ = MacLearning::ON;
    }
  }
}

bool IsMacLearningUnknown(InterfaceConfigs &itf_configs) {
  return ::std::any_of(
      itf_configs.cbegin(),
      itf_configs.cend(),
      [](const InterfaceConfig &ic) {
        return (ic.interface_.GetName() == "ethX1" || ic.interface_.GetName() == "ethX2")
            && ic.mac_learning_ == MacLearning::UNKNOWN;
      });
}

}

PersistenceExecutor::PersistenceExecutor(const ::std::string &persistence_path, IFileEditor &file_editor,
                                         IBackupRestore &backup_restore, IBackupRestore &legacy_restore,
                                         IDipSwitch &dip_switch)
    : persistence_path_ { persistence_path + "/netconfd.json" },
      interface_config_file_path_ { persistence_path + "/netconfd_interface_config.json" },
      file_editor_ { file_editor },
      backup_restore_ { backup_restore },
      legacy_restore_ { legacy_restore },
      dip_switch_ { dip_switch } {

  ReadNetconfdJson();
  ReadInterfaceConfigJson();
}

Status PersistenceExecutor::UpdateNetconfdJson() const {
  ::std::string json_str;
  JsonBuilder jb;

  jb.Append("bridge-config", current_bridge_config_).Append("ip-config", current_ip_configs_);

  if (!current_interfaces_.empty()) {
    jb.Append("interfaces", current_interfaces_);
  }

  if (dip_switch_.GetMode() != DipSwitchMode::HW_NOT_AVAILABLE && !current_dip_switch_config_.IsIncomplete()) {
    jb.Append("dip-ip-config", current_dip_switch_config_);
  }

  json_str = jb.ToString(JsonFormat::PRETTY);
  return file_editor_.WriteAndReplace(persistence_path_, json_str);
}

Status PersistenceExecutor::Write(const BridgeConfig &config) {
  if (current_bridge_config_ == config) {
    return Status();
  }

  current_bridge_config_ = config;

  Status status = UpdateNetconfdJson();

  return status;
}

Status PersistenceExecutor::ReadNetconfdJson() {
  ::std::string json;
  Status status = file_editor_.Read(persistence_path_, json);

  if (status.IsOk()) {
    current_bridge_config_.clear();
    current_ip_configs_.clear();
    current_dip_switch_config_.Clear();
    current_interfaces_.clear();

    JsonRestorer jr(::std::move(json));
    status = jr.Restore("bridge-config", current_bridge_config_);
    if (status.IsOk()) {
      status = jr.Restore("ip-config", current_ip_configs_);
    }
    if (status.IsOk() && dip_switch_.GetMode() != DipSwitchMode::HW_NOT_AVAILABLE) {
      status = jr.Restore("dip-ip-config", current_dip_switch_config_);
    }
    jr.Restore("interfaces", current_interfaces_);
  }
  return status;
}

Status PersistenceExecutor::Read(BridgeConfig &config) {
  config = current_bridge_config_;
  if (current_bridge_config_.empty()) {
    return Status { StatusCode::PERSISTENCE_READ };
  }
  return {};
}

Status PersistenceExecutor::Read(BridgeConfig &config, IPConfigs &configs, Interfaces& interfaces) {
  config = current_bridge_config_;
  configs = current_ip_configs_;
  interfaces = current_interfaces_;

  if (current_bridge_config_.empty() || current_ip_configs_.empty()) {
    return Status { StatusCode::PERSISTENCE_READ };
  }

  return {};
}

Status PersistenceExecutor::Read(DipSwitchIpConfig &config) {
  config = current_dip_switch_config_;
  if (current_dip_switch_config_.IsIncomplete()) {
    return Status { StatusCode::PERSISTENCE_READ };
  }
  return {};
}

Status PersistenceExecutor::Read(IPConfigs &configs) {
  configs = current_ip_configs_;

  if (current_ip_configs_.empty()) {
    return Status { StatusCode::PERSISTENCE_READ };
  }
  return {};
}

void PersistenceExecutor::Read(Interfaces &config) {
  config = current_interfaces_;
}

Status PersistenceExecutor::Write(const Interfaces &config) {
  if (current_interfaces_ == config) {
    return Status();
  }

  current_interfaces_ = config;
  Status status = UpdateNetconfdJson();

  if (status.IsNotOk()) {
    status.Set(StatusCode::PERSISTENCE_WRITE);
  }

  return status;
}

Status PersistenceExecutor::Write(const DipSwitchIpConfig &config) {
  if (current_dip_switch_config_ == config) {
    return {};
  }

  current_dip_switch_config_ = config;
  Status status = UpdateNetconfdJson();

  if (status.IsNotOk()) {
    status.Set(StatusCode::PERSISTENCE_WRITE);
  }

  return status;
}

Status PersistenceExecutor::Write(const IPConfigs &configs) {
  if (current_ip_configs_ == configs) {
    return {};
  }

  current_ip_configs_ = configs;
  Status status = UpdateNetconfdJson();

  if (status.IsNotOk()) {
    status.Set(StatusCode::PERSISTENCE_WRITE);
  }

  return status;
}

Status PersistenceExecutor::Write(const InterfaceConfigs &configs) {

  if (current_interface_configs_ == configs) {
    return {};
  }

  current_interface_configs_ = configs;

  JsonConverter jc;
  auto json_str = jc.ToJsonString(current_interface_configs_, JsonFormat::PRETTY);

  Status status = file_editor_.WriteAndReplace(interface_config_file_path_, json_str);

  return status;
}

Status PersistenceExecutor::ReadInterfaceConfigJson() {
  ::std::string json;
  Status status = file_editor_.Read(interface_config_file_path_, json);
  if (status.IsOk()) {
    current_interface_configs_.clear();
    JsonConverter jc;
    status = jc.FromJsonString(json, current_interface_configs_);
  }
  return status;
}

Status PersistenceExecutor::Read(InterfaceConfigs &configs) {
  configs = current_interface_configs_;
  return {};
}

static void AddLegacyIpParameter(IPConfigs &ip_configs, Interface interface, ::std::string &legacy_ip_config) {

  auto bridge_index = ::std::string(1, interface.GetName().back());

  auto find_bridge = [&](IPConfig ip_config) {
    return ip_config.interface_ == interface.GetName();
  };

  auto it = ::std::find_if(ip_configs.begin(), ip_configs.end(), find_bridge);
  if (it != ip_configs.end()) {
    legacy_ip_config.append("ip-config-type-eth" + bridge_index + "=" + IPSourceToString(it->source_) + "\n");
    legacy_ip_config.append("ip-address-eth" + bridge_index + "=" + it->address_ + "\n");
    legacy_ip_config.append("subnet-mask-eth" + bridge_index + "=" + it->netmask_ + "\n");
  } else {
    legacy_ip_config.append("ip-config-type-eth" + bridge_index + "=static\n");
    legacy_ip_config.append("ip-address-eth" + bridge_index + "=0.0.0.0\n");
    legacy_ip_config.append("subnet-mask-eth" + bridge_index + "=0.0.0.0\n");
  }
}

static void AddLegacyInterfaceParameter(InterfaceConfigs &itf_configs, Interface itf,
                                        ::std::string &legacy_itf_config) {

  ::std::string itf_index = ::std::string(itf.GetName() == "ethX1" ? ::std::string("0") : ::std::string("1"));

  auto find_itf = [&](InterfaceConfig itf_config) {
    return itf_config.interface_.GetName() == itf.GetName();
  };

  auto it = ::std::find_if(itf_configs.begin(), itf_configs.end(), find_itf);
  if (it != itf_configs.end()) {

    auto state = ::std::string(it->state_ == InterfaceState::UP ? "enabled" : "disabled");
    legacy_itf_config.append("ip-state-eth" + itf_index + "=" + state + "\n");
    legacy_itf_config.append("eth" + itf_index + "-state=" + state + "\n");

    auto autoneg = ::std::string(it->autoneg_ == Autonegotiation::ON ? "enabled" : "disabled");
    legacy_itf_config.append("eth" + itf_index + "-autoneg=" + autoneg + "\n");

    auto speed = ::std::to_string(it->speed_);
    legacy_itf_config.append("eth" + itf_index + "-speed=" + speed + "\n");

    auto duplex = ::std::string(it->duplex_ == Duplex::FULL ? ::std::string("full") : ::std::string("half"));
    legacy_itf_config.append("eth" + itf_index + "-duplex=" + duplex + "\n");

  } else {
    legacy_itf_config.append("ip-state-eth" + itf_index + "=enabled\n");
    legacy_itf_config.append("eth" + itf_index + "-state=enabled\n");
    legacy_itf_config.append("eth" + itf_index + "-autoneg=enabled\n");
    legacy_itf_config.append("eth" + itf_index + "-speed=100\n");
    legacy_itf_config.append("eth" + itf_index + "-duplex=full\n");
  }
}

static void AddLegacySwitchFastAgingParameter(InterfaceConfigs &itf_configs, ::std::string &legacy_itf_config) {

  ::std::string fast_aging = "0";

  auto number_of_mac_learning_off_itfs = ::std::count_if(
      itf_configs.cbegin(),
      itf_configs.cend(),
      [](const InterfaceConfig &ic) {
        return (ic.interface_.GetName() == "ethX1" || ic.interface_.GetName() == "ethX2")
            && (ic.mac_learning_ == MacLearning::OFF);
      });

  if (number_of_mac_learning_off_itfs == 2) {
    // MAC learning is disabled for both interfaces X1 and X2, therefore, set global fast aging option to off.
    fast_aging = "1";
  }

  legacy_itf_config.append(::std::string { switch_fast_aging_backup_key } + "=" + fast_aging + "\n");
}

void PersistenceExecutor::ModifyBr0AddressToDipSwitch(IPConfigs &ip_configs) {

  auto find_br0 = [&](IPConfig &ip_config) {
    return ip_config.interface_ == "br0";
  };

  auto it = ::std::find_if(ip_configs.begin(), ip_configs.end(), find_br0);
  if (it != ip_configs.end()) {
    it->address_ = current_dip_switch_config_.address_;
    it->netmask_ = current_dip_switch_config_.netmask_;
  }
}

static ::std::string GenerateLegacyIPBackup(IPConfigs &ip_configs) {

  ::std::string legacy_ip_parameter;
  AddLegacyIpParameter(ip_configs, Interface::CreateBridge("br0"), legacy_ip_parameter);
  AddLegacyIpParameter(ip_configs, Interface::CreateBridge("br1"), legacy_ip_parameter);
  return legacy_ip_parameter;
}

static ::std::string GenerateLegacyInterfaceBackup(InterfaceConfigs &itf_configs) {
  ::std::string legacy_interface_parameter;
  AddLegacyInterfaceParameter(itf_configs, Interface::CreatePort("ethX1"), legacy_interface_parameter);
  AddLegacyInterfaceParameter(itf_configs, Interface::CreatePort("ethX2"), legacy_interface_parameter);
  AddLegacySwitchFastAgingParameter(itf_configs, legacy_interface_parameter);
  return legacy_interface_parameter;
}

Status PersistenceExecutor::Backup(const std::string &file_path, const std::string &targetversion) {
  IPConfigs current_ip_configs = current_ip_configs_;

  FirmwareVersion fw_target { targetversion };
  FirmwareVersion fw_with_dipswitch { "03.04.02(16)" };

  if (fw_target.IsValid() && (fw_target < fw_with_dipswitch) && (dip_switch_.GetMode() == DipSwitchMode::STATIC)) {
    ModifyBr0AddressToDipSwitch(current_ip_configs);
  }

  JsonBuilder jb_network_data;
  jb_network_data.Append(interface_backup_key, current_interfaces_, fw_target).Append(bridge_backup_key,
                                                                                      current_bridge_config_, fw_target)
      .Append(ip_backup_key, current_ip_configs, fw_target).Append(interface_config_backup_key,
                                                                   current_interface_configs_, fw_target);

  ::std::string dipswitch_data;
  if (dip_switch_.GetMode() != DipSwitchMode::HW_NOT_AVAILABLE) {
    JsonBuilder jb_dipswitch_data;
    jb_dipswitch_data.Append(dip_ip_backup_key, current_dip_switch_config_);
    dipswitch_data = jb_dipswitch_data.ToString();
  }

  Status status = backup_restore_.Backup(file_path, jb_network_data.ToString(), dipswitch_data, persistence_version_);

  if (status.IsOk()) {
    ::std::string legacy_ip_parameter = GenerateLegacyIPBackup(current_ip_configs);
    legacy_ip_parameter.append(GenerateLegacyInterfaceBackup(current_interface_configs_));
    status = legacy_restore_.Backup(file_path, legacy_ip_parameter, "", 0);
  }

  return status;
}

Status PersistenceExecutor::Restore(const ::std::string &file_path, BridgeConfig &bridge_config, IPConfigs &ip_configs,
                                    InterfaceConfigs &interface_configs, DipSwitchIpConfig &dip_switch_config,
                                    Interfaces &interfaces) {
  bridge_config.clear();
  ip_configs.clear();
  interface_configs.clear();
  dip_switch_config.Clear();
  interfaces.clear();

  uint32_t version = 0;
  ::std::string network_data_json;
  ::std::string dipswitch_data_json;

  Status status = backup_restore_.Restore(file_path, network_data_json, dipswitch_data_json, version);
  if (status.IsNotOk()) {
    status = legacy_restore_.Restore(file_path, network_data_json, dipswitch_data_json, version);
  }

  if (status.IsOk() && (version != persistence_version_)) {
    status = Status { StatusCode::BACKUP_VERSION, ::std::to_string(persistence_version_), ::std::to_string(version) };
  }

  JsonRestorer data_restorer { ::std::move(network_data_json) };
  if (status.IsOk()) {
    status = data_restorer.Restore(bridge_backup_key, bridge_config);
  }
  if (status.IsOk()) {
    status = data_restorer.Restore(ip_backup_key, ip_configs);
  }
  if (status.IsOk()) {
    status = data_restorer.Restore(interface_backup_key, interfaces);
  }
  if (status.IsOk()) {
    status = data_restorer.Restore(interface_config_backup_key, interface_configs);

    // With FW22 we introduced the maclearning parameter.
    // For firmware versions less than FW22 the maclearning parameter can be reconstructed from the fastaging parameter.
    if (IsMacLearningUnknown(interface_configs)) {
      auto backup_data = ::std::string { };
      file_editor_.Read(file_path, backup_data);

      RestoreMacLearningFromLegacyBackup(interface_configs, backup_data);
    }

  }

  if (status.IsOk() && not dipswitch_data_json.empty()) {
    JsonRestorer dip_switch_restorer { ::std::move(dipswitch_data_json) };
    status = dip_switch_restorer.Restore(dip_ip_backup_key, dip_switch_config);
  }

  ::std::string json_str;
  if (status.IsOk()) {
    JsonBuilder jb;
    jb.Append("interfaces", interfaces).Append("bridge-config", bridge_config).Append("ip-config", ip_configs);
    if (dip_switch_.GetMode() != DipSwitchMode::HW_NOT_AVAILABLE) {
      jb.Append("dip-ip-config", dip_switch_config);
    }
    json_str = jb.ToString(JsonFormat::PRETTY);
  }

  if (status.IsOk()) {
    status = file_editor_.WriteAndReplace(persistence_path_, json_str);
    current_bridge_config_ = bridge_config;
    current_ip_configs_ = ip_configs;
    current_dip_switch_config_ = dip_switch_config;
    current_interfaces_ = interfaces;
  }

  if (status.IsOk()) {
    JsonConverter jc;
    auto json_string = jc.ToJsonString(interface_configs, JsonFormat::PRETTY);
    current_interface_configs_ = interface_configs;
    status = file_editor_.WriteAndReplace(interface_config_file_path_, json_string);
  }

  if (status.IsNotOk()) {
    bridge_config.clear();
    ip_configs.clear();
    dip_switch_config.Clear();
    interfaces.clear();
  }

  return status;
}

uint32_t PersistenceExecutor::GetBackupParameterCount() const {
  return backup_restore_.GetBackupParameterCount();
}

} /* namespace netconf */
