#include "Menu.h"

#include "Seasons.h"
#include "Settings.h"

#pragma warning(push)
#pragma warning(disable : 4099 5054)
#include "SKSEMenuFramework.h"
#pragma warning(pop)

namespace Menu
{
	namespace
	{
		using namespace ImGuiMCP;

		bool dirty = false;

		void Tip(const char* a_text)
		{
			if (IsItemHovered()) {
				SetTooltip("%s", a_text);
			}
		}

		const char* FrostfallRegionName(int a_code)
		{
			switch (a_code) {
			case Seasons::Code::kPineForest: return "Pine forest";
			case Seasons::Code::kVolcanicTundra: return "Volcanic tundra";
			case Seasons::Code::kFallForest: return "Fall forest";
			case Seasons::Code::kReach: return "The Reach / canyons";
			case Seasons::Code::kTundra: return "Tundra / steppe";
			case Seasons::Code::kTundraMarsh: return "Marsh tundra";
			case Seasons::Code::kCoast: return "North coast";
			case Seasons::Code::kSnow: return "Arctic snow";
			case Seasons::Code::kOblivion: return "Oblivion";
			case Seasons::Code::kFalmerValley: return "Forgotten Vale";
			case Seasons::Code::kSolstheim: return "Solstheim";
			case Seasons::Code::kWyrmstooth: return "Wyrmstooth";
			case Seasons::Code::kDarkend: return "DarkenD";
			default: return "Unknown / other";
			}
		}

		void __stdcall OnMenuEvent(SKSEMenuFramework::Model::EventType a_type)
		{
			if (a_type == SKSEMenuFramework::Model::kCloseMenu && dirty) {
				Settings::Get().Clamp();
				Settings::Get().Save();
				dirty = false;
			}
		}

		bool Changed(bool a_changed)
		{
			if (a_changed) {
				dirty = true;
				++Settings::version;  // Frostfall works the temperature out again at its next update
			}
			return a_changed;
		}

		void RenderStatus()
		{
			const auto& s = Settings::Get();
			const int   month = Seasons::CurrentMonth();
			const int   type = s.monthType[month];
			const auto  report = Seasons::GetReport();

			SeparatorText("Now");
			Text("%s: a %s month (%+d degrees)", kMonthNames[month], kMonthTypeNames[type], s.typeTemperature[type]);
			if (report.seen) {
				Text("You are in: %s. Base temperature %d, Frostfall's temperature %d.", FrostfallRegionName(report.region), report.baseTemperature,
					report.currentTemperature);
			} else {
				TextDisabled("Frostfall has not reported a temperature yet (it updates when the weather, the region or the time of day changes).");
			}
		}

		void RenderRegions()
		{
			auto& s = Settings::Get();
			TextWrapped("The temperature of each region when the month is \"none\". The month types (Months tab) are added to it, then Frostfall takes off the weather's share.");
			for (int i = 0; i < Settings::kRegionCount; i++) {
				PushID(i);
				Changed(SliderInt(kRegionNames[i], &s.regionBase[i], -40, 40, "%d"));
				PopID();
			}
			Tip("Frostfall's own values were 9 (unknown), 20 (pine forest), 18 (volcanic tundra), 12 (fall forest), 10 (Reach, tundra), 1 (marsh), -5 (coast), -10 (snow).");
		}

		void RenderMonths()
		{
			auto& s = Settings::Get();
			SeparatorText("What each type adds");
			for (int i = 0; i < Settings::kMonthTypeCount; i++) {
				PushID(i);
				Changed(SliderInt(kMonthTypeNames[i], &s.typeTemperature[i], -40, 40, "%+d"));
				PopID();
			}
			SeparatorText("Type of each month");
			TextWrapped("By default the hottest month is Sun's Height, the coldest Morning Star, and the types step down toward \"none\" at the equinoxes.");
			for (int i = 0; i < Settings::kMonths; i++) {
				PushID(100 + i);
				if (BeginCombo(kMonthNames[i], kMonthTypeNames[s.monthType[i]])) {
					for (int t = 0; t < Settings::kMonthTypeCount; t++) {
						if (Selectable(kMonthTypeNames[t], t == s.monthType[i])) {
							s.monthType[i] = t;
							Changed(true);
						}
					}
					EndCombo();
				}
				PopID();
			}
		}

