#include "Seasons.h"

#include "Globals.h"
#include "Polygons.h"
#include "Settings.h"

namespace Seasons
{
	namespace
	{
		std::atomic<int>  reportRegion{ Code::kUnknown };
		std::atomic<int>  reportBase{ 0 };
		std::atomic<int>  reportCurrent{ 0 };
		std::atomic<bool> reportSeen{ false };

		// http://alienryderflex.com/polygon/, the test Frostfall's own climate script uses
		bool Inside(const Polygons::Polygon& a_polygon, float a_x, float a_y)
		{
			bool inside = false;
			for (std::size_t i = 0, j = a_polygon.count - 1; i < a_polygon.count; j = i++) {
				const float xi = a_polygon.x[i], yi = a_polygon.y[i], xj = a_polygon.x[j], yj = a_polygon.y[j];
				if (((yi > a_y) != (yj > a_y)) && (a_x < (xj - xi) * (a_y - yi) / (yj - yi) + xi)) {
					inside = !inside;
				}
			}
			return inside;
		}

		int KindFromCode(int a_code)
		{
			switch (a_code) {
			case Code::kPineForest: return Settings::kPineForest;
			case Code::kVolcanicTundra: return Settings::kVolcanicTundra;
			case Code::kFallForest: return Settings::kFallForest;
			case Code::kReach: return Settings::kReach;
			case Code::kTundra: return Settings::kTundra;
			case Code::kTundraMarsh: return Settings::kTundraMarsh;
			case Code::kCoast: return Settings::kCoast;
			case Code::kSnow: return Settings::kSnow;
			default: return Settings::kUnknown;
			}
		}
	}

	int CurrentMonth()
	{
		const auto calendar = RE::Calendar::GetSingleton();
		if (!calendar || !calendar->gameMonth) {
			return 0;
		}
		return std::clamp(static_cast<int>(calendar->gameMonth->value), 0, Settings::kMonths - 1);
	}

	int RegionKind(int a_code, float a_x, float a_y, float a_z)
	{
		switch (a_code) {
		case Code::kOblivion:
			return -1;
		case Code::kFalmerValley:
			// the order of the original Frostfall (and of Frostfall - Seasons): the boss's cave is only the boss's above its floor
			if (Inside(Polygons::kFalmerBoss, a_x, a_y) && a_z >= 8385.0f) {
				return Settings::kPineForest;
			}
			if (Inside(Polygons::kFalmerIce, a_x, a_y)) {
				return Settings::kSnow;
			}
			if (Inside(Polygons::kFalmerCanyon, a_x, a_y)) {
				return Settings::kReach;
			}
			if (Inside(Polygons::kFalmerPlayground, a_x, a_y)) {
				return Settings::kCoast;
			}
			if (Inside(Polygons::kFalmerGrove, a_x, a_y)) {
				return Settings::kTundraMarsh;
			}
			return Settings::kUnknown;
		case Code::kSolstheim:
			return Inside(Polygons::kSolstheimArctic, a_x, a_y) ? Settings::kSnow : Settings::kAshWaste;
		case Code::kDarkend:
			return Inside(Polygons::kDarkendFrozen, a_x, a_y) ? Settings::kSnow : Settings::kTundraMarsh;
		case Code::kWyrmstooth:
			if (Inside(Polygons::kWyrmstoothCenter, a_x, a_y) || Inside(Polygons::kWyrmstoothWest, a_x, a_y)) {
				return Settings::kTundra;
			}
			return Settings::kSnow;
		default:
			return KindFromCode(a_code);
		}
	}

	int RegionBaseTemperature(int a_code, float a_x, float a_y, float a_z, int a_month)
	{
		const auto& s = Settings::Get();
		if (!s.enabled) {
			return kNotMine;
		}
		const int kind = RegionKind(a_code, a_x, a_y, a_z);
		if (kind < 0) {
			return kNotMine;
		}
		const int month = std::clamp(a_month, 0, Settings::kMonths - 1);
		return s.regionBase[kind] + s.typeTemperature[s.monthType[month]];
	}

