#define _MYALGORITHM_H_
#ifdef _MYALGORITHM_H_

#include <sc2api/sc2_api.h>
#include <vector>

#include "InfluenceMap.h"

using namespace sc2;
typedef uint32_t Gameloop;
typedef int IMPopId;
typedef std::vector<GridPoint> IMPop;

//解的表示,一个带有Gameloop标志的动作/位置序列
typedef std::map<Gameloop, std::pair<sc2::ABILITY_ID, sc2::Point3D>> MySoL;
//解的容器的表示
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

//算法所需要的游戏信息
struct GameInfomation {
    std::vector<NodeUnit> info_vec_unit;
};

class MyAlgorithm {
private:
    //存放解的容器
    VecMySoL myal_vec_sol;
    IMPop myal_IM_pop;

public:

    //算法初始化
    void initialize_Algorithm(const GameInfomation& game_info, const IMPop& IM_pop);
    //算法根据IM_pop生成一个解

};





#endif // _MYALGORITHM_H_