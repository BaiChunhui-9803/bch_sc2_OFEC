#include <vector>
#include <iostream>
#include <fstream>

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"
#include "sc2renderer/sc2_renderer.h"

//��ɢ����ͼ�ĳ���/���
#define MAP_GRID_SIZE 50
//ʵ������ͼ�ĳ���
#define MAP_X_LENGTH 50
//ʵ������ͼ�Ŀ��
#define MAP_Y_LENGTH 50
//������Χ��С
#define Rule_Size 4

//UnitsVec ��Ԫ�����������з����ѷ�
typedef sc2::Units UnitsVec;
//MapPoint ʵ������ͼ����
typedef sc2::Point2D MapPoint;
//GridPoint ��ɢ����ͼ���꣬ӳ������ɢ������
typedef sc2::Point2DI GridPoint;
//arrint2D* ��ά����ָ��
typedef const int arrint2D[MAP_GRID_SIZE];

//��д�ļ�·��
static const char* fOutPathIMarr = "D:/bch_sc2_OFEC/sc2api/project/IM/datafile/IM_arr.txt";

//����ͼ�ĵ��ҹ�ϵ
enum MapAlliance {
    Self = 1,
    Enemy = 4,
    Neutral = 3,
};

//������Χ��״
enum InfluenceShape {
    Square = 1,
    Circle = 2,
};

struct InfluenceRule {
    InfluenceShape m_shape = Square;
    unsigned int m_range = Rule_Size;
    //���ڵ���ԭ����������Ϊ16��9=5+3+1��4=3+1��1
    int m_self_level[Rule_Size] = { 16, 5, 3, 1 };
    int m_enemy_level[Rule_Size] = { -16, -5, -3, -1 };
};

//����ͼ�Ĺؼ�������
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



//����ͼ��
class InfluenceMap {
    friend class MapPosition;
public:
    //����ͼ��λ��
    MapPosition m_map_position;
    //����ͼ�ĵ��ҹ�ϵ
    MapAlliance m_map_alliance;
    //����ͼ����ɢ����
    int m_map_arr[MAP_GRID_SIZE][MAP_GRID_SIZE] = {};

public:
    InfluenceMap(UnitsVec& unit_vector, MapAlliance map_alliance);
    ~InfluenceMap() = default;

    MapPosition getMapPosition() const { return m_map_position; }
    MapAlliance getMapAlliance() const { return m_map_alliance; }
    arrint2D* getMapArray() const { return m_map_arr; }


    //����MapPoint���꣬����תΪ�����еĵ�
    GridPoint turnMapToGrid(const MapPoint& map_point);
    //����GridPoint���꣬����תΪʵ������ͼ�еĵ�
    MapPoint turnGridToMap(const GridPoint& grid_point);
    //����unit_vector��������ͼ�е�����ֵ
    void updateIMValue(const UnitsVec& unit_vector);

    void writeIMarrToFile();

private:
    void update_(const sc2::Unit *const unit);
    void update_beighbors(GridPoint center, sc2::Unit::Alliance alliance, int level, InfluenceRule rule = InfluenceRule());
};