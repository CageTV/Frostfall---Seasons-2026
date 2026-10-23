#pragma once

// The optional "Frostfall 2026 - Seasons.esp": read-only global variables that mirror the seasons, for patches and scripts that read globals.
// Without the plugin nothing here does anything.
namespace Globals
{
	// Looks the globals up (kDataLoaded)
	void Init();

	// Writes the current values into the globals; main thread
	void Refresh();

	// Sends the mod events a script can register for (FrostfallSeasons_MonthChanged, FrostfallSeasons_SettingsChanged) when the month or a setting changed
	// since the last call; main thread
	void Notify();
}
