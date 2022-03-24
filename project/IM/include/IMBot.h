#ifndef _IMBOT_H_
#define _IMBOT_H_
#include <iostream>

#include "sc2api/sc2_api.h"

#include "InfluenceMap.h"
#include "MyAlgorithm.h"

typedef std::vector<InfluenceMap> IM_VEC;

std::ostream& operator<<(std::ostream& os, const InfluenceMap& IM);
std::ostream& operator<<(std::ostream& os, const arrint2D* arr);
std::ostream& operator<<(std::ostream& os, const GridPoint& grid_point);
std::ostream& operator<<(std::ostream& os, const MapPoint& map_point);

namespace sc2 {
    class IMBot : public sc2::Agent {
    private:
        IM_VEC m_IM_vec;
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
    };


}

#endif // _IMBOT_H_