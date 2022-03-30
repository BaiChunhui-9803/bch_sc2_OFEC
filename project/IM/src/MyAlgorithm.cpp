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

}

sc2::Tag MyAlgorithm::findNearsetPoint() {
	float distance;
	float distance_min = 99.0f;
	int index;
	for (int i = 0; i < myal_game_info.info_vec_unit.size(); ++i) {
		if (myal_game_info.info_vec_unit.at(i).n_alliance == Enemy) {
			float distance = myal_IM.calculateWeightDistance(
				myal_IM_pop,
				myal_game_info.info_vec_unit.at(i).n_pop,
				myal_center_self,
				MapPoint(myal_game_info.info_vec_unit.at(i).n_pos));
			if (distance < distance_min) {
				distance_min = distance;
				index = i;
			}
		}
	}
	return myal_game_info.info_vec_unit.at(index).n_tag;
}

MapPoint MyAlgorithm::getKiteMapPoint(const int index) {
	MapPoint point_sum = myal_center_self;
	//for (int i = 0; i < myal_IM_pop.size(); ++i) {
	//	MapPoint point_n = myal_IM.turnGridToMap(myal_IM_pop.at(i).first);
	//	float k_n = myal_IM.calculateWeightDistance(
	//		myal_IM_pop,
	//		i,
	//		myal_center_self,
	//		point_n);
	//	point_sum += 1 / k_n * (myal_center_self - point_n);
	//}
	for (int i = 0; i < myal_IM_pop.size(); ++i) {
		if (i != index) {
			MapPoint point_n = myal_IM.turnGridToMap(myal_IM_pop.at(i).first);
			float k_n = myal_IM.calculateDistance(myal_center_self, point_n);
			point_sum += 2.25f / k_n * k_n * (myal_center_self - point_n);
		}
	}
	return point_sum;
}

int MyAlgorithm::getPopIndex(const sc2::Tag tag) {
	for (int i = 0; i < myal_game_info.info_vec_unit.size(); ++i) {
		if (myal_game_info.info_vec_unit.at(i).n_tag == tag) {
			return myal_game_info.info_vec_unit.at(i).n_pop;
		}
	}
	return 0;

}
//void MyAlgorithm::Algorithm() {
//
//}

//void MyAlgorithm::end_Algorithm() {
//	game_info.info_vec_unit.clear();
//}