	Phase DayPhase(float a_hour)
	{
		const auto& s = Settings::Get();
		if (a_hour > s.sunsetEnd || a_hour < s.sunriseStart) {
			return kNight;
		}
		if ((a_hour >= s.sunsetStart && a_hour <= s.sunsetEnd) || (a_hour >= s.sunriseStart && a_hour <= s.sunriseEnd)) {
			return kTwilight;
		}
		return kDay;
	}

	int PhasePenalty(int a_phase)
	{
		const auto& s = Settings::Get();
		return a_phase == kNight ? s.nightPenalty : (a_phase == kTwilight ? s.twilightPenalty : 0);
	}

	bool InThroatOfTheWorld(float a_x, float a_y)
	{
		return a_x >= 26624.0f && a_x <= 73731.0f && a_y >= -79871.0f && a_y <= -20479.0f && Inside(Polygons::kThroat, a_x, a_y);
	}

	Report GetReport()
	{
		return { reportRegion.load(), reportBase.load(), reportCurrent.load(), reportSeen.load() };
	}

	// ---- Papyrus: FrostfallSeasonsNative ----
	namespace
	{
		bool IsActive(RE::StaticFunctionTag*) { return Settings::Get().enabled; }
		std::int32_t GetCurrentMonth(RE::StaticFunctionTag*) { return CurrentMonth(); }
		std::int32_t GetRegionBaseTemperature(RE::StaticFunctionTag*, std::int32_t a_region, float a_x, float a_y, float a_z)
		{
			return RegionBaseTemperature(a_region, a_x, a_y, a_z, CurrentMonth());
		}
		std::int32_t GetDayPhase(RE::StaticFunctionTag*, float a_hour) { return DayPhase(a_hour); }
		std::int32_t GetPhasePenalty(RE::StaticFunctionTag*, std::int32_t a_phase) { return PhasePenalty(a_phase); }
		bool IsInThroatOfTheWorld(RE::StaticFunctionTag*, float a_x, float a_y) { return InThroatOfTheWorld(a_x, a_y); }
		bool DangerousMountain(RE::StaticFunctionTag*) { return Settings::Get().dangerousThroat; }
		bool ColdCities(RE::StaticFunctionTag*) { return Settings::Get().coldCities; }
		std::int32_t GetSettingsVersion(RE::StaticFunctionTag*) { return Settings::version.load(); }
		void ReportClimate(RE::StaticFunctionTag*, std::int32_t a_region, std::int32_t a_base, std::int32_t a_current)
		{
			reportRegion = a_region;
			reportBase = a_base;
			reportCurrent = a_current;
			reportSeen = true;
			// the globals of the optional plugin and the mod events are written on the main thread
			SKSE::GetTaskInterface()->AddTask([]() {
				Globals::Refresh();
				Globals::Notify();
			});
		}

		bool Register(RE::BSScript::IVirtualMachine* a_vm)
		{
			constexpr auto cls = "FrostfallSeasonsNative"sv;
			a_vm->RegisterFunction("IsActive", cls, IsActive);
			a_vm->RegisterFunction("GetCurrentMonth", cls, GetCurrentMonth);
			a_vm->RegisterFunction("GetRegionBaseTemperature", cls, GetRegionBaseTemperature);
			a_vm->RegisterFunction("GetDayPhase", cls, GetDayPhase);
			a_vm->RegisterFunction("GetPhasePenalty", cls, GetPhasePenalty);
			a_vm->RegisterFunction("IsInThroatOfTheWorld", cls, IsInThroatOfTheWorld);
			a_vm->RegisterFunction("DangerousMountain", cls, DangerousMountain);
			a_vm->RegisterFunction("ColdCities", cls, ColdCities);
			a_vm->RegisterFunction("GetSettingsVersion", cls, GetSettingsVersion);
			a_vm->RegisterFunction("ReportClimate", cls, ReportClimate);
			return true;
		}
	}

	void RegisterNatives()
	{
		SKSE::GetPapyrusInterface()->Register(Register);
	}
}
