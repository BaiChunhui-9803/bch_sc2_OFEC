#include <iostream>
#include <fstream>

#include "sc2api/sc2_api.h"

#include "IMBot.h"


namespace sc2 {

	void IMBot::OnGameStart() {

	}

	void IMBot::OnStep() {
		//观测器
		const ObservationInterface* observation = Observation();
		//控制器
		ActionInterface* action = Actions();
		//单元容器
		sc2::Units units_vec = observation->GetUnits();
		sc2::Units units_vec_self = observation->GetUnits(sc2::Unit::Alliance::Self);
		sc2::Units units_vec_enemy = observation->GetUnits(Unit::Alliance::Enemy);

		/***********************************************/
		InfluenceMap IM(units_vec, Neutral);
		InfluenceMap IM_self(units_vec_self, Self);
		InfluenceMap IM_enemy(units_vec_enemy, Enemy);
		IM.updateIMValue(units_vec);
		IM.writeIMarrToFile();
		std::cout << IM;
		auto in = IM.getMapArray();
		auto a=IM.m_map_arr;
		auto aa=&(IM.m_map_arr);
		int i;

	}
}

std::ostream& operator<<(std::ostream& os, const InfluenceMap& IM) {
	os << "IM:" << std::endl;
	os << "IM:m_map_position:" << IM.getMapPosition().getCenter() << std::endl;
	os << "IM:m_map_alliance:" << IM.getMapAlliance() << std::endl;
	os << "IM:m_map_arr:" << std::endl;
	os << IM.getMapArray();
	return os;
}

std::ostream& operator<<(std::ostream& os, const arrint2D* arr) {
	for (int i = 0; i < MAP_GRID_SIZE; ++i) {
		for (int j = 0; j < MAP_GRID_SIZE; ++j) {
			os << arr[i][j] << " ";
		}
		os << std::endl;
	}
	return os;
}


std::ostream& operator<<(std::ostream& os, const GridPoint& grid_point) {
	os << "(" << grid_point.x << "," << grid_point.y << ")";
	return os;
}

std::ostream & operator<<(std::ostream & os, const MapPoint & map_point)
{
	os << "(" << map_point.x << "," << map_point.y << ")";
	return os;
}