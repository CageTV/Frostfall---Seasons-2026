"""Builds src/scripts/_Frost_ClimateSystem.psc from Frostfall 2026's own script by applying the Frostfall - Seasons changes.

Every replacement must match exactly once, so a changed Frostfall script is noticed instead of being silently half-patched.
Tabs are written as "T" followed by nothing here: the helper t() turns the marker "<TAB>" into a tab.

Usage: python tools/patch_climate.py [path to Frostfall 2026's _Frost_ClimateSystem.psc]
"""
import os
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = sys.argv[1] if len(sys.argv) > 1 else r"E:\WorkSpace\Frostfall-2026-repo\src\scripts\_Frost_ClimateSystem.psc"
OUT = os.path.join(HERE, "src", "scripts", "_Frost_ClimateSystem.psc")

text = open(SRC, encoding="utf-8", newline="").read().replace("\r\n", "\n")


def t(s):
    return s.replace("<TAB>", "\t")


def sub(old, new):
    global text
    old, new = t(old), t(new)
    n = text.count(old)
    if n != 1:
        raise SystemExit(f"expected one match, found {n} for:\n{old}")
    text = text.replace(old, new)


text = ("; Frostfall 2026 - Seasons: Frostfall 2026's _Frost_ClimateSystem.psc (Frostfall by Chesko, MIT) with seasonal temperatures.\n"
        "; Ideas and values from Frostfall - Seasons 1.5 by BrinaSair. The numbers and the settings live in FrostfallSeasons.dll.\n"
        "; Every call goes through FrostfallSeasonsNative.Active(), so this gives Frostfall 2026's own temperatures when the plugin is missing\n"
        "; or the seasons are off.\n"
        + text)

# ---- state ----
sub("bool was_nighttime\nbool is_nighttime\n",
    "bool was_nighttime\nbool is_nighttime\nbool was_twilight\nbool is_twilight\nint last_month = -1\nint last_settings_version = -1\n"
    "Worldspace whiterun_world\nWorldspace solitude_world\n")

# ---- the Throat of the World belongs to the arctic region ----
sub("<TAB><TAB>bool inSnow = IsPointInPolygonFast(snow_polyx, snow_polyy, snow_minx, snow_maxx, snow_miny, snow_maxy, playerX, playerY)\n\n<TAB><TAB>if inPineForest && !inVolcanicTundra\n",
    "<TAB><TAB>bool inSnow = IsPointInPolygonFast(snow_polyx, snow_polyy, snow_minx, snow_maxx, snow_miny, snow_maxy, playerX, playerY)\n\n"
    "<TAB><TAB>; Frostfall - Seasons: the Throat of the World is snow country, not the region around its foot\n"
    "<TAB><TAB>if FrostfallSeasonsNative.Active() && FrostfallSeasonsNative.IsInThroatOfTheWorld(playerX, playerY) && !inPineForest && !inTundra && !inFallForest\n"
    "<TAB><TAB><TAB>return REGION_SNOW\n<TAB><TAB>endif\n\n<TAB><TAB>if inPineForest && !inVolcanicTundra\n")

# ---- tell the settings page what was worked out ----
sub("<TAB>FrostDebug(0, \"%%%% Climate ::: Current Temp: \" + current_temperature + \", Region: \" + region)\n",
    "<TAB>FrostDebug(0, \"%%%% Climate ::: Current Temp: \" + current_temperature + \", Region: \" + region)\n"
    "<TAB>if FrostfallSeasonsNative.Active()\n<TAB><TAB>FrostfallSeasonsNative.ReportClimate(region, GetRegionBaselineTemperature(region), current_temperature)\n<TAB>endif\n")

