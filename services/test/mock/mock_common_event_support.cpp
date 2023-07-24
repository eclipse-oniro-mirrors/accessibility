/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common_event_support.h"

namespace OHOS {
namespace EventFwk {

const std::string CommonEventSupport::COMMON_EVENT_BOOT_COMPLETED = "usual.event.BOOT_COMPLETED";
const std::string CommonEventSupport::COMMON_EVENT_LOCKED_BOOT_COMPLETED = "usual.event.LOCKED_BOOT_COMPLETED";
const std::string CommonEventSupport::COMMON_EVENT_SHUTDOWN = "usual.event.SHUTDOWN";
const std::string CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED = "usual.event.BATTERY_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_BATTERY_LOW = "usual.event.BATTERY_LOW";
const std::string CommonEventSupport::COMMON_EVENT_BATTERY_OKAY = "usual.event.BATTERY_OKAY";
const std::string CommonEventSupport::COMMON_EVENT_POWER_CONNECTED = "usual.event.POWER_CONNECTED";
const std::string CommonEventSupport::COMMON_EVENT_POWER_DISCONNECTED = "usual.event.POWER_DISCONNECTED";
const std::string CommonEventSupport::COMMON_EVENT_SCREEN_OFF = "usual.event.SCREEN_OFF";
const std::string CommonEventSupport::COMMON_EVENT_SCREEN_ON = "usual.event.SCREEN_ON";
const std::string CommonEventSupport::COMMON_EVENT_USER_PRESENT = "usual.event.USER_PRESENT";
const std::string CommonEventSupport::COMMON_EVENT_TIME_TICK = "usual.event.TIME_TICK";
const std::string CommonEventSupport::COMMON_EVENT_TIME_CHANGED = "usual.event.TIME_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_DATE_CHANGED = "usual.event.DATE_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED = "usual.event.TIMEZONE_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_CLOSE_SYSTEM_DIALOGS = "usual.event.CLOSE_SYSTEM_DIALOGS";
const std::string CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED = "usual.event.PACKAGE_ADDED";
const std::string CommonEventSupport::COMMON_EVENT_PACKAGE_REPLACED = "usual.event.PACKAGE_REPLACED";
const std::string CommonEventSupport::COMMON_EVENT_MY_PACKAGE_REPLACED = "usual.event.MY_PACKAGE_REPLACED";
const std::string CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED = "usual.event.PACKAGE_REMOVED";
const std::string CommonEventSupport::COMMON_EVENT_PACKAGE_FULLY_REMOVED = "usual.event.PACKAGE_FULLY_REMOVED";
const std::string CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED = "usual.event.PACKAGE_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_PACKAGE_RESTARTED = "usual.event.PACKAGE_RESTARTED";
const std::string CommonEventSupport::COMMON_EVENT_PACKAGE_DATA_CLEARED = "usual.event.PACKAGE_DATA_CLEARED";
const std::string CommonEventSupport::COMMON_EVENT_PACKAGES_SUSPENDED = "usual.event.PACKAGES_SUSPENDED";
const std::string CommonEventSupport::COMMON_EVENT_PACKAGES_UNSUSPENDED = "usual.event.PACKAGES_UNSUSPENDED";
const std::string CommonEventSupport::COMMON_EVENT_MY_PACKAGE_SUSPENDED = "usual.event.MY_PACKAGE_SUSPENDED";
const std::string CommonEventSupport::COMMON_EVENT_MY_PACKAGE_UNSUSPENDED = "usual.event.MY_PACKAGE_UNSUSPENDED";
const std::string CommonEventSupport::COMMON_EVENT_UID_REMOVED = "usual.event.UID_REMOVED";
const std::string CommonEventSupport::COMMON_EVENT_PACKAGE_FIRST_LAUNCH = "usual.event.PACKAGE_FIRST_LAUNCH";
const std::string CommonEventSupport::COMMON_EVENT_PACKAGE_NEEDS_VERIFICATION =
    "usual.event.PACKAGE_NEEDS_VERIFICATION";
const std::string CommonEventSupport::COMMON_EVENT_PACKAGE_VERIFIED = "usual.event.PACKAGE_VERIFIED";
const std::string CommonEventSupport::COMMON_EVENT_EXTERNAL_APPLICATIONS_AVAILABLE =
    "usual.event.EXTERNAL_APPLICATIONS_AVAILABLE";
const std::string CommonEventSupport::COMMON_EVENT_EXTERNAL_APPLICATIONS_UNAVAILABLE =
    "usual.event.EXTERNAL_APPLICATIONS_UNAVAILABLE";
const std::string CommonEventSupport::COMMON_EVENT_CONFIGURATION_CHANGED = "usual.event.CONFIGURATION_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_LOCALE_CHANGED = "usual.event.LOCALE_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_MANAGE_PACKAGE_STORAGE = "usual.event.MANAGE_PACKAGE_STORAGE";
const std::string CommonEventSupport::COMMON_EVENT_DRIVE_MODE = "common.event.DRIVE_MODE";
const std::string CommonEventSupport::COMMON_EVENT_HOME_MODE = "common.event.HOME_MODE";
const std::string CommonEventSupport::COMMON_EVENT_OFFICE_MODE = "common.event.OFFICE_MODE";
const std::string CommonEventSupport::COMMON_EVENT_USER_STARTED = "usual.event.USER_STARTED";
const std::string CommonEventSupport::COMMON_EVENT_USER_BACKGROUND = "usual.event.USER_BACKGROUND";
const std::string CommonEventSupport::COMMON_EVENT_USER_FOREGROUND = "usual.event.USER_FOREGROUND";
const std::string CommonEventSupport::COMMON_EVENT_USER_SWITCHED = "usual.event.USER_SWITCHED";
const std::string CommonEventSupport::COMMON_EVENT_USER_STARTING = "usual.event.USER_STARTING";
const std::string CommonEventSupport::COMMON_EVENT_USER_UNLOCKED = "usual.event.USER_UNLOCKED";
const std::string CommonEventSupport::COMMON_EVENT_USER_STOPPING = "usual.event.USER_STOPPING";
const std::string CommonEventSupport::COMMON_EVENT_USER_STOPPED = "usual.event.USER_STOPPED";
const std::string CommonEventSupport::COMMON_EVENT_HWID_LOGIN = "common.event.HWID_LOGIN";
const std::string CommonEventSupport::COMMON_EVENT_HWID_LOGOUT = "common.event.HWID_LOGOUT";
const std::string CommonEventSupport::COMMON_EVENT_HWID_TOKEN_INVALID = "common.event.HWID_TOKEN_INVALID";
const std::string CommonEventSupport::COMMON_EVENT_HWID_LOGOFF = "common.event.HWID_LOGOFF";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_POWER_STATE = "usual.event.wifi.POWER_STATE";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_SCAN_FINISHED = "usual.event.wifi.SCAN_FINISHED";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_RSSI_VALUE = "usual.event.wifi.RSSI_VALUE";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_CONN_STATE = "usual.event.wifi.CONN_STATE";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_HOTSPOT_STATE = "usual.event.wifi.HOTSPOT_STATE";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_AP_STA_JOIN = "usual.event.wifi.WIFI_HS_STA_JOIN";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_AP_STA_LEAVE = "usual.event.wifi.WIFI_HS_STA_LEAVE";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_MPLINK_STATE_CHANGE = "usual.event.wifi.mplink.STATE_CHANGE";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_P2P_CONN_STATE = "usual.event.wifi.p2p.CONN_STATE_CHANGE";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_P2P_STATE_CHANGED = "usual.event.wifi.p2p.STATE_CHANGE";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_P2P_PEERS_STATE_CHANGED = "usual.event.wifi.p2p.DEVICES_CHANGE";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_P2P_PEERS_DISCOVERY_STATE_CHANGED =
    "usual.event.wifi.p2p.PEER_DISCOVERY_STATE_CHANGE";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_P2P_CURRENT_DEVICE_STATE_CHANGED =
    "usual.event.wifi.p2p.CURRENT_DEVICE_CHANGE";
const std::string CommonEventSupport::COMMON_EVENT_WIFI_P2P_GROUP_STATE_CHANGED =
    "usual.event.wifi.p2p.GROUP_STATE_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HANDSFREE_AG_CONNECT_STATE_UPDATE =
    "usual.event.bluetooth.handsfree.ag.CONNECT_STATE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HANDSFREE_AG_CURRENT_DEVICE_UPDATE =
    "usual.event.bluetooth.handsfree.ag.CURRENT_DEVICE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HANDSFREE_AG_AUDIO_STATE_UPDATE =
    "usual.event.bluetooth.handsfree.ag.AUDIO_STATE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_A2DPSOURCE_CONNECT_STATE_UPDATE =
    "usual.event.bluetooth.a2dpsource.CONNECT_STATE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_A2DPSOURCE_CURRENT_DEVICE_UPDATE =
    "usual.event.bluetooth.a2dpsource.CURRENT_DEVICE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_A2DPSOURCE_PLAYING_STATE_UPDATE =
    "usual.event.bluetooth.a2dpsource.PLAYING_STATE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_A2DPSOURCE_AVRCP_CONNECT_STATE_UPDATE =
    "usual.event.bluetooth.a2dpsource.AVRCP_CONNECT_STATE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_A2DPSOURCE_CODEC_VALUE_UPDATE =
    "usual.event.bluetooth.a2dpsource.CODEC_VALUE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_DISCOVERED =
    "usual.event.bluetooth.remotedevice.DISCOVERED";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_CLASS_VALUE_UPDATE =
    "usual.event.bluetooth.remotedevice.CLASS_VALUE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_ACL_CONNECTED =
    "usual.event.bluetooth.remotedevice.ACL_CONNECTED";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_ACL_DISCONNECTED =
    "usual.event.bluetooth.remotedevice.ACL_DISCONNECTED";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_NAME_UPDATE =
    "usual.event.bluetooth.remotedevice.NAME_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_PAIR_STATE =
    "usual.event.bluetooth.remotedevice.PAIR_STATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_BATTERY_VALUE_UPDATE =
    "usual.event.bluetooth.remotedevice.BATTERY_VALUE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_SDP_RESULT =
    "usual.event.bluetooth.remotedevice.SDP_RESULT";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_UUID_VALUE =
    "usual.event.bluetooth.remotedevice.UUID_VALUE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_PAIRING_REQ =
    "usual.event.bluetooth.remotedevice.PAIRING_REQ";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_PAIRING_CANCEL =
    "usual.event.bluetooth.remotedevice.PAIRING_CANCEL";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_CONNECT_REQ =
    "usual.event.bluetooth.remotedevice.CONNECT_REQ";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_CONNECT_REPLY =
    "usual.event.bluetooth.remotedevice.CONNECT_REPLY";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_REMOTEDEVICE_CONNECT_CANCEL =
    "usual.event.bluetooth.remotedevice.CONNECT_CANCEL";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HANDSFREEUNIT_CONNECT_STATE_UPDATE =
    "usual.event.bluetooth.handsfreeunit.CONNECT_STATE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HANDSFREEUNIT_AUDIO_STATE_UPDATE =
    "usual.event.bluetooth.handsfreeunit.AUDIO_STATE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HANDSFREEUNIT_AG_COMMON_EVENT =
    "usual.event.bluetooth.handsfreeunit.AG_COMMON_EVENT";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HANDSFREEUNIT_AG_CALL_STATE_UPDATE =
    "usual.event.bluetooth.handsfreeunit.AG_CALL_STATE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HOST_STATE_UPDATE =
    "usual.event.bluetooth.host.STATE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HOST_REQ_DISCOVERABLE =
    "usual.event.bluetooth.host.REQ_DISCOVERABLE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HOST_REQ_ENABLE = "usual.event.bluetooth.host.REQ_ENABLE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HOST_REQ_DISABLE =
    "usual.event.bluetooth.host.REQ_DISABLE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HOST_SCAN_MODE_UPDATE =
    "usual.event.bluetooth.host.SCAN_MODE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HOST_DISCOVERY_STARTED =
    "usual.event.bluetooth.host.DISCOVERY_STARTED";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HOST_DISCOVERY_FINISHED =
    "usual.event.bluetooth.host.DISCOVERY_FINISHED";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_HOST_NAME_UPDATE =
    "usual.event.bluetooth.host.NAME_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_A2DPSINK_CONNECT_STATE_UPDATE =
    "usual.event.bluetooth.a2dpsink.CONNECT_STATE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_A2DPSINK_PLAYING_STATE_UPDATE =
    "usual.event.bluetooth.a2dpsink.PLAYING_STATE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_BLUETOOTH_A2DPSINK_AUDIO_STATE_UPDATE =
    "usual.event.bluetooth.a2dpsink.AUDIO_STATE_UPDATE";
const std::string CommonEventSupport::COMMON_EVENT_NFC_ACTION_ADAPTER_STATE_CHANGED =
    "usual.event.nfc.action.ADAPTER_STATE_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_NFC_ACTION_RF_FIELD_ON_DETECTED =
    "usual.event.nfc.action.RF_FIELD_ON_DETECTED";
const std::string CommonEventSupport::COMMON_EVENT_NFC_ACTION_RF_FIELD_OFF_DETECTED =
    "usual.event.nfc.action.RF_FIELD_OFF_DETECTED";
const std::string CommonEventSupport::COMMON_EVENT_DISCHARGING = "usual.event.DISCHARGING";
const std::string CommonEventSupport::COMMON_EVENT_CHARGING = "usual.event.CHARGING";
const std::string CommonEventSupport::COMMON_EVENT_DEVICE_IDLE_MODE_CHANGED = "usual.event.DEVICE_IDLE_MODE_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_POWER_SAVE_MODE_CHANGED = "usual.event.POWER_SAVE_MODE_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_USER_ADDED = "usual.event.USER_ADDED";
const std::string CommonEventSupport::COMMON_EVENT_USER_REMOVED = "usual.event.USER_REMOVED";
const std::string CommonEventSupport::COMMON_EVENT_ABILITY_ADDED = "common.event.ABILITY_ADDED";
const std::string CommonEventSupport::COMMON_EVENT_ABILITY_REMOVED = "common.event.ABILITY_REMOVED";
const std::string CommonEventSupport::COMMON_EVENT_ABILITY_UPDATED = "common.event.ABILITY_UPDATED";
const std::string CommonEventSupport::COMMON_EVENT_LOCATION_MODE_STATE_CHANGED =
    "usual.event.location.MODE_STATE_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_IVI_SLEEP = "common.event.IVI_SLEEP";
const std::string CommonEventSupport::COMMON_EVENT_IVI_PAUSE = "common.event.IVI_PAUSE";
const std::string CommonEventSupport::COMMON_EVENT_IVI_STANDBY = "common.event.IVI_STANDBY";
const std::string CommonEventSupport::COMMON_EVENT_IVI_LASTMODE_SAVE = "common.event.IVI_LASTMODE_SAVE";
const std::string CommonEventSupport::COMMON_EVENT_IVI_VOLTAGE_ABNORMAL = "common.event.IVI_VOLTAGE_ABNORMAL";
const std::string CommonEventSupport::COMMON_EVENT_IVI_HIGH_TEMPERATURE = "common.event.IVI_HIGH_TEMPERATURE";
const std::string CommonEventSupport::COMMON_EVENT_IVI_EXTREME_TEMPERATURE = "common.event.IVI_EXTREME_TEMPERATURE";
const std::string CommonEventSupport::COMMON_EVENT_IVI_TEMPERATURE_ABNORMAL = "common.event.IVI_TEMPERATURE_ABNORMAL";
const std::string CommonEventSupport::COMMON_EVENT_IVI_VOLTAGE_RECOVERY = "common.event.IVI_VOLTAGE_RECOVERY";
const std::string CommonEventSupport::COMMON_EVENT_IVI_TEMPERATURE_RECOVERY = "common.event.IVI_TEMPERATURE_RECOVERY";
const std::string CommonEventSupport::COMMON_EVENT_IVI_ACTIVE = "common.event.IVI_ACTIVE";
const std::string CommonEventSupport::COMMON_EVENT_USB_STATE = "usual.event.hardware.usb.action.USB_STATE";
const std::string CommonEventSupport::COMMON_EVENT_USB_PORT_CHANGED =
    "usual.event.hardware.usb.action.USB_PORT_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_USB_DEVICE_ATTACHED =
    "usual.event.hardware.usb.action.USB_DEVICE_ATTACHED";
const std::string CommonEventSupport::COMMON_EVENT_USB_DEVICE_DETACHED =
    "usual.event.hardware.usb.action.USB_DEVICE_DETACHED";
const std::string CommonEventSupport::COMMON_EVENT_USB_ACCESSORY_ATTACHED =
    "usual.event.hardware.usb.action.USB_ACCESSORY_ATTACHED";
const std::string CommonEventSupport::COMMON_EVENT_USB_ACCESSORY_DETACHED =
    "usual.event.hardware.usb.action.USB_ACCESSORY_DETACHED";
const std::string CommonEventSupport::COMMON_EVENT_DEVICE_STORAGE_LOW = "usual.event.DEVICE_STORAGE_LOW";
const std::string CommonEventSupport::COMMON_EVENT_DEVICE_STORAGE_OK = "usual.event.DEVICE_STORAGE_OK";
const std::string CommonEventSupport::COMMON_EVENT_DEVICE_STORAGE_FULL = "usual.event.DEVICE_STORAGE_FULL";
const std::string CommonEventSupport::COMMON_EVENT_CONNECTIVITY_CHANGE = "usual.event.CONNECTIVITY_CHANGE";
const std::string CommonEventSupport::COMMON_EVENT_DISK_REMOVED = "usual.event.data.DISK_REMOVED";
const std::string CommonEventSupport::COMMON_EVENT_DISK_UNMOUNTED = "usual.event.data.DISK_UNMOUNTED";
const std::string CommonEventSupport::COMMON_EVENT_DISK_MOUNTED = "usual.event.data.DISK_MOUNTED";
const std::string CommonEventSupport::COMMON_EVENT_DISK_BAD_REMOVAL = "usual.event.data.DISK_BAD_REMOVAL";
const std::string CommonEventSupport::COMMON_EVENT_DISK_UNMOUNTABLE = "usual.event.data.DISK_UNMOUNTABLE";
const std::string CommonEventSupport::COMMON_EVENT_DISK_EJECT = "usual.event.data.DISK_EJECT";
const std::string CommonEventSupport::COMMON_EVENT_VISIBLE_ACCOUNTS_UPDATED =
    "usual.event.data.VISIBLE_ACCOUNTS_UPDATED";
const std::string CommonEventSupport::COMMON_EVENT_ACCOUNT_DELETED = "usual.event.data.ACCOUNT_DELETED";
const std::string CommonEventSupport::COMMON_EVENT_FOUNDATION_READY = "common.event.FOUNDATION_READY";
const std::string CommonEventSupport::COMMON_EVENT_SIM_CARD_DEFAULT_VOICE_SUBSCRIPTION_CHANGED =
    "usual.event.SIM.DEFAULT_VOICE_SUBSCRIPTION_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_SIM_STATE_CHANGED = "usual.event.SIM.CARD_STATE_CHANGED";
const std::string CommonEventSupport::COMMON_EVENT_AIRPLANE_MODE_CHANGED = "usual.event.AIRPLANE_MODE";
const std::string CommonEventSupport::COMMON_EVENT_TEST_ACTION1 = "usual.event.test1";
const std::string CommonEventSupport::COMMON_EVENT_TEST_ACTION2 = "usual.event.test2";

CommonEventSupport::CommonEventSupport()
{
    Init();
}

CommonEventSupport::~CommonEventSupport()
{}

void CommonEventSupport::Init()
{
    commonEventSupport_.emplace_back(CommonEventSupport::COMMON_EVENT_USER_PRESENT);
    commonEventSupport_.emplace_back(CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    commonEventSupport_.emplace_back(CommonEventSupport::COMMON_EVENT_PACKAGE_REPLACED);
    commonEventSupport_.emplace_back(CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    commonEventSupport_.emplace_back(CommonEventSupport::COMMON_EVENT_PACKAGE_FULLY_REMOVED);
    commonEventSupport_.emplace_back(CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    commonEventSupport_.emplace_back(CommonEventSupport::COMMON_EVENT_PACKAGE_RESTARTED);
    commonEventSupport_.emplace_back(CommonEventSupport::COMMON_EVENT_PACKAGE_DATA_CLEARED);
    commonEventSupport_.emplace_back(CommonEventSupport::COMMON_EVENT_PACKAGES_SUSPENDED);
    commonEventSupport_.emplace_back(CommonEventSupport::COMMON_EVENT_PACKAGES_UNSUSPENDED);
    commonEventSupport_.emplace_back(CommonEventSupport::COMMON_EVENT_PACKAGE_FIRST_LAUNCH);
    commonEventSupport_.emplace_back(CommonEventSupport::COMMON_EVENT_PACKAGE_NEEDS_VERIFICATION);
    commonEventSupport_.emplace_back(CommonEventSupport::COMMON_EVENT_PACKAGE_VERIFIED);
    return;
}

bool CommonEventSupport::IsSystemEvent(std::string& str)
{
    return true;
}
} // namespace EventFwk
} // namespace OHOS