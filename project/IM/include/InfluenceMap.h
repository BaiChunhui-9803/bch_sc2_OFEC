#include <vector>
#include <iostream>
#include <fstream>

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2renderer/sc2_renderer.h"

//离散势力图的长度/宽度
#define MAP_GRID_SIZE 50
//实际势力图的长度
#define MAP_X_LENGTH 50
//实际势力图的宽度
#define MAP_Y_LENGTH 50
//势力范围大小
#define Rule_Size 4

//UnitsVec 单元容器，包含敌方、友方
typedef sc2::Units UnitsVec;
//MapPoint 实际势力图坐标
typedef sc2::Point2D MapPoint;
//GridPoint 离散势力图坐标，映射在离散矩阵里
typedef sc2::Point2DI GridPoint;
//arrint2D* 二维数组指针
typedef const int arrint2D[MAP_GRID_SIZE];

//读写文件路径
static const char* fOutPathIMarr = "D:/bch_sc2_OFEC/sc2api/project/IM/datafile/IM_arr.txt";

//势力图的敌我关系
enum MapAlliance {
    Self = 1,
    Enemy = 4,
    Neutral = 3,
};

//势力范围形状
enum InfluenceShape {
    Square = 1,
    Circle = 2,
};

struct InfluenceRule {
    InfluenceShape m_shape = Square;
    unsigned int m_range = Rule_Size;
    //由于递增原因，势力增长为16、9=5+3+1、4=3+1、1
    int m_self_level[Rule_Size] = { 16, 5, 3, 1 };
    int m_enemy_level[Rule_Size] = { -16, -5, -3, -1 };
};

//势力图的关键点坐标
class MapPosition {
private:
    MapPoint m_center;
    MapPoint m_top_left;
    MapPoint m_top_right;
    MapPoint m_bottom_left;
    MapPoint m_bottom_right;

public:
    MapPosition() :
        m_center(MapPoint(0.0f, 0.0f)),
        m_top_left(MapPoint(0.0f - MAP_X_LENGTH / 2.0, 0.0f + MAP_Y_LENGTH / 2.0)),
        m_top_right(MapPoint(0.0f + MAP_X_LENGTH / 2.0, 0.0f + MAP_Y_LENGTH / 2.0)),
        m_bottom_left(MapPoint(0.0f - MAP_X_LENGTH / 2.0, 0.0f - MAP_Y_LENGTH / 2.0)),
        m_bottom_right(MapPoint(0.0f + MAP_X_LENGTH / 2.0, 0.0f - MAP_Y_LENGTH / 2.0)) {};
    ~MapPosition() = default;
    MapPosition(UnitsVec& unit_vector);
    MapPoint getCenter() const { return m_center; }
    MapPoint getTopLeft() const { return m_top_left; }
    MapPoint getTopRight() const { return m_top_right; }
    MapPoint getBottomLeft() const { return m_bottom_left; }
    MapPoint getBottomRight() const { return m_bottom_right; }
};



//势力图类
class InfluenceMap {
    friend class MapPosition;
public:
    //势力图的位置
    MapPosition m_map_position;
    //势力图的敌我关系
    MapAlliance m_map_alliance;
    //势力图的离散矩阵
    int m_map_arr[MAP_GRID_SIZE][MAP_GRID_SIZE] = {};

public:
    InfluenceMap(UnitsVec& unit_vector, MapAlliance map_alliance);
    ~InfluenceMap() = default;

    MapPosition getMapPosition() const { return m_map_position; }
    MapAlliance getMapAlliance() const { return m_map_alliance; }
    arrint2D* getMapArray() const { return m_map_arr; }


    //给定MapPoint坐标，将其转为矩阵中的点
    GridPoint turnMapToGrid(const MapPoint& map_point);
    //给定GridPoint坐标，将其转为实际势力图中的点
    MapPoint turnGridToMap(const GridPoint& grid_point);
    //根据unit_vector更新势力图中的势力值
    void updateIMValue(const UnitsVec& unit_vector);

    void writeIMarrToFile();

private:
    void update_(const sc2::Unit *const unit);
    void update_beighbors(GridPoint center, sc2::Unit::Alliance alliance, int level, InfluenceRule rule = InfluenceRule());
};