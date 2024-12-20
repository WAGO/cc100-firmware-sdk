// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <netconfd_dbus.h>

#include <functional>
#include "IDBusHandlerRegistry.h"

namespace netconf {
namespace dbus {

class DBusHandlerRegistry : public IDBusHandlerRegistry {
 public:
  using Getter = ::std::function<::std::string(::std::string&)>;
  using Setter = ::std::function<::std::string(::std::string)>;
  using Trigger = ::std::function<::std::string()>;

  DBusHandlerRegistry();
  ~DBusHandlerRegistry() override;

  DBusHandlerRegistry(const DBusHandlerRegistry&) = delete;
  DBusHandlerRegistry& operator=(const DBusHandlerRegistry&) = delete;
  DBusHandlerRegistry(DBusHandlerRegistry&&) = delete;
  DBusHandlerRegistry& operator=(DBusHandlerRegistry&&) = delete;

  GDBusObjectSkeleton* GetInterfaceConfigObject() const override;
  GDBusObjectSkeleton* GetIPObject() const override;
  GDBusObjectSkeleton* GetBackupObject() const override;
  GDBusObjectSkeleton* GetEventObject() const override;
  GDBusObjectSkeleton* GetInterfaceObject() const override;

  void RegisterSetBridgeConfigHandler(Setter&& handler) {
    set_bridge_config_handler_ = ::std::forward<Setter>(handler);
  }

  void RegisterGetBridgeConfigHandler(Getter&& handler) {
    get_bridge_config_handler_ = std::forward<Getter>(handler);
  }

  void RegisterGetDeviceInterfacesHandler(Getter&& handler) {
    get_device_interfaces_handler_ = ::std::forward<Getter>(handler);
  }

  void RegisterSetInterfaceConfigHandler(Setter&& handler) {
    set_interface_config_handler_ = ::std::forward<Setter>(handler);
  }

  void RegisterGetInterfaceConfigHandler(Getter&& handler) {
    get_interface_config_handler_ = ::std::forward<Getter>(handler);
  }

  void RegisterGetInterfaceStatusesHandler(Getter&& handler) {
    get_interface_statuses_handler_ = ::std::forward<Getter>(handler);
  }

  void RegisterGetBackupParamCountHandler(::std::function<::std::string(void)>&& handler) {
    get_backup_param_count_handler_ = ::std::forward<::std::function<::std::string(void)>>(handler);
  }

  void RegisterBackupHandler(::std::function<::std::string(::std::string, ::std::string)>&& handler) {
    backup_handler_ = std::forward<::std::function<::std::string(::std::string, ::std::string)>>(handler);
  }

  void RegisterRestoreHandler(Setter&& handler) {
    restore_handler_ = std::forward<Setter>(handler);
  }

  void RegisterSetAllIPConfigsHandler(Setter&& handler) {
    set_all_ip_config_handler_ = std::forward<Setter>(handler);
  }

  void RegisterSetIPConfigHandler(Setter&& handler) {
    set_ip_config_handler_ = std::forward<Setter>(handler);
  }

  void RegisterGetAllIPConfigsHandler(Getter&& handler) {
    get_all_ip_config_handler_ = std::forward<Getter>(handler);
  }

  void RegisterGetAllCurrentIPConfigsHandler(Getter&& handler) {
    get_all_current_ip_config_handler_ = std::forward<Getter>(handler);
  }

  void RegisterTempFixIpHandler(::std::function<::std::string(void)>&& handler) {
    tempfixip_handler_ = std::forward<::std::function<::std::string(void)>>(handler);
  }

  void RegisterSetTempDHCPClientIDHandler(Setter&& handler) {
    set_tempdhcpclientid_handler_ = std::forward<Setter>(handler);
  }

  void RegisterGetTempDHCPClientIDHandler(Getter&& handler) {
    get_tempdhcpclientid_handler_ = std::forward<Getter>(handler);
  }

  void RegisterGetDipSwitchConfigHandler(Getter&& handler) {
    get_dip_switch_config_handler_ = std::forward<Getter>(handler);
  }

  void RegisterSetDipSwitchConfigHandler(Setter&& handler) {
    set_dip_switch_config_handler_ = std::forward<Setter>(handler);
  }

  void RegisterDynamicIPEventHandler(Setter&& handler) {
    set_dynamic_ip_event_handler_ = std::forward<Setter>(handler);
  }

  void RegisterReloadHostConfEventHandler(Trigger&& handler) {
      reload_host_conf_handler_ = std::forward<Trigger>(handler);
  }

  void RegisterAddInterfaceHandler(Setter&& handler) {
      add_interface_handler_ = std::forward<Setter>(handler);
  }

  void RegisterDeleteInterfaceHandler(Setter&& handler) {
      delete_interface_handler_ = std::forward<Setter>(handler);
  }

 private:
  static gboolean SetBridgeConfig(netconfdInterface_config *object,
                                  GDBusMethodInvocation *invocation,
                                  const gchar *arg_config, gpointer user_data);

