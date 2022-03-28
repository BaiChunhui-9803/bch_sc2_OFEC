#define _MYALGORITHM_H_
#ifdef _MYALGORITHM_H_

#include <sc2api/sc2_api.h>
#include <vector>

#include "InfluenceMap.h"

using namespace sc2;
typedef uint32_t Gameloop;
typedef int IMPopId;
typedef std::vector<GridPoint> IMPop;

//��ı�ʾ,һ������Gameloop��־�Ķ���/λ������
typedef std::map<Gameloop, std::pair<sc2::ABILITY_ID, sc2::Point3D>> MySoL;
//��������ı�ʾ
typedef std::vector<MySoL> VecMySoL;

struct ValueMySol {
    Gameloop v_gameloop;
    sc2::ABILITY_ID v_ability_id;
    sc2::Point3D v_point;

    ValueMySol(Gameloop gameloop, sc2::ABILITY_ID ability_id, sc2::Point3D point) :
        v_gameloop(gameloop),
        v_ability_id(ability_id),
        v_point(point) {}
};

struct NodeUnit {
    sc2::Unit::Alliance n_alliance;
    sc2::Tag n_tag;
    sc2::UnitTypeID n_type;
    sc2::Point3D n_pos;
    IMPopId n_pop;
};

//�㷨����Ҫ����Ϸ��Ϣ
struct GameInfomation {
    std::vector<NodeUnit> info_vec_unit;
};

class MyAlgorithm {
private:
    //��Ž������
    VecMySoL myal_vec_sol;
    IMPop myal_IM_pop;

public:

    //�㷨��ʼ��
    void initialize_Algorithm(const GameInfomation& game_info, const IMPop& IM_pop);
    //�㷨����IM_pop����һ����

};





#endif // _MYALGORITHM_H_