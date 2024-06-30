#pragma once

#include "json.h"
#include "json_conversions.h"
#include "settings/settingsOption.h"
#include "settings_extra.h"
#include "settings_apps.h"
#include "settings_widgets.h"
#include "utilities/logging.h"
#include <Preferences.h>
#include <vector>
#include <map>

using json = nlohmann::json;

struct wifi_station
{
		String wifi_ssid = "";
		String wifi_pass = "";
		uint8_t channel = 9;
};

// Save data struct
struct Config
{
		int ver = 1;
		bool first_time = true;

		bool wifi_start = false;
		String wifi_ssid = "";
		String wifi_pass = "";

		std::vector<wifi_station> wifi_options;

		String mdns_name = "tinywatch";
		bool website_darkmode = true;

		// UTC and Country settings - country needed for some widgets like Open Weather
		String country = "";
		String city = "";
		int16_t utc_offset = 999;

		// Display
		std::vector<int> bl_period_vbus = {120, 120, 120}; // Seconds
		std::vector<int> bl_period_vbat = {30, 45, 60};	   // Seconds

		std::vector<int> bl_level_vbus = {100, 80, 60}; // percentage (0-100)
		std::vector<int> bl_level_vbat = {100, 60, 30}; // percentage (0-100)

		// Time
		bool time_24hour = false;
		bool time_dateformat = false; // False - DMY, True - MDY
		uint8_t clock_face_index = 0;

		// Watch
		bool left_handed = true;
		bool flipped = false;
		bool show_nav_arrows = true;

		// Sound
		bool audio_ui = true;
		bool audio_alarm = true;
		bool audio_on_hour = false;
		bool audio_test = false;

		// IMU
		bool imu_process_steps = true;
		bool imu_process_wrist = false;

		/*
		 * Additional settings stucts go here
		 */

		// MQTT specific settings - see Struct above
		Config_mqtt mqtt;

		// Haptics specific settings
		Config_haptics haptics;

		// Binary Clock Face specific settings
		Config_custom_binary custom_binary;

		/*
		 * Widget Settings Structs
		 */

		// Battery/FG specific settings - see Struct above
		Config_widget_battery battery;

		// Open Weather specific settings - see Struct above
		Config_widget_open_weather open_weather;

		/*
		 * App Settings Structs
		 */

		// Microphone App specific Settings
		Config_app_microphone app_microphone;

		// Compass specific settings
		Config_app_compass compass;

		json last_saved_data;
};

enum SettingType
{
	CONTROL,
	WIDGET,
	MQTT
};

struct setting_group
{
		String name = "";
		String description = "";
		std::vector<SettingsOptionBase *> groups = {};
		SettingType type = SettingType::CONTROL;

		void setup(String nm, SettingType t, String d = "")
		{
			name = nm;
			type = t;
			description = d;
		};
};

class Settings
{

	public:
		Config config;
		std::vector<setting_group> settings_groups;

		Settings()
		{
			// Setup settings groups
			settings_groups[0].setup("General Watch Settings", SettingType::CONTROL);

			settings_groups[1].setup("Audio Settings", SettingType::CONTROL);

			settings_groups[2].setup("Haptics Settings", SettingType::CONTROL, "Haptics are only available in TinyWATCH revision P7 and later.");

			settings_groups[3].setup("Display Settings", SettingType::CONTROL, "Your watch has 3 steps of brightness, based on the duration at each step. Step 1 is the highest brightest and step 3 is the lowest.<br>At each duration interval, the brightness will reduce to the next step.<br>This is to conserve battery capacity as the backlight draws more current the brighter it is.<br>Touching the screen will automatically set the display back to step 1.");

			settings_groups[4].setup("Open Weather Settings", SettingType::WIDGET, "Add your Open Weather API key here to be able to see your current weather details on your watch face.");

			settings_groups[5].setup("MQTT Settings", SettingType::WIDGET);
		}

		bool load();
		bool save(bool force);
		bool backup();
		bool create();
		void print_file(void);
		bool has_wifi_creds(void);
		bool has_country_set(void);
		void update_wifi_credentials(String ssid, String pass);
		String get_load_status();
		String get_save_status();

