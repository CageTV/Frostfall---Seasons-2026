#include "Globals.h"

#include "Seasons.h"
#include "Settings.h"

namespace Globals
{
	namespace
	{
		constexpr auto pluginName = "Frostfall 2026 - Seasons.esp"sv;

		enum Id : std::size_t
		{
#define X(name) k##name,
#include "GlobalList.inc"
#undef X
			kCount
		};

		std::array<RE::TESGlobal*, kCount> globals{};
		bool                               present = false;

		void Set(Id a_id, float a_value)
		{
			if (const auto global = globals[a_id]) {
				global->value = a_value;
			}
		}
	}

	void Init()
	{
		const auto data = RE::TESDataHandler::GetSingleton();
		if (!data || !data->LookupModByName(pluginName)) {
			logger::info("{} is not loaded, the seasons are not mirrored into global variables", pluginName);
			return;
		}
		std::size_t found = 0;
		for (std::size_t i = 0; i < kCount; i++) {
			globals[i] = data->LookupForm<RE::TESGlobal>(static_cast<RE::FormID>(0x800 + i), pluginName);
			found += globals[i] ? 1 : 0;
		}
		present = found > 0;
		logger::info("{}: {} of {} global variables found", pluginName, found, static_cast<std::size_t>(kCount));
		if (found != kCount) {
			logger::warn("Some globals are missing: is this the plugin that came with this version of FrostfallSeasons.dll?");
		}
		Refresh();
	}

	void Refresh()
	{
		if (!present) {
			return;
		}
		const auto& s = Settings::Get();
		const auto  report = Seasons::GetReport();
		const int   month = Seasons::CurrentMonth();
		const int   type = s.monthType[month];

		float hour = 12.0f;
		if (const auto calendar = RE::Calendar::GetSingleton(); calendar && calendar->gameHour) {
			hour = calendar->gameHour->value;
		}

		Set(kEnabled, s.enabled ? 1.0f : 0.0f);
		Set(kMonth, static_cast<float>(month));
		Set(kMonthType, static_cast<float>(type));
		Set(kMonthTemperature, static_cast<float>(s.typeTemperature[type]));
		Set(kRegion, static_cast<float>(report.region));
		Set(kRegionBaseTemperature, static_cast<float>(report.baseTemperature));
		Set(kDayPhase, static_cast<float>(Seasons::DayPhase(hour)));
		Set(kDangerousMountain, s.dangerousThroat ? 1.0f : 0.0f);
		Set(kColdCities, s.coldCities ? 1.0f : 0.0f);

		Set(kTEMP_COLD, static_cast<float>(s.typeTemperature[Settings::kCold]));
		Set(kTEMP_CHILL, static_cast<float>(s.typeTemperature[Settings::kChill]));
		Set(kTEMP_BRISK, static_cast<float>(s.typeTemperature[Settings::kBrisk]));
		Set(kTEMP_NONE, static_cast<float>(s.typeTemperature[Settings::kNone]));
		Set(kTEMP_NICE, static_cast<float>(s.typeTemperature[Settings::kNice]));
		Set(kTEMP_WARM, static_cast<float>(s.typeTemperature[Settings::kWarm]));
		Set(kTEMP_HOT, static_cast<float>(s.typeTemperature[Settings::kHot]));

		Set(kREGION_UNKNOWN_BASE_TEMP, static_cast<float>(s.regionBase[Settings::kUnknown]));
		Set(kREGION_PINEFOREST_BASE_TEMP, static_cast<float>(s.regionBase[Settings::kPineForest]));
		Set(kREGION_VOLCANICTUNDRA_BASE_TEMP, static_cast<float>(s.regionBase[Settings::kVolcanicTundra]));
		Set(kREGION_FALLFOREST_BASE_TEMP, static_cast<float>(s.regionBase[Settings::kFallForest]));
		Set(kREGION_REACH_BASE_TEMP, static_cast<float>(s.regionBase[Settings::kReach]));
		Set(kREGION_TUNDRA_BASE_TEMP, static_cast<float>(s.regionBase[Settings::kTundra]));
		Set(kREGION_TUNDRAMARSH_BASE_TEMP, static_cast<float>(s.regionBase[Settings::kTundraMarsh]));
		Set(kREGION_COAST_BASE_TEMP, static_cast<float>(s.regionBase[Settings::kCoast]));
		Set(kREGION_SNOW_BASE_TEMP, static_cast<float>(s.regionBase[Settings::kSnow]));
		Set(kREGION_ASHWASTE_BASE_TEMP, static_cast<float>(s.regionBase[Settings::kAshWaste]));

		Set(kSunriseStartHour, s.sunriseStart);
		Set(kSunriseEndHour, s.sunriseEnd);
		Set(kSunsetStartHour, s.sunsetStart);
		Set(kSunsetEndHour, s.sunsetEnd);
		Set(kTwilightPenalty, static_cast<float>(s.twilightPenalty));
		Set(kNightPenalty, static_cast<float>(s.nightPenalty));
	}

	void Notify()
	{
		static int lastMonth = -1;
		static int lastVersion = -1;

		const int month = Seasons::CurrentMonth();
		const int version = Settings::version.load();
		const auto source = SKSE::GetModCallbackEventSource();

		if (source && lastMonth >= 0 && month != lastMonth) {
			SKSE::ModCallbackEvent event{ "FrostfallSeasons_MonthChanged", kMonthNames[month], static_cast<float>(month), nullptr };
			source->SendEvent(&event);
		}
		if (source && lastVersion >= 0 && version != lastVersion) {
			SKSE::ModCallbackEvent event{ "FrostfallSeasons_SettingsChanged", "", static_cast<float>(version), nullptr };
			source->SendEvent(&event);
		}
		lastMonth = month;
		lastVersion = version;
	}
}
