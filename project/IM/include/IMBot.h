#ifndef _IMBOT_H_
#define _IMBOT_H_
#include <iostream>
#include <memory>

#include "sc2api/sc2_api.h"
#include "InfluenceMap.h"
#include "MyAlgorithm.h"

typedef float PerGameloop;
typedef float HPself;
typedef float HPenemy;
typedef std::vector<InfluenceMap> IMVec;
typedef boost::tuple<PerGameloop, Gameloop, HPself, HPenemy, InfluenceMap> IMNode;
typedef std::unique_ptr<boost::tuple<PerGameloop, Gameloop, HPself, HPenemy, InfluenceMap>> IMNode_ptr;
typedef std::list<std::unique_ptr<boost::tuple<PerGameloop, Gameloop, HPself, HPenemy, InfluenceMap>>> IMptrList;


std::ostream& operator<<(std::ostream& os, const InfluenceMap& IM);
std::ostream& operator<<(std::ostream& os, const arrint2D* arr);
std::ostream& operator<<(std::ostream& os, const GridPoint& grid_point);
std::ostream& operator<<(std::ostream& os, const MapPoint& map_point);

struct GameSet {
private:
    HPself max_HPself = 0.0f;
    HPenemy max_HPenemy = 0.0f;

public:
    GameSet() :max_HPself(0.0f), max_HPenemy(0.0f) {};
    const HPself getMaxHPSelf() const { return max_HPself; }
    const HPenemy getMaxHPEnemy() const { return max_HPenemy; }
    void updateGameSet(const UnitsVec& units_vec_self, const UnitsVec& units_vec_enemy) {
        for (auto &u : units_vec_self) max_HPself += u->health_max;
        for (auto &u : units_vec_enemy) max_HPenemy += u->health_max;
    }
};

namespace sc2 {
    class IMBot : public sc2::Agent {
    private:
        IMVec m_IM_vec;
        IMptrList m_IMptr_list = {};
        GameSet m_game_set = GameSet();
        GameInfomation m_game_info;
        InfluenceMap m_IM = InfluenceMap(Neutral);
        InfluenceMap m_IM_self = InfluenceMap(Self);
        InfluenceMap m_IM_enemy = InfluenceMap(Enemy);

    public:
        //友元类
        friend class MyAlgorithm;
        MyAlgorithm m_AL;


    public:
        // 游戏启动执行
        virtual void OnGameStart() final;
        // 游戏走帧执行 Your bots OnStep function will be called each time the coordinator steps the simulation forward.
        virtual void OnStep() final;



    public:
        //传递游戏信息
        bool updateGameInfo(const UnitsVec& units, const InfluenceMap& IM);
        const HPself getHPSelf(const UnitsVec& units) const;
        const HPenemy getHPEnemy(const UnitsVec& units) const;
    };


}

#endif // _IMBOT_H_