		// Add any SettingsOption values here for any settings you want to bind with a tw_Control
		SettingsOptionBool setting_wifi_start{&config.wifi_start, 0, "Auto WiFi", "NO", "YES"};
		SettingsOptionBool setting_time_24hour{&config.time_24hour, 0, "Time Mode", "12H", "24H"};
		SettingsOptionBool setting_time_dateformat{&config.time_dateformat, 0, "Date FMT", "DMY", "MDY"};
		SettingsOptionBool setting_left_handed{&config.left_handed, 0, "Handed", "LEFT", "RIGHT"};
		SettingsOptionBool setting_flipped{&config.flipped, 0, "Flipped", "NO", "YES"};
		SettingsOptionBool setting_nav_arrows{&config.show_nav_arrows, 0, "Nav Arrows", "HIDE", "SHOW"};

		SettingsOptionBool setting_audio_ui{&config.audio_ui, 1, "UI Sound", "NO", "YES"};
		SettingsOptionBool setting_audio_alarm{&config.audio_alarm, 1, "Alarm Sound", "NO", "YES"};
		SettingsOptionBool setting_audio_on_hour{&config.audio_on_hour, 1, "Beep Hour", "NO", "YES"};

		// haptics
		SettingsOptionBool setting_haptics_enabled{&config.haptics.enabled, 2, "Enabled", "NO", "YES"};
		SettingsOptionBool setting_haptics_trig_boot{&config.haptics.trigger_on_boot, 2, "On Boot", "NO", "YES"};
		SettingsOptionBool setting_haptics_trig_wake{&config.haptics.trigger_on_wake, 2, "On Wake", "NO", "YES"};
		SettingsOptionBool setting_haptics_trig_alarm{&config.haptics.trigger_on_alarm, 2, "On Alarm", "NO", "YES"};
		SettingsOptionBool setting_haptics_trig_hour{&config.haptics.trigger_on_hour, 2, "On Hour", "NO", "YES"};
		SettingsOptionBool setting_haptics_trig_event{&config.haptics.trigger_on_event, 2, "On Event", "NO", "YES"};
		SettingsOptionBool setting_haptics_trig_longpress{&config.haptics.trigger_on_longpress, 2, "LongPress", "NO", "YES"};

		// Display
		SettingsOptionIntVector setting_bl_level_vbus{&config.bl_level_vbus, 0, 100, 10, false, 3, "Backlight brightness on USB (%%)"};		// need %% to have it not be PARSED by the string processor :(
		SettingsOptionIntVector setting_bl_level_vbat{&config.bl_level_vbat, 0, 100, 10, false, 3, "Backlight brightness on Battery (%%)"}; // need %% to have it not be PARSED by the string processor :(
		SettingsOptionIntVector setting_bl_period_vbus{&config.bl_period_vbus, 0, 600, 1, false, 3, "Backlight duration on USB (Sec)"};
		SettingsOptionIntVector setting_bl_period_vbat{&config.bl_period_vbat, 0, 600, 1, false, 3, "Backlight duration on Battery (Sec)"};

		// Open Weather
		SettingsOptionBool widget_ow_enabled{&config.open_weather.enabled, 4, "Enabled", "NO", "YES"};
		SettingsOptionString widget_ow_apikey{&config.open_weather.api_key, 4, "API KEY", false};
		SettingsOptionIntRange widget_ow_poll_interval{&config.open_weather.poll_frequency, 10, 300, 10, false, 4, "Poll Interval (Min)"};
		SettingsOptionBool widget_ow_units{&config.open_weather.units_metric, 4, "Temperature Units", "Fahrenheit", "Celsius"};

		// MQTT
		SettingsOptionBool mqtt_enabled{&config.mqtt.enabled, 5, "Enabled", "NO", "YES"};
		SettingsOptionString mqtt_broker_ip{&config.mqtt.broker_ip, 5, "Broker IP"};
		SettingsOptionInt mqtt_broker_port{&config.mqtt.broker_port, 1, 2000, false, 5, "Broker Port"};
		SettingsOptionString mqtt_username{&config.mqtt.username, 5, "Username", false};
		SettingsOptionString mqtt_password{&config.mqtt.password, 5, "Password", false};
		SettingsOptionString mqtt_device_name{&config.mqtt.device_name, 5, "Device Name"};
		SettingsOptionString mqtt_topic_listen{&config.mqtt.topic_listen, 5, "Listen Topic"};

		bool ui_forced_save = false; //

	private:
		static constexpr const char *filename = "/settings.json";
		static constexpr const char *tmp_filename = "/tmp_settings.json";
		static constexpr const char *log = "/log.txt";
		static constexpr const char *backup_prefix = "settings_back_";
		static const int max_backups = 10;
		static long backupNumber(const String);

		unsigned long max_time_between_saves = 30000; // every 30 seconds
		unsigned long last_save_time = 0;

		void log_to_nvs(const char *key, const char *log);
		Preferences nvs;
};

extern Settings settings;
