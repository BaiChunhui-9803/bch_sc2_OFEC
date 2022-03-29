#include "MyAlgorithm.h"



void MyAlgorithm::initialize_Algorithm(const GameInfomation& game_info, const IMPopVec& IM_pop) {
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

	for (int i = 0; i < game_info.info_vec_unit.size(); ++i) {
		if (game_info.info_vec_unit.at(i).n_alliance == Enemy) {
			float distance_w = myal_IM.calculateWeightDistance(
				myal_IM_pop,
				myal_game_info.info_vec_unit.at(i).n_pop,
				myal_center_self,
				MapPoint(game_info.info_vec_unit.at(i).n_pos));
			std::cout << myal_game_info.info_vec_unit.at(i).n_pop << ": " << distance_w << std::endl;
		}
	}

}

//void MyAlgorithm::Algorithm() {
//
//}

//void MyAlgorithm::end_Algorithm() {
//	game_info.info_vec_unit.clear();
//}