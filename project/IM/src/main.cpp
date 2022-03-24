#include "sc2api/sc2_api.h"
#include "IMBot.h"

#include "sc2utils/sc2_arg_parser.h"
#include "sc2utils/sc2_manage_process.h"

//*************************************************************************************************
int main(int argc, char* argv[]) {
	sc2::Coordinator coordinator;
	if (!coordinator.LoadSettings(argc, argv)) {
		return 1;
	}

	coordinator.SetWindowLocation(800, 100);
	// true: 实际速度
	// false: 加速，允许使用暂停
	coordinator.SetRealtime(false);

	// 添加自定义bot
	sc2::IMBot bot;

	coordinator.SetParticipants({
		CreateParticipant(sc2::Race::Terran, &bot),
		CreateComputer(sc2::Race::Zerg),
		});


	// 启动游戏
	coordinator.LaunchStarcraft();

	// 加载地图
	//coordinator.StartGame("Example/MarineMicro_6enemy.SC2Map");
	coordinator.StartGame("Example/MarineMicro_distributed.SC2Map");
	//coordinator.StartGame("Example/MarineMicro_1enemy.SC2Map");

	while (coordinator.Update()) {
		if (sc2::PollKeyPress()) {
			break;
		}
	}

	return 0;
}
