#define CATCH_CONFIG_RUNNER
#include "../utility/catch.hpp"

//ctime务必在前
#include <ctime>
#include <sc2api/sc2_api.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "sc2utils/sc2_manage_process.h"


#include "../project/pathing_2agent/include/GA_TSP.h"
#include "../project/pathing_2agent/include/Mineral_TSP.h"
#include "../project/pathing_2agent/include/pathing_bot.h"


using namespace std;
int main(int argc, char* argv[]) {

	sc2::Coordinator coordinator;
	//coordinator.LoadSettings(argc, argv);
	if (!coordinator.LoadSettings(argc, argv)) {
		return 1;
	}

	//coordinator.SetFeatureLayers(sc2::FeatureLayerSettings(24.0f, 64, 64, 64, 64));

	coordinator.SetWindowLocation(800, 100);
	coordinator.SetRealtime(true);
	//coordinator.SetWindowSize(100, 100);
	//coordinator.SetRender(sc2::RenderSettings(100,100,100,100));
	//coordinator.SetFeatureLayers(FeatureLayerSettings(24.0f, 80, 80, 80, 80));


	PathingBot bot;
	coordinator.SetParticipants({
		CreateParticipant(sc2::Race::Terran, &bot),
		sc2::CreateComputer(sc2::Race::Zerg)
		});

	coordinator.LaunchStarcraft();
	const char* kmap = "mini_games/CollectMineralShards_12000s.SC2Map";
	coordinator.StartGame(kmap);

	while (coordinator.Update()) {
		if (sc2::PollKeyPress()) {
			break;
		}
	}

	return 0;
}


//using namespace std;
//int main(int argc, char* argv[]) {
//	for (int id(0); id < argc; ++id) {
//		cout << argv[id] << endl;
//	}
//	time_t timer_start, timer_end;
//	OFEC::registerProblem();
//	OFEC::registerAlgorithm();
//	OFEC::setAlgForPro();
//	time(&timer_start);
//#ifdef OFEC_UNIT_TEST
//	int result = Catch::Session().run(argc, argv);
//#else
//	OFEC::run();
//#endif
//	time(&timer_end);
//	std::cout << "Time used: " << difftime(timer_end, timer_start) << " seconds" << std::endl;
//	return 0;
//}