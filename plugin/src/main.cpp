#include "Menu.h"
#include "Seasons.h"
#include "Settings.h"

namespace
{
	void InitializeLog()
	{
		auto path = logger::log_directory();
		if (!path) {
			return;
		}
		*path /= "FrostfallSeasons.log"sv;
		auto log = std::make_shared<spdlog::logger>("global log"s, std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);
		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("[%H:%M:%S.%e] [%l] %v"s);
	}

	void OnMessage(SKSE::MessagingInterface::Message* a_message)
	{
		if (a_message->type == SKSE::MessagingInterface::kDataLoaded) {
			Menu::Register();
		}
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	SKSE::Init(a_skse);
	InitializeLog();

	auto& settings = Settings::Get();
	settings.Load();
	if (settings.debugLog) {
		spdlog::set_level(spdlog::level::debug);
	}
	settings.Save();  // writes every key when there is no ini yet, so there is something to edit

	Seasons::RegisterNatives();
	SKSE::GetMessagingInterface()->RegisterListener(OnMessage);

	logger::info("Frostfall Seasons loaded, seasonal temperatures {}", settings.enabled ? "on" : "off");
	return true;
}
