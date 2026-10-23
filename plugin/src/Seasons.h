#pragma once

namespace Seasons
{
	// Frostfall's own region codes (_Frost_ClimateSystem.psc)
	namespace Code
	{
		constexpr int kUnknown = -1;
		constexpr int kPineForest = 1;
		constexpr int kVolcanicTundra = 2;
		constexpr int kFallForest = 3;
		constexpr int kReach = 4;
		constexpr int kTundra = 5;
		constexpr int kTundraMarsh = 6;
		constexpr int kCoast = 7;
		constexpr int kSnow = 8;
		constexpr int kOblivion = 9;
		constexpr int kFalmerValley = 10;
		constexpr int kSolstheim = 11;
		constexpr int kWyrmstooth = 20;
		constexpr int kDarkend = 21;
	}

	constexpr int kNotMine = -1000;

	enum Phase : int { kDay = 0, kTwilight = 1, kNight = 2 };

	// the game's month, 0 (Morning Star) to 11 (Evening Star)
	int CurrentMonth();

	// which of the settings' regions Frostfall's region code and the player's position stand for; -1 when the region is not handled here (Oblivion)
	int RegionKind(int a_frostfallRegion, float a_x, float a_y, float a_z);

	// the temperature the climate script starts from; kNotMine when the seasons are off or the region is not handled here
	int RegionBaseTemperature(int a_frostfallRegion, float a_x, float a_y, float a_z, int a_month);

	Phase DayPhase(float a_hour);
	int   PhasePenalty(int a_phase);
	bool  InThroatOfTheWorld(float a_x, float a_y);

	// what the climate script last worked out, for the settings page
	struct Report
	{
		int region{ Code::kUnknown };
		int baseTemperature{ 0 };
		int currentTemperature{ 0 };
		bool seen{ false };
	};
	Report GetReport();

	void RegisterNatives();
}
