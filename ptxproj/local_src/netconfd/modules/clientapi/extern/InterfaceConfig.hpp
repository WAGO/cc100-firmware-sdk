// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <optional>
#include <memory>
#include <string>
#include <cstring>

#include "Status.hpp"
#include "Types.hpp"
#include "ConfigBase.hpp"

namespace netconf {
namespace api {

/**
 * Get the MAC address of a system interface.
 * The full system interface name must be given (e.g. wwan0)
 *
 * @param interface_name system interface name
 * @return MacAddress object.
 */
::std::string GetMacAddress(const ::std::string &interface);

/**
 * Set the state of any interface in the system.
 * The full system interface name must be given (e.g. wwan0)
 * @note The InterfaceState is not saved persistently
 *
 * @param interface_name The system interface name to be set
 * @param state The new state of the interface
 * @return Status object containing status of the operation.
 */
Status SetInterfaceState(const ::std::string &interface, InterfaceState state);

/**
 * Get the current state of any interface in the system.
 * The full system interface name must be given (e.g. wwan0)
 *
 * @param interface_name The system interface name to be set
 * @param state Destination of the current interface state
 * @return Status object containing status of the operation.
 */
Status GetInterfaceState(const ::std::string &interface, InterfaceState &state);

/**
 * @brief Container class for the @see InterfaceConfig
 *
 */
class InterfaceConfigs: public detail::ConfigBase<netconf::InterfaceConfig> {
 public:
  InterfaceConfigs() = default;
  explicit InterfaceConfigs(const netconf::InterfaceConfigs& config);

  void AddInterfaceConfig(InterfaceConfig config);
  void RemoveInterfaceConfig(const ::std::string &interface);
#if __cplusplus >= 201703L
  ::std::optional<InterfaceConfig> GetInterfaceConfig(const ::std::string &interface);
#endif

  InterfaceConfig& operator[](const ::std::string &interface);

 private:
  ::std::string GetCompareValue(const netconf::InterfaceConfig& ic) const noexcept override;

};

/**
 * Convert a base interface config to a json string.
 * @return
 */
::std::string ToJson(const netconf::InterfaceConfig& config) noexcept;
::std::string ToPrettyJson(const netconf::InterfaceConfig& config) noexcept;
::std::string ToString(const netconf::InterfaceConfig& config, const ::std::string& sep = " ") noexcept;

::std::string ToJson(const InterfaceConfigs& configs) noexcept;
::std::string ToPrettyJson(const InterfaceConfigs& configs) noexcept;
::std::string ToString(const InterfaceConfigs& configs) noexcept;

::std::string ToJson(const InterfaceStatuses& states) noexcept;
::std::string ToPrettyJson(const InterfaceStatuses& states) noexcept;
::std::string ToString(const InterfaceStatuses& states) noexcept;
::std::string ToString(const netconf::InterfaceStatus &state, const ::std::string &sep) noexcept;


/**
 * Create a InterfaceConfigs from a json string.
 * @return InterfaceConfigs object.
 */
Status MakeInterfaceConfigs(const ::std::string &json_str, InterfaceConfigs& config);

/**
 * @brief Get the Interface Configs object from the netconfd network configuration daemon.
 *
 * @param config
 * @return Status
 */
Status GetInterfaceConfigs(InterfaceConfigs& config);

/**
 * @brief Set the Interface Configs object for the netconfd network
 *
 * @param config
 * @return Status
 */
Status SetInterfaceConfigs(const InterfaceConfigs &config);

/**
 * Create InterfaceStatuses from a json string.
 * @return InterfaceStatuses object.
 */
Status MakeInterfaceStatuses(const ::std::string &json_str, InterfaceStatuses& statuses);

/**
 * @brief Get the Interface Statuses object from the netconfd network configuration daemon.
 *
 * @param statuses
 * @return Status
 */
Status GetInterfaceStatuses(InterfaceStatuses &statuses);

/**
 * Validate the interface configurations against the interface informations obtained by GetInterfaceInformation()
 *
 * @param configs The configs to be validated
 * @param interface_infos The interface information considered in validation
 * @return Result of the validation
 */
Status ValidateInterfaceConfigs(const InterfaceConfigs &configs, const InterfaceInformations& interface_infos );

}  // namespace api
}  // namespace netconf
