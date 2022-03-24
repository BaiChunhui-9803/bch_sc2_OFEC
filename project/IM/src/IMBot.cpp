#include <iostream>
#include <fstream>

#include "sc2api/sc2_api.h"

#include "IMBot.h"

int i = 0;
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
		/****************传递/更新/存储IM*****************/
		//InfluenceMap IM1(units_vec, Neutral);
		//InfluenceMap IM2(units_vec_self, Self);
		//InfluenceMap IM3(units_vec_enemy, Enemy);
		//m_IM = IM1;
		//m_IM_self = IM2;
		//m_IM_enemy = IM3;

		m_IM.updateIMValue(units_vec);
		m_IM_self.updateIMValue(units_vec_self);
		m_IM_enemy.updateIMValue(units_vec_enemy);
		m_IM.writeIMarrToFile(fOutPathIMarr, fOutPathIMarr3r);
		m_IM_self.writeIMarrToFile(fOutPathIMarrSelf, fOutPathIMarrSelf3r);
		m_IM_enemy.writeIMarrToFile(fOutPathIMarrEnemy, fOutPathIMarrEnemy3r);
		//std::cout << IM;
#ifdef GNUPLOT_IOSTREAM_H
		++i;
		if (i % 10 == 0) {
			displayIMarr();
		}
#endif
		/***********************************************/
		/*************算法初始化/传递游戏信息*************/
		//updateGameInfo(units_vec, IM);






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

bool sc2::IMBot::updateGameInfo(const UnitsVec& units, const InfluenceMap& IM) {
	for (auto &u : units) {
		NodeUnit node_unit;
		node_unit.n_alliance = u->alliance;
		node_unit.n_tag = u->tag;
		node_unit.n_type = u->unit_type;
		node_unit.n_pos = u->pos;
		m_game_info.info_vec_unit.push_back(node_unit);
	}
	//m_game_info.info_IM = IM;
	return true;
}