# ---- day phase, dawn and dusk, month and settings in the cache check ----
sub("<TAB>float hour = GameHour.GetValue()\n<TAB>if hour > 19 || hour < 7\n<TAB><TAB>is_nighttime = true\n<TAB>else\n<TAB><TAB>is_nighttime = false\n<TAB>endif\n\n"
    "<TAB>bool time_changed = false\n<TAB>if (is_nighttime && !was_nighttime) || (!is_nighttime && was_nighttime)\n<TAB><TAB>time_changed = true\n<TAB>endif\n",
    "<TAB>float hour = GameHour.GetValue()\n"
    "<TAB>bool seasons = FrostfallSeasonsNative.Active()\n"
    "<TAB>if seasons\n"
    "<TAB><TAB>; Frostfall - Seasons: dawn and dusk take less off than the night does\n"
    "<TAB><TAB>int day_phase = FrostfallSeasonsNative.GetDayPhase(hour)\n"
    "<TAB><TAB>is_nighttime = day_phase == 2\n"
    "<TAB><TAB>is_twilight = day_phase == 1\n"
    "<TAB>else\n"
    "<TAB><TAB>is_twilight = false\n"
    "<TAB><TAB>if hour > 19 || hour < 7\n<TAB><TAB><TAB>is_nighttime = true\n<TAB><TAB>else\n<TAB><TAB><TAB>is_nighttime = false\n<TAB><TAB>endif\n"
    "<TAB>endif\n\n"
    "<TAB>bool time_changed = false\n"
    "<TAB>if (is_nighttime && !was_nighttime) || (!is_nighttime && was_nighttime) || (is_twilight && !was_twilight) || (!is_twilight && was_twilight)\n"
    "<TAB><TAB>time_changed = true\n<TAB>endif\n\n"
    "<TAB>; A new month or a changed setting changes the base temperature, whatever the weather does\n"
    "<TAB>if seasons\n"
    "<TAB><TAB>int this_month = FrostfallSeasonsNative.GetCurrentMonth()\n"
    "<TAB><TAB>int this_version = FrostfallSeasonsNative.GetSettingsVersion()\n"
    "<TAB><TAB>if this_month != last_month || this_version != last_settings_version\n"
    "<TAB><TAB><TAB>time_changed = true\n"
    "<TAB><TAB>endif\n"
    "<TAB><TAB>last_month = this_month\n"
    "<TAB><TAB>last_settings_version = this_version\n"
    "<TAB>endif\n")

sub("<TAB>if is_nighttime\n<TAB><TAB>current_temperature -= 5\n<TAB>endif\n<TAB>was_nighttime = is_nighttime\n",
    "<TAB>if is_twilight\n<TAB><TAB>current_temperature -= FrostfallSeasonsNative.GetPhasePenalty(1)\n<TAB>endif\n<TAB>was_twilight = is_twilight\n\n"
    "<TAB>if is_nighttime\n"
    "<TAB><TAB>if seasons\n<TAB><TAB><TAB>current_temperature -= FrostfallSeasonsNative.GetPhasePenalty(2)\n<TAB><TAB>else\n<TAB><TAB><TAB>current_temperature -= 5\n<TAB><TAB>endif\n"
    "<TAB>endif\n<TAB>was_nighttime = is_nighttime\n")

