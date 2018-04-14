#include <stdlib.h>
#include "common.hpp"
#include "discord_rpc.h"

class DiscordRPC {
public:
	static void InitDiscord();
	static void ShutdownDiscord();
	static void RunCallbacks();
	static void UpdatePresence(std::string details);
private:
	static void handleDiscordReady();
	static void handleDiscordDisconnected(int errcode, const char * message);
	static void handleDiscordError(int errcode, const char * message);
	static void handleDiscordJoin(const char * secret);
	static void handleDiscordSpectate(const char * secret);
	static void handleDiscordJoinRequest(const DiscordJoinRequest * request);
};