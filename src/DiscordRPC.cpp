#include "DiscordRPC.hpp"
#include "discord_register.h"
#include <cstring>
#include <ctime>


static const char* APPLICATION_ID = "431943416071323649";

void DiscordRPC::InitDiscord()
{
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = handleDiscordReady;
	handlers.errored = handleDiscordError;
	handlers.disconnected = handleDiscordDisconnected;
	handlers.joinGame = handleDiscordJoin;
	handlers.spectateGame = handleDiscordSpectate;
	handlers.joinRequest = handleDiscordJoinRequest;

	// Discord_Initialize(const char* applicationId, DiscordEventHandlers* handlers, int autoRegister, const char* optionalSteamId)
	Discord_Initialize(APPLICATION_ID, &handlers, 1, nullptr);
}

void DiscordRPC::ShutdownDiscord() {
	Discord_Shutdown();
}

void DiscordRPC::RunCallbacks() {
	Discord_RunCallbacks();
}

void DiscordRPC::UpdatePresence(std::string details) {
	char buffer[256];
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = "Playing Solo";
	discordPresence.details = details.c_str();
	discordPresence.startTimestamp = std::time(0);
	discordPresence.instance = 1;
	Discord_UpdatePresence(&discordPresence);
}

void DiscordRPC::handleDiscordReady()
{
	printf("\nDiscord: ready\n");
}

void DiscordRPC::handleDiscordDisconnected(int errcode, const char* message)
{
	printf("\nDiscord: disconnected (%d: %s)\n", errcode, message);
}

void DiscordRPC::handleDiscordError(int errcode, const char* message)
{
	printf("\nDiscord: error (%d: %s)\n", errcode, message);
}

void DiscordRPC::handleDiscordJoin(const char* secret)
{
	printf("\nDiscord: join (%s)\n", secret);
}

void DiscordRPC::handleDiscordSpectate(const char* secret)
{
	printf("\nDiscord: spectate (%s)\n", secret);
}

void DiscordRPC::handleDiscordJoinRequest(const DiscordJoinRequest* request)
{
	printf("\nDiscord: join request\n");
	Discord_Respond(request->userId, DISCORD_REPLY_NO);
}

