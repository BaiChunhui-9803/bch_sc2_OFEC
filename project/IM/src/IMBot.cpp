#include <iostream>
#include <fstream>

#include "sc2api/sc2_api.h"

#include "IMBot.h"

int i = 0;
namespace sc2 {

	void IMBot::OnGameStart() {
		//观测器
		const ObservationInterface* static_observation = Observation();
		//控制器
		ActionInterface* static_action = Actions();
		//单元容器
		UnitsVec static_units_vec = static_observation->GetUnits();
		UnitsVec static_units_vec_self = static_observation->GetUnits(sc2::Unit::Alliance::Self);
		UnitsVec static_units_vec_enemy = static_observation->GetUnits(Unit::Alliance::Enemy);
		/****************设置游戏基本参数****************/
		m_game_set.updateGameSet(static_units_vec_self, static_units_vec_enemy);
		/***********************************************/
	}

	void IMBot::OnStep() {
		//观测器
		const ObservationInterface* observation = Observation();
		//控制器
		ActionInterface* action = Actions();
		//单元容器
		UnitsVec units_vec = observation->GetUnits();
		UnitsVec units_vec_self = observation->GetUnits(sc2::Unit::Alliance::Self);
		UnitsVec units_vec_enemy = observation->GetUnits(Unit::Alliance::Enemy);
		/***********************************************/
		/****************传递/更新/存储IM****************/
		//InfluenceMap IM1(units_vec, Neutral);
		InfluenceMap IM2(units_vec_self, Self);
		InfluenceMap IM3(units_vec_enemy, Enemy);
		//m_IM = IM1;
		//m_IM_self = IM2;
		//m_IM_enemy = IM3;
		m_IMptr_list.push_back(std::make_unique<IMNode>(boost::make_tuple(
			0.0f,
			observation->GetGameLoop(),
			this->getHPSelf(units_vec_self) / m_game_set.getMaxHPSelf(),
			this->getHPEnemy(units_vec_enemy) / m_game_set.getMaxHPEnemy(),
			InfluenceMap(units_vec, Neutral))));
		m_IMptr_list.back()->get<4>().updateIMValue(units_vec);
		//m_IM.updateIMValue(units_vec);
		IM2.updateIMValue(units_vec_self);
		IM3.updateIMValue(units_vec_enemy);
		m_IMptr_list.back()->get<4>().writeIMarrToFile(fOutPathIMarr, fOutPathIMarr3r);
		IM2.writeIMarrToFile(fOutPathIMarrSelf, fOutPathIMarrSelf3r);
		IM3.writeIMarrToFile(fOutPathIMarrEnemy, fOutPathIMarrEnemy3r);
		//std::cout << IM;
#ifdef GNUPLOT_IOSTREAM_H
		++i;
		if (i % 10 == 0) {
			displayIMarr();
		}
#endif
		/***********************************************/
		/*************算法初始化/传递游戏信息*************/
		updateGameInfo(units_vec, m_IM);






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

const HPself sc2::IMBot::getHPSelf(const UnitsVec& units) const {
	HPself value = 0.0f;
	for (auto& u : units) {
		value += u->health;
	}
	return value;
}

const HPenemy sc2::IMBot::getHPEnemy(const UnitsVec& units) const {
	HPenemy value = 0.0f;
	for (auto& u : units) {
		value += u->health;
	}
	return value;
}