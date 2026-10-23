scriptname FrostfallSeasonsNative hidden
{Frostfall 2026 - Seasons: bridge to FrostfallSeasons.dll, the SKSE plugin that holds the seasonal temperatures (a base temperature for each
region, a change for each month, dawn and dusk) and their page in the SKSE Menu Framework. Every call goes through Active(), so without the plugin
(or with the seasons switched off) Frostfall's climate script gives Frostfall's own temperatures.}

; True when FrostfallSeasons.dll is installed
bool function IsInstalled() global
	return SKSE.GetPluginVersion("FrostfallSeasons") > 0
endFunction

; True when the plugin is installed and the seasonal temperatures are switched on
bool function Active() global
	return IsInstalled() && IsActive()
endFunction

; Implemented by FrostfallSeasons.dll
bool function IsActive() global native
; The game's month, 0 (Morning Star) to 11 (Evening Star)
int function GetCurrentMonth() global native
; The base temperature of a Frostfall region code (with the Forgotten Vale, Solstheim, Wyrmstooth and DarkenD told apart by position)
; for the current month, or -1000 when the region is not handled (Oblivion)
int function GetRegionBaseTemperature(int aiRegion, float afPosX, float afPosY, float afPosZ) global native
; 0 day, 1 dawn or dusk, 2 night
int function GetDayPhase(float afHour) global native
; Degrees to take off in a day phase
int function GetPhasePenalty(int aiPhase) global native
bool function IsInThroatOfTheWorld(float afPosX, float afPosY) global native
bool function DangerousMountain() global native
bool function ColdCities() global native
; Goes up every time a setting changes, so the climate script works the temperature out again
int function GetSettingsVersion() global native
; Tells the settings page what the climate script worked out
function ReportClimate(int aiRegion, int aiBaseTemperature, int aiCurrentTemperature) global native
