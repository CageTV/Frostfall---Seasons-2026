#pragma once

// Data/SKSE/Plugins/FrostfallSeasons.ini. Everything here has a page in the SKSE Menu Framework (Frostfall > Seasons).
//
// The defaults are the ones of Frostfall - Seasons 1.5 by BrinaSair: seven month types (the hottest month is "hot", the coldest "cold",
// the months either side step toward "none" at the equinoxes) added to a base temperature for each region.
struct Settings
{
	// Region kinds, in the order of the settings. Frostfall's own region codes are mapped to these in Seasons.cpp.
	enum Region : int
	{
		kUnknown = 0,
		kPineForest,
		kVolcanicTundra,
		kFallForest,
		kReach,
		kTundra,
		kTundraMarsh,
		kCoast,
		kSnow,
		kAshWaste,
		kRegionCount
	};

	// Month types, coldest first
	enum MonthType : int
	{
		kCold = 0,
		kChill,
		kBrisk,
		kNone,
		kNice,
		kWarm,
		kHot,
		kMonthTypeCount
	};

	static constexpr int kMonths = 12;

	bool enabled{ true };

	std::array<int, kMonthTypeCount> typeTemperature{ -12, -8, -4, 0, 4, 8, 12 };
	std::array<int, kMonths>         monthType{ kCold, kChill, kBrisk, kNone, kNice, kWarm, kHot, kWarm, kNice, kNone, kBrisk, kChill };
	std::array<int, kRegionCount>    regionBase{ 9, 15, 12, 10, 8, 5, 1, -5, -10, 3 };

	// Dawn and dusk: a milder penalty than the night's, instead of a step at 7 and 19
	float sunriseStart{ 5.0f };
	float sunriseEnd{ 8.0f };
	float sunsetStart{ 18.0f };
	float sunsetEnd{ 21.0f };
	int   twilightPenalty{ 2 };
	int   nightPenalty{ 5 };

	bool dangerousThroat{ false };  // eight tiers of cold on the way up the Throat of the World instead of five
	bool coldCities{ true };        // Windhelm, Whiterun and Solitude no longer have a floor of 2 degrees

	bool debugLog{ false };

	static Settings& Get();
	static constexpr const char* kPath = "Data/SKSE/Plugins/FrostfallSeasons.ini";

	void Load();
	void Save() const;
	void Clamp();

	// Counts every change, so Frostfall's climate script knows to work out the temperature again
	static inline std::atomic<int> version{ 1 };
};

extern const char* const kMonthNames[Settings::kMonths];
extern const char* const kMonthTypeNames[Settings::kMonthTypeCount];
extern const char* const kRegionNames[Settings::kRegionCount];
