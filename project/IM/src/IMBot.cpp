#include <iostream>
#include <fstream>

#include "sc2api/sc2_api.h"

#include "IMBot.h"

Gameloop display_cnt = 0;
Gameloop gameloop_cnt = 0;
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
		InfluenceMap IM2(units_vec_self, Self);
		InfluenceMap IM3(units_vec_enemy, Enemy);
		m_IMptr_list.push_back(std::make_unique<IMNode>(boost::make_tuple(
			0.0f,
			observation->GetGameLoop(),
			this->getHPSelf(units_vec_self) / m_game_set.getMaxHPSelf(),
			this->getHPEnemy(units_vec_enemy) / m_game_set.getMaxHPEnemy(),
			InfluenceMap(units_vec, Neutral))));
		m_IMptr_list.back()->get<4>().updateIMValue(units_vec);
		IM2.updateIMValue(units_vec_self);
		IM3.updateIMValue(units_vec_enemy);
		m_IMptr_list.back()->get<4>().writeIMarrToFile(fOutPathIMarr, fOutPathIMarr3r);
		IM2.writeIMarrToFile(fOutPathIMarrSelf, fOutPathIMarrSelf3r);
		IM3.writeIMarrToFile(fOutPathIMarrEnemy, fOutPathIMarrEnemy3r);
		//std::cout << IM;
#ifdef GNUPLOT_IOSTREAM_H
		//if (display_cnt % 10 == 0) {
			displayIMarr();
		//}
#endif
		/***********************************************/
		/*************算法初始化/传递游戏信息*************/
		m_IM_pop = updateIMPop(units_vec_enemy);
		updateGameInfo(units_vec);
		if (gameloop_cnt % 10 == 0) updatePerGameLoop(gameloop_cnt);











		/***********************************************/
		/********************变量自增********************/
		//游戏循环计数器
		++display_cnt;
		++gameloop_cnt;
		/***********************************************/
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

bool sc2::IMBot::updateGameInfo(const UnitsVec& units) {
	for (auto &u : units) {
		NodeUnit node_unit;
		node_unit.n_alliance = u->alliance;
		node_unit.n_tag = u->tag;
		node_unit.n_type = u->unit_type;
		node_unit.n_pos = u->pos;
		node_unit.n_pop = getIMPop(MapPoint(u->pos));
		m_game_info.info_vec_unit.push_back(node_unit);
	}
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

bool sc2::IMBot::updatePerGameLoop(const Gameloop loop_cnt) {
	for (auto it = this->m_IMptr_list.begin(); it != this->m_IMptr_list.end(); ++it) {
		(*it)->head = float((*it)->get<1>()) / float(loop_cnt);
	}
	return true;
}

std::vector<GridPoint> sc2::IMBot::updateIMPop(const UnitsVec& units_vec_enemy) {
	int gridmap[MAP_GRID_SIZE][MAP_GRID_SIZE] = {};
	std::vector<GridPoint> gridvec;
	int i = 0;
	for (auto &u : units_vec_enemy) {
		GridPoint gridpoint = m_IM.turnMapToGrid(MapPoint(u->pos));
		std::vector<int> beighbor_vec = findBeighborsIn2(gridpoint, gridmap);
		if (isNewPop(gridpoint, beighbor_vec)) {
			++i;
			gridmap[gridpoint.x][gridpoint.y] = i;
			gridvec.push_back(GridPoint(gridpoint.x, gridpoint.y));
		}

	}
	return gridvec;
}

bool sc2::IMBot::isNewPop(const GridPoint& grid_point, const std::vector<int>& vec) {
	for (int i = 0; i < vec.size(); ++i) {
		if (vec.at(i) != 0) return false;
	}
	return true;
}

std::vector<int> sc2::IMBot::findBeighborsIn2(const GridPoint& grid_point, arrint2D* arr_ptr) {
	int max_x = MAP_GRID_SIZE - 1;
	int max_y = MAP_GRID_SIZE - 1;
	std::vector<int> list;
	list.push_back(arr_ptr[grid_point.x][grid_point.y]);
	for (int dx = (grid_point.x > 0 ? -2 : 0); dx <= (grid_point.x < max_x ? 2 : 0); ++dx) {
		for (int dy = (grid_point.y > 0 ? -2 : 0); dy <= (grid_point.y < max_y ? 2 : 0); ++dy) {
			if (dx != 0 || dy != 0) {
				list.push_back(arr_ptr[grid_point.x + dx][grid_point.y + dy]);
			}
		}
	}

	return list;
}

IMPopId sc2::IMBot::getIMPop(const MapPoint& map_point) {
	GridPoint gridppint = m_IM.turnMapToGrid(map_point);
	int distance_min = 0;
	for (int i = 0; i < m_IM_pop.size(); ++i) {

	}



	return IMPopId();
}