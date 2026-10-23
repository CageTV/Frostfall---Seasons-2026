"""Builds the ONE FOMOD installer zip of Frostfall 2026 - Seasons from release-contents/.

  fomod/info.xml, fomod/ModuleConfig.xml
  Frostfall 2026 - Seasons/        always installed: Scripts (the climate script and the bridge), SKSE/Plugins/FrostfallSeasons.dll, README, LICENSE
  Optional - Form hooks/           optional: Frostfall 2026 - Seasons.esp, an ESL-flagged plugin of read-only global variables

The scripts and the DLL are the same for the regular and the ESL build of Frostfall 2026, so there is no build choice.
Usage: python tools/make_release.py <version>
"""
import os
import sys
import zipfile

HERE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REL = os.path.join(HERE, "release-contents")
OUT = os.path.join(HERE, "release")
version = sys.argv[1] if len(sys.argv) > 1 else "1.0.0"
os.makedirs(OUT, exist_ok=True)

MAIN = "Frostfall 2026 - Seasons"
OPT = "Optional - Form hooks"

INFO = f"""<?xml version="1.0" encoding="UTF-8"?>
<fomod>
  <Name>Frostfall 2026 - Seasons</Name>
  <Author>CageTV</Author>
  <Version>{version}</Version>
  <Website>https://github.com/CageTV/Frostfall---Seasons-2026</Website>
  <Description>Seasonal temperatures for Frostfall 2026. A rebuild of Frostfall - Seasons by BrinaSair.</Description>
</fomod>
"""

CONFIG = f"""<?xml version="1.0" encoding="UTF-8"?>
<config xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://qconsulting.ca/fo3/ModConfig5.0.xsd">
  <moduleName>Frostfall 2026 - Seasons</moduleName>
  <requiredInstallFiles>
    <folder source="{MAIN}" destination=""/>
  </requiredInstallFiles>
  <installSteps order="Explicit">
    <installStep name="Seasons">
      <optionalFileGroups order="Explicit">
        <group name="Installed with every choice" type="SelectAll">
          <plugins order="Explicit">
            <plugin name="Seasonal temperatures">
              <description>The seasonal temperatures: FrostfallSeasons.dll (the numbers and the Frostfall &gt; Seasons page in the SKSE Menu Framework) and Frostfall 2026's climate script with the seasons applied (_Frost_ClimateSystem.pex, the only Frostfall script this overwrites). The same files serve the regular and the ESL build of Frostfall 2026. Remove the old Frostfall - Seasons first. Needs Frostfall 2026, SKSE, the Address Library and the SKSE Menu Framework 3.</description>
              <files/>
              <typeDescriptor><type name="Required"/></typeDescriptor>
            </plugin>
          </plugins>
        </group>
        <group name="Optional" type="SelectAny">
          <plugins order="Explicit">
            <plugin name="Form hooks (ESL plugin)">
              <description>Frostfall 2026 - Seasons.esp: an ESL-flagged plugin with no masters and 32 read-only global variables (the month, its type, the current region and its base temperature, the day phase, every region's base temperature, the month types, the dawn and dusk hours...) that FrostfallSeasons.dll keeps up to date. For patches, Creation Kit conditions and scripts that read globals. Changing a value in the plugin changes nothing: the settings live in FrostfallSeasons.ini. Not needed for the seasons to work.</description>
              <files>
                <folder source="{OPT}" destination=""/>
              </files>
              <typeDescriptor><type name="Optional"/></typeDescriptor>
            </plugin>
          </plugins>
        </group>
      </optionalFileGroups>
    </installStep>
  </installSteps>
</config>
"""

path = os.path.join(OUT, f"Frostfall 2026 - Seasons {version}.zip")
if os.path.exists(path):
    os.remove(path)
with zipfile.ZipFile(path, "w", zipfile.ZIP_DEFLATED) as z:
    z.writestr("fomod/info.xml", INFO)
    z.writestr("fomod/ModuleConfig.xml", CONFIG)
    for root, _, files in os.walk(REL):
        for fn in files:
            src = os.path.join(root, fn)
            rel = os.path.relpath(src, REL).replace("\\", "/")
            if rel.startswith(OPT + "/"):
                z.write(src, rel)
            else:
                z.write(src, f"{MAIN}/{rel}")
print(path, os.path.getsize(path) // 1024, "KB")
with zipfile.ZipFile(path) as z:
    for n in z.namelist():
        print("  ", n)