# ---- the lowest temperature of a place ----
a = text.index("int function GetMinTemperatureByLocation()")
b = text.index("int function GetMaxTemperatureByLocation()")
new_min = t("""int function GetMinTemperatureByLocation()
<TAB>int min_temp = -100
<TAB>bool seasons = FrostfallSeasonsNative.Active()
<TAB>bool cold_cities = seasons && FrostfallSeasonsNative.ColdCities()
<TAB>if cold_cities
<TAB><TAB>LoadCityWorldspaces()
<TAB>endif

<TAB>if seasons && ws == Tamriel && (pos_x <= -93300 && pos_x >= -99200) && (pos_y <= 107400 && pos_y >= 97000) && pos_z >= 13500
<TAB><TAB>; Frostfall - Seasons: the platform the statue of Meridia raises the player on is always mild
<TAB><TAB>was_on_high_hrothgar_peak = false
<TAB><TAB>return 10
<TAB>elseif ws == WindhelmWorld || ws == WindhelmPitWorldspace || (cold_cities && (ws == whiterun_world || ws == solitude_world))
<TAB><TAB>was_on_high_hrothgar_peak = false
<TAB><TAB>if cold_cities
<TAB><TAB><TAB>; Frostfall - Seasons: Windhelm never went under 2 degrees while Whiterun and Solitude had no floor at all. The three share one that still lets a winter night be felt.
<TAB><TAB><TAB>return -15
<TAB><TAB>endif
<TAB><TAB>return 2
<TAB>elseif ws == Tamriel && (pos_x <= 74340 && pos_x >= 32000) && (pos_y <= -21000 && pos_y >= -66600) && pos_z >= 35541
<TAB><TAB>if !was_on_high_hrothgar_peak
<TAB><TAB><TAB>was_on_high_hrothgar_peak = true
<TAB><TAB><TAB>ShowCalmMessage()
<TAB><TAB>endif
<TAB><TAB>return 10
<TAB>else
<TAB><TAB>was_on_high_hrothgar_peak = false
<TAB><TAB>return min_temp
<TAB>endif
endFunction

function LoadCityWorldspaces()
<TAB>if !whiterun_world
<TAB><TAB>whiterun_world = Game.GetFormFromFile(0x0001A26F, "Skyrim.esm") as Worldspace
<TAB>endif
<TAB>if !solitude_world
<TAB><TAB>solitude_world = Game.GetFormFromFile(0x00037EDF, "Skyrim.esm") as Worldspace
<TAB>endif
endFunction

; Frostfall - Seasons: a value for winter (Evening Star to Sun's Dawn), one for summer (Mid Year to Last Seed) and one for the months between
int function SeasonalValue(int aiMonth, int aiWinter, int aiBetween, int aiSummer)
<TAB>if aiMonth <= 1 || aiMonth == 11
<TAB><TAB>return aiWinter
<TAB>elseif aiMonth >= 5 && aiMonth <= 7
<TAB><TAB>return aiSummer
<TAB>endif
<TAB>return aiBetween
endFunction

; The snow-covered heights: -10 all year in Frostfall. Frostfall - Seasons: -15 in winter and -5 in summer, in case a summer mod melts the snow.
int function HighGroundMax(bool abSeasons, int aiMonth)
<TAB>if abSeasons
<TAB><TAB>return SeasonalValue(aiMonth, -15, -10, -5)
<TAB>endif
<TAB>return -10
endFunction

""")
text = text[:a] + new_min + text[b:]

# ---- the highest temperature of a place: the original code stays, the seasons add to it ----
sub("<TAB>if ws != Tamriel\n<TAB><TAB>return max_temp\n<TAB>endif\n\n<TAB>; High Hrothgar\n<TAB>if (pos_x <= 74340 && pos_x >= 32000) && (pos_y <= -21000 && pos_y >= -66600)\n",
    "<TAB>if ws != Tamriel\n<TAB><TAB>return max_temp\n<TAB>endif\n\n"
    "<TAB>bool seasons = FrostfallSeasonsNative.Active()\n<TAB>int month = 0\n<TAB>if seasons\n<TAB><TAB>month = FrostfallSeasonsNative.GetCurrentMonth()\n<TAB>endif\n\n"
    "<TAB>; High Hrothgar, dangerous mountain (Frostfall - Seasons): eight tiers, each 5 degrees worse than the one below\n"
    "<TAB>if seasons && FrostfallSeasonsNative.DangerousMountain() && (pos_x <= 74340 && pos_x >= 32000) && (pos_y <= -21000 && pos_y >= -66600)\n"
    "<TAB><TAB>if pos_z >= 35541<TAB><TAB>;Elevation 5: the peak, a strange calm\n<TAB><TAB><TAB>max_temp = 10\n"
    "<TAB><TAB>elseif pos_z >= 34000<TAB>;Elevation 4: shortly before the peak\n<TAB><TAB><TAB>max_temp = -35\n"
    "<TAB><TAB>elseif pos_z >= 31000<TAB>;Elevation 3.5: where a rock formation splits the path in two\n<TAB><TAB><TAB>max_temp = -30\n"
    "<TAB><TAB>elseif pos_z >= 27500<TAB>;Elevation 3: a third of the way up from High Hrothgar, where the goat stands\n<TAB><TAB><TAB>max_temp = -25\n"
    "<TAB><TAB>elseif pos_z >= 24500<TAB>;Elevation 2.5: the top of the stairs after High Hrothgar\n<TAB><TAB><TAB>max_temp = -20\n"
    "<TAB><TAB>elseif pos_z >= 21300<TAB>;Elevation 2: the wide steps before High Hrothgar\n<TAB><TAB><TAB>max_temp = -15\n"
    "<TAB><TAB>elseif pos_z >= 18500<TAB>;Elevation 1.5: the first curve after the snow becomes continuous\n<TAB><TAB><TAB>max_temp = -10\n"
    "<TAB><TAB>elseif pos_z >= 15750<TAB>;Elevation 1: snow starts on the path\n<TAB><TAB><TAB>max_temp = -5\n"
    "<TAB><TAB>endif\n"
    "<TAB><TAB>FrostDebug(0, \"%%%% Climate ::: Override Area: High Hrothgar (dangerous) | Max Temp: \" + max_temp)\n"
    "<TAB>; High Hrothgar\n"
    "<TAB>elseif (pos_x <= 74340 && pos_x >= 32000) && (pos_y <= -21000 && pos_y >= -66600)\n")

