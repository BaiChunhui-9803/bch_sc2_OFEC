#include "sc2api/sc2_api.h"
#include "MicroKitingBot.h"
#include "MicroAgentBot.h"

#include "sc2utils/sc2_arg_parser.h"
#include "sc2utils/sc2_manage_process.h"


//*************************************************************************************************
int main(int argc, char* argv[]) {
	for (int i = 0; i < 100; i++) {
		sc2::Coordinator coordinator;
		if (!coordinator.LoadSettings(argc, argv)) {
			return 1;
		}

		coordinator.SetWindowLocation(800, 100);
		coordinator.SetRealtime(false);

		// 添加自定义bot
		//MicroKitingBot bot;
		MicroAgentBot bot;

		coordinator.SetParticipants({
			CreateParticipant(sc2::Race::Terran, &bot),
			CreateComputer(sc2::Race::Zerg),
			});


		// 启动游戏		
		coordinator.LaunchStarcraft();

		// 加载地图
		//coordinator.StartGame(sc2::kMapMarineMicro);
		coordinator.StartGame("Example/MarineMicro_6enemy.SC2Map");

		while (coordinator.Update()) {
			if (sc2::PollKeyPress()) {
				break;
			}

			if (bot.get_Game_Ended_()) {
				break;
			}
		}
	}

	return 0;
}