		void RenderDaylight()
		{
			auto& s = Settings::Get();
			TextWrapped("Frostfall takes 5 degrees off from 19:00 to 7:00. Here dawn and dusk take a smaller share, then the night takes its own. The game's own sunrise and sunset times cannot be read, so set the hours that fit the light you see.");
			SeparatorText("Dawn");
			Changed(SliderFloat("Dawn starts", &s.sunriseStart, 0.0f, 12.0f, "%.1f h"));
			Changed(SliderFloat("Dawn ends", &s.sunriseEnd, s.sunriseStart, 12.0f, "%.1f h"));
			SeparatorText("Dusk");
			Changed(SliderFloat("Dusk starts", &s.sunsetStart, 12.0f, 24.0f, "%.1f h"));
			Changed(SliderFloat("Dusk ends", &s.sunsetEnd, s.sunsetStart, 24.0f, "%.1f h"));
			SeparatorText("Degrees taken off");
			Changed(SliderInt("At dawn and dusk", &s.twilightPenalty, 0, 15, "%d"));
			Changed(SliderInt("At night", &s.nightPenalty, 0, 15, "%d"));
		}

		void RenderPlaces()
		{
			auto& s = Settings::Get();
			SeparatorText("Throat of the World");
			Changed(Checkbox("Dangerous mountain", &s.dangerousThroat));
			Tip("Eight tiers of cold on the way up to High Hrothgar and the peak, each 5 degrees worse than the one below (down to -35), instead of five tiers down to -20. The peak stays calm.");
			SeparatorText("Cities");
			Changed(Checkbox("Windhelm, Whiterun and Solitude can get cold", &s.coldCities));
			Tip("Frostfall never let Windhelm drop under 2 degrees. With this on the three cities are limited to -15 instead, so a blizzard or a winter night is felt there as well.");
		}

		void RenderTable()
		{
			const auto& s = Settings::Get();
			const int   month = Seasons::CurrentMonth();
			TextWrapped("Base temperature of each region in each month, before the weather and the time of day.");
			if (BeginTable("##seasons", Settings::kMonths + 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_SizingFixedFit)) {
				TableSetupColumn("Region");
				for (int m = 0; m < Settings::kMonths; m++) {
					TableSetupColumn(std::string(kMonthNames[m], 3).c_str());
				}
				TableHeadersRow();
				for (int r = 0; r < Settings::kRegionCount; r++) {
					TableNextRow();
					TableSetColumnIndex(0);
					Text("%s", kRegionNames[r]);
					for (int m = 0; m < Settings::kMonths; m++) {
						TableSetColumnIndex(m + 1);
						const int t = s.regionBase[r] + s.typeTemperature[s.monthType[m]];
						const ImVec4 colour = t <= -5 ? ImVec4{ 0.55f, 0.75f, 1.0f, 1.0f } : (t >= 18 ? ImVec4{ 1.0f, 0.75f, 0.45f, 1.0f } : ImVec4{ 0.9f, 0.9f, 0.9f, 1.0f });
						if (m == month) {
							TextColored(ImVec4{ 1.0f, 0.9f, 0.4f, 1.0f }, "%d", t);
						} else {
							TextColored(colour, "%d", t);
						}
					}
				}
				EndTable();
			}
		}

		void __stdcall RenderSettings()
		{
			auto& s = Settings::Get();

			Changed(Checkbox("Seasonal temperatures", &s.enabled));
			Tip("Off: Frostfall's own temperatures, one base value for each region all year and a single step at 19:00 and 7:00.");
			if (!s.enabled) {
				TextDisabled("Frostfall is using its own temperatures.");
			}

			RenderStatus();

			if (BeginTabBar("##seasonstabs")) {
				if (BeginTabItem("Regions")) { RenderRegions(); EndTabItem(); }
				if (BeginTabItem("Months")) { RenderMonths(); EndTabItem(); }
				if (BeginTabItem("Daylight")) { RenderDaylight(); EndTabItem(); }
				if (BeginTabItem("Places")) { RenderPlaces(); EndTabItem(); }
				if (BeginTabItem("Overview")) { RenderTable(); EndTabItem(); }
				EndTabBar();
			}

			Spacing();
			if (Button("Save now")) {
				s.Clamp();
				s.Save();
				dirty = false;
			}
			SameLine();
			if (Button("Reload from disk")) {
				s.Load();
				dirty = false;
			}
			SameLine();
			if (Button("Reset to defaults")) {
				const bool debug = s.debugLog;
				s = Settings{};
				s.debugLog = debug;
				Changed(true);
			}
			TextDisabled(dirty ? "Unsaved changes. They are saved when this menu closes." : "Saved to Data/SKSE/Plugins/FrostfallSeasons.ini");
		}
	}

	void Register()
	{
		if (!SKSEMenuFramework::IsInstalled()) {
			logger::info("SKSE Menu Framework is not installed, the settings stay in the ini");
			return;
		}
		// the same section as Frostfall 2026's own pages
		SKSEMenuFramework::SetSection("Frostfall");
		SKSEMenuFramework::AddSectionItem("Seasons", RenderSettings);
		SKSEMenuFramework::AddEvent(OnMenuEvent, 0.0f);  // never freed on purpose, like the framework's own examples
		logger::info("Registered the Seasons page with the SKSE Menu Framework");
	}
}