  static gboolean GetBridgeConfig(netconfdInterface_config *object,
                                  GDBusMethodInvocation *invocation,
                                  gpointer user_data);

  static gboolean GetDeviceInterfaces(netconfdInterface_config *object,
                                      GDBusMethodInvocation *invocation,
                                      gpointer user_data);

  static gboolean SetInterfaceConfig(netconfdInterface_config *object,
                                     GDBusMethodInvocation *invocation,
                                     const gchar *arg_config,
                                     gpointer user_data);

  static gboolean GetInterfaceConfig(netconfdInterface_config *object,
                                     GDBusMethodInvocation *invocation,
                                     gpointer user_data);

  static gboolean GetInterfaceStatuses(netconfdInterface_config *object,
                                     GDBusMethodInvocation *invocation,
                                     gpointer user_data);


  static gboolean SetAllIPConfig(netconfdIp_config *object,
                                 GDBusMethodInvocation *invocation,
                                 const gchar *arg_config, gpointer user_data);

  static gboolean SetIPConfig(netconfdIp_config *object,
                              GDBusMethodInvocation *invocation,
                              const gchar *arg_config, gpointer user_data);

  static gboolean GetAllIPConfig(netconfdIp_config *object,
                                 GDBusMethodInvocation *invocation,
                                 gpointer user_data);

  static gboolean GetAllCurrentIPConfig(netconfdIp_config *object,
                                       GDBusMethodInvocation *invocation,
                                       gpointer user_data);

  static gboolean TemporaryFixIP(netconfdIp_config *object,
                              GDBusMethodInvocation *invocation,
                              gpointer user_data);

  static gboolean SetTemporaryDHCPClientID(netconfdIp_config *object,
                              GDBusMethodInvocation *invocation,
                              const gchar *arg_config, gpointer user_data);

  static gboolean GetTemporaryDHCPClientID(netconfdIp_config *object,
                              GDBusMethodInvocation *invocation,
                              gpointer user_data);

  static gboolean GetDipSwitchConfig(netconfdIp_config *object,
                              GDBusMethodInvocation *invocation,
                              gpointer user_data);

  static gboolean SetDipSwitchConfig(netconfdIp_config *object,
                                 GDBusMethodInvocation *invocation,
                                 const gchar *arg_config, gpointer user_data);

  static gboolean DynamicIPEvent(netconfdEvent *object,
                                  GDBusMethodInvocation *invocation,
                                  const gchar *arg_config, gpointer user_data);

  static gboolean ReloadHostConf(_netconfdEvent *object, GDBusMethodInvocation *invocation,
                                  gpointer user_data);

  static gboolean GetBackupParamCount(netconfdBackup *object,
                                      GDBusMethodInvocation *invocation,
                                      gpointer user_data);

  static gboolean Backup(netconfdBackup *object,
                         GDBusMethodInvocation *invocation,
                         const gchar *arg_content, const gchar *arg_targetversion , gpointer user_data);

  static gboolean Restore(netconfdBackup *object,
                          GDBusMethodInvocation *invocation,
                          const gchar *arg_config, gpointer user_data);

  static gboolean AddInterface(netconfdInterface *object,
                               GDBusMethodInvocation *invocation,
                               const gchar *arg_config, gpointer user_data);

  static gboolean DeleteInterface(netconfdInterface *object,
                                  GDBusMethodInvocation *invocation,
                                  const gchar *arg_config, gpointer user_data);

  //interface_config
  netconfdObjectSkeleton *interface_config_object_;
  netconfdInterface_config* interface_config_;

  Setter set_bridge_config_handler_;
  Getter get_bridge_config_handler_;
  Getter get_device_interfaces_handler_;
  Setter set_interface_config_handler_;
  Getter get_interface_config_handler_;
  Getter get_interface_statuses_handler_;
  Trigger get_backup_param_count_handler_;


  //ip_config
  netconfdObjectSkeleton *ip_object_;
  netconfdIp_config* ip_config_;

  Setter set_all_ip_config_handler_;
  Setter set_ip_config_handler_;
  Getter get_all_ip_config_handler_;
  Getter get_all_current_ip_config_handler_;
  Getter get_dip_switch_config_handler_;
  Setter set_dip_switch_config_handler_;
  Trigger tempfixip_handler_;
  Setter set_tempdhcpclientid_handler_;
  Getter get_tempdhcpclientid_handler_;

  //event
  netconfdObjectSkeleton *event_object_;
  netconfdEvent* event_;
  Setter set_dynamic_ip_event_handler_;
  Trigger reload_host_conf_handler_;


  //backup & restore
  netconfdObjectSkeleton *backup_object_;
  netconfdBackup* backup_;
  ::std::function<::std::string(::std::string, ::std::string)> backup_handler_;
  Setter restore_handler_;

  // interface
  netconfdObjectSkeleton *interface_object_;
  netconfdInterface* interface_;
  Setter add_interface_handler_;
  Setter delete_interface_handler_;

};

} /* namespace dbus */
} /* namespace netconf */
