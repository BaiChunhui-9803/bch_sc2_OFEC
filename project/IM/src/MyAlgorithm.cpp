#include "MyAlgorithm.h"



void MyAlgorithm::initialize_Algorithm(const GameInfomation& game_info, const IMPop& IM_pop) {
	myal_vec_sol.clear();
	myal_game_info = game_info;
	myal_IM_pop = IM_pop;
	std::vector<MapPoint> vec_mappoint;
	for (auto it = game_info.info_vec_unit.begin(); it != game_info.info_vec_unit.end(); ++it) {
		if (it->n_alliance == Enemy) {
			MapPoint point(it->n_pos.x, it->n_pos.y);
			vec_mappoint.push_back(point);
		}
	}
	myal_center_self = myal_IM.getCenterMapPoint(vec_mappoint);
}

//void MyAlgorithm::Algorithm() {
//
//}

//void MyAlgorithm::end_Algorithm() {
//	game_info.info_vec_unit.clear();
//}