#include "InfluenceMap.h"

MapPosition::MapPosition(UnitsVec& unit_vector) {
	if (unit_vector.size() != 0) {
		MapPoint pos_center;
		float sum_x = 0, sum_y = 0;
		for (const auto &it : unit_vector) {
			sum_x += it->pos.x;
			sum_y += it->pos.y;
		}
		m_center = MapPoint(sum_x / float(unit_vector.size()), sum_y / float(unit_vector.size()));
		m_top_left = MapPoint(m_center.x - float(MAP_X_LENGTH) / 2.0f, m_center.y + float(MAP_Y_LENGTH) / 2.0f);
		m_top_right = MapPoint(m_center.x + float(MAP_X_LENGTH) / 2.0f, m_center.y + float(MAP_Y_LENGTH) / 2.0f);
		m_bottom_left = MapPoint(m_center.x - float(MAP_X_LENGTH) / 2.0f, m_center.y - float(MAP_Y_LENGTH) / 2.0f);
		m_bottom_right = MapPoint(m_center.x + float(MAP_X_LENGTH) / 2.0f, m_center.y - float(MAP_Y_LENGTH) / 2.0f);
	}
}




InfluenceMap::InfluenceMap(UnitsVec& unit_vector, MapAlliance map_alliance) :
	m_map_position(MapPosition(unit_vector)), m_map_alliance(map_alliance) {
	m_map_arr;
}

GridPoint InfluenceMap::turnMapToGrid(const MapPoint& map_point) {
	float Xmin = this->m_map_position.getBottomLeft().x;
	float Xmax = this->m_map_position.getTopRight().x;
	float Ymin = this->m_map_position.getBottomRight().y;
	float Ymax = this->m_map_position.getTopLeft().y;

	float gridvalue = float(Xmax - Xmin) / float(MAP_GRID_SIZE);
	GridPoint grid_point(MAP_GRID_SIZE - int((map_point.y - Ymin) / gridvalue), int((map_point.x - Xmin) / gridvalue));
	return GridPoint(grid_point);
}

MapPoint InfluenceMap::turnGridToMap(const GridPoint & grid_point) {
	float Xmin = this->m_map_position.getBottomLeft().x;
	float Xmax = this->m_map_position.getTopRight().x;
	float Ymin = this->m_map_position.getBottomRight().y;
	float Ymax = this->m_map_position.getTopLeft().y;

	float gridvalue = float(Xmax - Xmin) / float(MAP_GRID_SIZE);
	float xmin = Xmin + gridvalue * (grid_point.y);
	float xmax = Xmin + gridvalue * (grid_point.y + 1);
	float ymin = Ymin + gridvalue * (MAP_GRID_SIZE - grid_point.x);
	float ymax = Ymin + gridvalue * (MAP_GRID_SIZE - grid_point.x + 1);

	float ran_x = gridvalue * float(rand()) / RAND_MAX + xmin;
	float ran_y = gridvalue * float(rand()) / RAND_MAX + ymin;
	return MapPoint(ran_x, ran_y);
}

void InfluenceMap::updateIMValue(const UnitsVec& unit_vector) {

	for (auto it = unit_vector.begin(); it != unit_vector.end(); ++it) {
		const sc2::Unit *const unit = *it;
		update_(unit);
	}

}

void InfluenceMap::update_(const sc2::Unit *const unit) {
	MapPoint center_mp = unit->pos;
	GridPoint center_gp = turnMapToGrid(center_mp);
	int max_x = MAP_GRID_SIZE - 1;
	int max_y = MAP_GRID_SIZE - 1;

	for (int i = 0; i < Rule_Size; ++i)
		update_beighbors(center_gp, unit->alliance, i);

}

void InfluenceMap::update_beighbors(GridPoint center, sc2::Unit::Alliance alliance, int level, InfluenceRule rule) {
	int max_x = MAP_GRID_SIZE - 1;
	int max_y = MAP_GRID_SIZE - 1;

	if (alliance == Self) {
		if (level == 0)
			m_map_arr[center.x][center.y] += rule.m_self_level[level];
		else {
			for (int dx = (center.x > 0 ? -1 * abs(level) : 0); dx <= (center.x < max_x ? abs(level) : 0); ++dx) {
				for (int dy = (center.y > 0 ? -1 * abs(level) : 0); dy <= (center.y < max_y ? abs(level) : 0); ++dy) {
					if (dx != 0 || dy != 0) {
						m_map_arr[center.x + dx][center.y + dy] += rule.m_self_level[level];
					}
				}
			}
		}
	}
	else if (alliance == Enemy) {
		if (level == 0)
			m_map_arr[center.x][center.y] += rule.m_enemy_level[level];
		else {
			for (int dx = (center.x > 0 ? -1 * abs(level) : 0); dx <= (center.x < max_x ? abs(level) : 0); ++dx) {
				for (int dy = (center.y > 0 ? -1 * abs(level) : 0); dy <= (center.y < max_y ? abs(level) : 0); ++dy) {
					if (dx != 0 || dy != 0) {
						m_map_arr[center.x + dx][center.y + dy] += rule.m_enemy_level[level];
					}
				}
			}
		}
	}

}

//void InfluenceMap::writeIMToFile() {
//	std::ofstream fout;
//	fout.open(fOutPathIMarr);
//}