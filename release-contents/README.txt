# Frostfall 2026 - Seasons

Seasonal temperatures for Frostfall 2026. Frostfall gives each region one base temperature for the
whole year; this makes the temperature follow the month: a **base temperature for each region** plus a **change for each month**, a smoother
**dawn and dusk** instead of one step at 19:00 and 7:00, and a few fixes to Frostfall's limits on high ground and in cities.

A rebuild of Frostfall - Seasons (Nexus 16317) by BrinaSair for Frostfall 2026: same idea, same
default values. The settings are a page in the SKSE Menu Framework (next to Frostfall's own pages) instead of an MCM, and the numbers live in
an SKSE plugin instead of global variables and a plugin file.

**Pre-release.** Built and compiled against Frostfall 2026's scripts; not yet tested in game.

## What is in it

| File | What |
|---|---|
| `SKSE/Plugins/FrostfallSeasons.dll` | the seasonal temperatures, the regions of the DLC and new lands, the settings page, `FrostfallSeasons.ini` |
| `Scripts/_Frost_ClimateSystem.pex` | Frostfall 2026's climate script with the seasons applied. **The only Frostfall script it overwrites.** |
| `Scripts/FrostfallSeasonsNative.pex` | the bridge between the two |
| `Frostfall 2026 - Seasons.esp` (optional) | an ESL-flagged plugin with no masters and 32 read-only global variables, see *For patches and other mods* |

The seasons themselves need **no plugin**. Nothing of Frostfall's plugin is read or changed, so the same files work with both the regular and the
ESL build of Frostfall 2026, and the old `Frostfall_Seasonal_Temps_patch.esp` is not needed. Every call to the plugin is guarded: without the DLL,
or with *Seasonal temperatures* switched off, Frostfall 2026 gives its own temperatures exactly.

## Install

One FOMOD installer. It always installs the DLL and the scripts, and offers the **Form hooks (ESL plugin)** as an option.

1. Install **Frostfall 2026** (regular or ESL) and the SKSE Menu Framework.
2. **Remove the old Frostfall - Seasons** (its plugin, scripts and MCM). It overwrites the same scripts and cannot be used together.
3. Install this over Frostfall 2026 so that `_Frost_ClimateSystem.pex` is replaced (it is the only file of Frostfall 2026 it touches).
4. Settings: SKSE Menu Framework > Frostfall > **Seasons**. They are saved to `Data/SKSE/Plugins/FrostfallSeasons.ini`.

## For patches and other mods

Most mods only need Frostfall's own temperature, which already includes the seasons: `FrostUtil.GetCurrentTemperature()` and the
`FrostfallCurrentTemperatureReadOnly` global. For the seasons themselves:

- **Papyrus:** `FrostfallSeasonsNative` has `IsInstalled()`, `Active()`, `GetCurrentMonth()`, `GetRegionBaseTemperature(region, x, y, z)`,
  `GetDayPhase(hour)`, `GetPhasePenalty(phase)`. Check `FrostfallSeasonsNative.Active()` first.
- **Mod events** (register with `RegisterForModEvent`): `FrostfallSeasons_MonthChanged` (string = month name, number = month 0-11) and
  `FrostfallSeasons_SettingsChanged`.
- **Global variables** (the optional plugin, `Frostfall 2026 - Seasons.esp`, FormIDs 000800 to 00081F, editor ids `FrostfallSeasons_...`): `Enabled`, `Month`,
  `MonthType`, `MonthTemperature`, `Region` (Frostfall's region code), `RegionBaseTemperature`, `DayPhase`, `DangerousMountain`, `ColdCities`,
  `TEMP_COLD` ... `TEMP_HOT`, `REGION_..._BASE_TEMP` for the ten regions, the dawn and dusk hours and the two penalties. They are read-only mirrors:
  the DLL writes them, and changing one changes nothing. The settings are in `FrostfallSeasons.ini`.

Frostfall works the temperature out again when the weather, the region, the time of day, the month or a setting changes, so a changed value is
felt at its next update (a few seconds).

## What changed against the original

- Settings in the SKSE Menu Framework, with a table of every region in every month.
- Each of the 12 months can be given any of the seven types (the original fixed them).
- The DLC and new-land regions (Forgotten Vale, Solstheim, Wyrmstooth, DarkenD) are told apart by position in the plugin, so the original's rewrite of
  `_Frost_Compatibility` is not needed and that script stays Frostfall 2026's.
- The month changes the temperature as soon as it changes (the original waited for the next weather change).
- The original's summer/winter rule for snowy heights (`month >= 5 || month <= 7`) was true in every month, so everything outside winter counted
  as summer. Winter (Evening Star to Sun's Dawn) now gives -15, summer (Mid Year to Last Seed) -5 and the months between -10.
- The outline of the Throat of the World had 14 points in an array of 13; all 14 are used.

Everything else is as in Frostfall - Seasons 1.5: month types (cold -12, chill -8, brisk -4, none 0, nice +4, warm +8, hot +12), region
temperatures (unknown 9, pine forest 15, volcanic tundra 12, fall forest 10, Reach 8, tundra 5, marsh 1, coast -5, snow -10, ash waste 3), dawn
5-8 and dusk 18-21 taking 2 degrees off and the night taking 5, the Throat of the World counting as arctic snow, an optional *dangerous mountain*
with eight tiers of cold, Windhelm, Whiterun and Solitude no longer stuck at a floor of 2 (it is -15), and the Statue of Meridia platform always mild.

Source: https://github.com/CageTV/Frostfall---Seasons-2026