for z, name in ((16600, "Rift"), (7300, "Falkreath Hold"), (1500, "Bleak Falls Barrow"), (2000, "Brittleshin Pass")):
    sub(f"<TAB><TAB>if pos_z >= {z}\n<TAB><TAB><TAB>max_temp = -10\n<TAB><TAB>endif\n<TAB><TAB>FrostDebug(0, \"%%%% Climate ::: Override Area: {name} | Max Temp: \" + max_temp)\n",
        f"<TAB><TAB>if pos_z >= {z}\n<TAB><TAB><TAB>max_temp = HighGroundMax(seasons, month)\n<TAB><TAB>endif\n<TAB><TAB>FrostDebug(0, \"%%%% Climate ::: Override Area: {name} | Max Temp: \" + max_temp)\n")

sub("<TAB><TAB>if pos_z >= -7000\n<TAB><TAB><TAB>max_temp = -10\n<TAB><TAB>else\n<TAB><TAB><TAB>max_temp = -5\n<TAB><TAB>endif\n<TAB><TAB>FrostDebug(0, \"%%%% Climate ::: Override Area: Haafingar | Max Temp: \" + max_temp)\n<TAB>endif\n",
    "<TAB><TAB>if pos_z >= -7000\n<TAB><TAB><TAB>max_temp = HighGroundMax(seasons, month)\n"
    "<TAB><TAB>elseif seasons\n<TAB><TAB><TAB>max_temp = SeasonalValue(month, -10, -5, 0)\n"
    "<TAB><TAB>else\n<TAB><TAB><TAB>max_temp = -5\n<TAB><TAB>endif\n"
    "<TAB><TAB>FrostDebug(0, \"%%%% Climate ::: Override Area: Haafingar | Max Temp: \" + max_temp)\n"
    "<TAB>; Frostfall - Seasons: the statue of Meridia, when it raises the player into the sky\n"
    "<TAB>elseif seasons && (pos_x <= -93300 && pos_x >= -99200) && (pos_y <= 107400 && pos_y >= 97000) && pos_z >= 13500\n"
    "<TAB><TAB>max_temp = 10\n"
    "<TAB><TAB>FrostDebug(0, \"%%%% Climate ::: Override Area: Statue of Meridia platform | Max Temp: \" + max_temp)\n"
    "<TAB>endif\n")

# ---- the starting point of the temperature ----
sub("int function GetRegionBaselineTemperature(int region)\n<TAB>if region == REGION_UNKNOWN\n",
    "int function GetRegionBaselineTemperature(int region)\n"
    "<TAB>; Frostfall - Seasons: a base temperature for each region and each month, from the settings page\n"
    "<TAB>if FrostfallSeasonsNative.Active()\n"
    "<TAB><TAB>int seasonal = FrostfallSeasonsNative.GetRegionBaseTemperature(region, pos_x, pos_y, pos_z)\n"
    "<TAB><TAB>if seasonal > -1000\n<TAB><TAB><TAB>return seasonal\n<TAB><TAB>endif\n"
    "<TAB>endif\n\n"
    "<TAB>if region == REGION_UNKNOWN\n")

open(OUT, "w", encoding="utf-8", newline="").write(text.replace("\n", "\r\n"))
print("wrote", OUT, len(text.splitlines()), "lines")
