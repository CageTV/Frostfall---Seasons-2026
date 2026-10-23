"""Builds the optional "Frostfall 2026 - Seasons.esp" (an ESL-flagged plugin of read-only global variables, no masters) and the list the DLL
uses to find them.

The plugin only mirrors what FrostfallSeasons.dll knows, so that Creation Kit conditions, SPID/KID filters, patches and scripts that read
globals can see the seasons. Changing a value in the plugin changes nothing; the settings live in FrostfallSeasons.ini.

Writes:  plugin-src/Frostfall 2026 - Seasons.esp/   (Spriggit YAML, the source of the plugin)
         plugin/src/GlobalList.inc                    (X-macro list, the order is the FormID order: 000800, 000801, ...)
         release-contents/Optional - Form hooks/Frostfall 2026 - Seasons.esp   (through Spriggit)
Usage: python tools/make_plugin.py
"""
import json
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
NAME = "Frostfall 2026 - Seasons.esp"
SPRIGGIT = r"E:\Tabula Rasa\mods\Skyrim-Claude Code Modder's Toolkit\tools\spriggit-cli.sh"

# (editor id, "Short" or "Float", default, what it is)
GLOBALS = [
    ("FrostfallSeasons_Enabled", "Short", 1, "1 when the seasonal temperatures are on"),
    ("FrostfallSeasons_Month", "Short", 0, "the game's month, 0 Morning Star .. 11 Evening Star"),
    ("FrostfallSeasons_MonthType", "Short", 0, "type of the month: 0 cold, 1 chill, 2 brisk, 3 none, 4 nice, 5 warm, 6 hot"),
    ("FrostfallSeasons_MonthTemperature", "Short", 0, "degrees the month's type adds"),
    ("FrostfallSeasons_Region", "Short", -1, "Frostfall's region code of the last climate update (-1 unknown)"),
    ("FrostfallSeasons_RegionBaseTemperature", "Short", 0, "base temperature of that region this month"),
    ("FrostfallSeasons_DayPhase", "Short", 0, "0 day, 1 dawn or dusk, 2 night"),
    ("FrostfallSeasons_DangerousMountain", "Short", 0, "1 when the dangerous Throat of the World is on"),
    ("FrostfallSeasons_ColdCities", "Short", 1, "1 when Windhelm, Whiterun and Solitude can get cold"),
    # month types
    ("FrostfallSeasons_TEMP_COLD", "Short", -12, "what a cold month adds"),
    ("FrostfallSeasons_TEMP_CHILL", "Short", -8, "what a chill month adds"),
    ("FrostfallSeasons_TEMP_BRISK", "Short", -4, "what a brisk month adds"),
    ("FrostfallSeasons_TEMP_NONE", "Short", 0, "what a none month adds"),
    ("FrostfallSeasons_TEMP_NICE", "Short", 4, "what a nice month adds"),
    ("FrostfallSeasons_TEMP_WARM", "Short", 8, "what a warm month adds"),
    ("FrostfallSeasons_TEMP_HOT", "Short", 12, "what a hot month adds"),
    # region base temperatures (names as in Frostfall - Seasons 1.5)
    ("FrostfallSeasons_REGION_UNKNOWN_BASE_TEMP", "Short", 9, "unknown / other"),
    ("FrostfallSeasons_REGION_PINEFOREST_BASE_TEMP", "Short", 15, "pine forest"),
    ("FrostfallSeasons_REGION_VOLCANICTUNDRA_BASE_TEMP", "Short", 12, "volcanic tundra"),
    ("FrostfallSeasons_REGION_FALLFOREST_BASE_TEMP", "Short", 10, "fall forest"),
    ("FrostfallSeasons_REGION_REACH_BASE_TEMP", "Short", 8, "the Reach and canyons"),
    ("FrostfallSeasons_REGION_TUNDRA_BASE_TEMP", "Short", 5, "tundra / steppe"),
    ("FrostfallSeasons_REGION_TUNDRAMARSH_BASE_TEMP", "Short", 1, "marsh tundra"),
    ("FrostfallSeasons_REGION_COAST_BASE_TEMP", "Short", -5, "north coast"),
    ("FrostfallSeasons_REGION_SNOW_BASE_TEMP", "Short", -10, "arctic snow"),
    ("FrostfallSeasons_REGION_ASHWASTE_BASE_TEMP", "Short", 3, "ash waste (Solstheim)"),
    # daylight
    ("FrostfallSeasons_SunriseStartHour", "Float", 5.0, "dawn starts"),
    ("FrostfallSeasons_SunriseEndHour", "Float", 8.0, "dawn ends"),
    ("FrostfallSeasons_SunsetStartHour", "Float", 18.0, "dusk starts"),
    ("FrostfallSeasons_SunsetEndHour", "Float", 21.0, "dusk ends"),
    ("FrostfallSeasons_TwilightPenalty", "Short", 2, "degrees taken off at dawn and dusk"),
    ("FrostfallSeasons_NightPenalty", "Short", 5, "degrees taken off at night"),
]

src = os.path.join(HERE, "plugin-src", NAME)
shutil.rmtree(src, ignore_errors=True)
os.makedirs(os.path.join(src, "Globals"))

with open(os.path.join(src, "RecordData.yaml"), "w", encoding="utf-8", newline="\n") as f:
    f.write("SpriggitSource:\n  PackageName: Spriggit.Yaml.Skyrim\n  Version: 0.41\n")
    f.write(f"ModKey: {NAME}\nGameRelease: SkyrimSE\nModHeader:\n  Stats:\n    Version: 1.7\n  Flags:\n  - Small\n")
    f.write("  Author: Frostfall 2026 - Seasons\n")
    f.write("  Description: Optional add-on for Frostfall 2026 - Seasons. Read-only global variables that mirror the seasons for patches and scripts; the settings live in FrostfallSeasons.ini.\n")
    f.write("  INTV: 1\n")

with open(os.path.join(src, "spriggit-meta.json"), "w", encoding="utf-8", newline="\n") as f:
    json.dump({"PackageName": "Spriggit.Yaml.Skyrim", "Version": "0.41.0", "Release": "SkyrimSE", "ModKey": NAME}, f, indent=2)

for i, (edid, kind, default, _) in enumerate(GLOBALS):
    local = 0x800 + i
    assert local <= 0xFFF
    name = f"{edid} - {local:06X}_{NAME}.yaml"
    with open(os.path.join(src, "Globals", name), "w", encoding="utf-8", newline="\n") as f:
        f.write(f"MutagenObjectType: Global{kind}\nFormKey: {local:06X}:{NAME}\nEditorID: {edid}\nData: {default}\n")

with open(os.path.join(HERE, "plugin", "src", "GlobalList.inc"), "w", encoding="utf-8", newline="\n") as f:
    f.write("// generated by tools/make_plugin.py; the order is the FormID order, 000800 upward\n")
    for edid, kind, default, doc in GLOBALS:
        key = edid.replace("FrostfallSeasons_", "")
        f.write(f"X({key})  // {doc}\n")

out_dir = os.path.join(HERE, "release-contents", "Optional - Form hooks")
os.makedirs(out_dir, exist_ok=True)
out = os.path.join(out_dir, NAME)
if os.path.exists(out):
    os.remove(out)
r = subprocess.run(["bash", SPRIGGIT, "deserialize", "--InputPath", src, "--OutputPath", out], capture_output=True, text=True)
print((r.stdout or "")[-600:], (r.stderr or "")[-300:])
print("plugin:", out, os.path.exists(out), len(GLOBALS), "globals")
sys.exit(0 if os.path.exists(out) else 1)
