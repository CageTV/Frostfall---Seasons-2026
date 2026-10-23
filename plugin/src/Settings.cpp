#include "Settings.h"

const char* const kMonthNames[Settings::kMonths] = { "Morning Star", "Sun's Dawn", "First Seed", "Rain's Hand", "Second Seed", "Mid Year",
	"Sun's Height", "Last Seed", "Hearthfire", "Frostfall", "Sun's Dusk", "Evening Star" };
const char* const kMonthTypeNames[Settings::kMonthTypeCount] = { "Cold", "Chill", "Brisk", "None", "Nice", "Warm", "Hot" };
const char* const kRegionNames[Settings::kRegionCount] = { "Unknown / other", "Pine forest", "Volcanic tundra", "Fall forest", "The Reach / canyons",
	"Tundra / steppe", "Marsh tundra", "North coast", "Arctic snow", "Ash waste (Solstheim)" };

namespace
{
	constexpr const char* typeKeys[Settings::kMonthTypeCount] = { "iCold", "iChill", "iBrisk", "iNone", "iNice", "iWarm", "iHot" };
	constexpr const char* regionKeys[Settings::kRegionCount] = { "iUnknown", "iPineForest", "iVolcanicTundra", "iFallForest", "iReach", "iTundra",
		"iTundraMarsh", "iCoast", "iSnow", "iAshWaste" };
	constexpr const char* monthKeys[Settings::kMonths] = { "iMorningStar", "iSunsDawn", "iFirstSeed", "iRainsHand", "iSecondSeed", "iMidYear",
		"iSunsHeight", "iLastSeed", "iHearthfire", "iFrostfall", "iSunsDusk", "iEveningStar" };
}

Settings& Settings::Get()
{
	static Settings instance;
	return instance;
}

void Settings::Clamp()
{
	for (auto& t : typeTemperature) {
		t = std::clamp(t, -60, 60);
	}
	for (auto& t : regionBase) {
		t = std::clamp(t, -60, 60);
	}
	for (auto& m : monthType) {
		m = std::clamp(m, 0, kMonthTypeCount - 1);
	}
	sunriseStart = std::clamp(sunriseStart, 0.0f, 24.0f);
	sunriseEnd = std::clamp(sunriseEnd, sunriseStart, 24.0f);
	sunsetEnd = std::clamp(sunsetEnd, 0.0f, 24.0f);
	sunsetStart = std::clamp(sunsetStart, 0.0f, sunsetEnd);
	twilightPenalty = std::clamp(twilightPenalty, 0, 30);
	nightPenalty = std::clamp(nightPenalty, 0, 30);
}

void Settings::Load()
{
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(kPath);

	enabled = ini.GetBoolValue("General", "bEnabled", enabled);
	dangerousThroat = ini.GetBoolValue("General", "bDangerousMountain", dangerousThroat);
	coldCities = ini.GetBoolValue("General", "bColdCities", coldCities);

	for (int i = 0; i < kMonthTypeCount; i++) {
		typeTemperature[i] = static_cast<int>(ini.GetLongValue("MonthTypes", typeKeys[i], typeTemperature[i]));
	}
	for (int i = 0; i < kMonths; i++) {
		monthType[i] = static_cast<int>(ini.GetLongValue("Months", monthKeys[i], monthType[i]));
	}
	for (int i = 0; i < kRegionCount; i++) {
		regionBase[i] = static_cast<int>(ini.GetLongValue("Regions", regionKeys[i], regionBase[i]));
	}

	sunriseStart = static_cast<float>(ini.GetDoubleValue("Daylight", "fSunriseStart", sunriseStart));
	sunriseEnd = static_cast<float>(ini.GetDoubleValue("Daylight", "fSunriseEnd", sunriseEnd));
	sunsetStart = static_cast<float>(ini.GetDoubleValue("Daylight", "fSunsetStart", sunsetStart));
	sunsetEnd = static_cast<float>(ini.GetDoubleValue("Daylight", "fSunsetEnd", sunsetEnd));
	twilightPenalty = static_cast<int>(ini.GetLongValue("Daylight", "iTwilightPenalty", twilightPenalty));
	nightPenalty = static_cast<int>(ini.GetLongValue("Daylight", "iNightPenalty", nightPenalty));

	debugLog = ini.GetBoolValue("Log", "Debug", debugLog);
	Clamp();
	++version;
}

void Settings::Save() const
{
	CSimpleIniA ini;
	ini.SetUnicode();
	ini.LoadFile(kPath);

	ini.SetBoolValue("General", "bEnabled", enabled, "; off = Frostfall's own temperatures (one base temperature per region, night from 19:00 to 7:00)");
	ini.SetBoolValue("General", "bDangerousMountain", dangerousThroat, "; more tiers of cold on the way up the Throat of the World");
	ini.SetBoolValue("General", "bColdCities", coldCities, "; Windhelm, Whiterun and Solitude can get as cold as the land around them");

	for (int i = 0; i < kMonthTypeCount; i++) {
		ini.SetLongValue("MonthTypes", typeKeys[i], typeTemperature[i], i == 0 ? "; degrees added to a region's base temperature in a month of that type" : nullptr);
	}
	for (int i = 0; i < kMonths; i++) {
		ini.SetLongValue("Months", monthKeys[i], monthType[i], i == 0 ? "; type of each month: 0 cold, 1 chill, 2 brisk, 3 none, 4 nice, 5 warm, 6 hot" : nullptr);
	}
	for (int i = 0; i < kRegionCount; i++) {
		ini.SetLongValue("Regions", regionKeys[i], regionBase[i], i == 0 ? "; base temperature of each region, before the month's change" : nullptr);
	}

	ini.SetDoubleValue("Daylight", "fSunriseStart", sunriseStart, "; hours (24 h clock); dawn and dusk get the smaller penalty");
	ini.SetDoubleValue("Daylight", "fSunriseEnd", sunriseEnd);
	ini.SetDoubleValue("Daylight", "fSunsetStart", sunsetStart);
	ini.SetDoubleValue("Daylight", "fSunsetEnd", sunsetEnd);
	ini.SetLongValue("Daylight", "iTwilightPenalty", twilightPenalty);
	ini.SetLongValue("Daylight", "iNightPenalty", nightPenalty);

	ini.SetBoolValue("Log", "Debug", debugLog);

	std::error_code ec;
	std::filesystem::create_directories(std::filesystem::path(kPath).parent_path(), ec);
	if (ini.SaveFile(kPath) < 0) {
		logger::error("Could not write {}", kPath);
	}
}
