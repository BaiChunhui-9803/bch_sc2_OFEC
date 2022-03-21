#include <iostream>

#include "sc2api/sc2_api.h"

#include "InfluenceMap.h"
typedef std::vector<InfluenceMap> IM_VEC;

std::ostream& operator<<(std::ostream& os, const InfluenceMap& IM);
std::ostream& operator<<(std::ostream& os, const arrint2D* arr);
std::ostream& operator<<(std::ostream& os, const GridPoint& grid_point);
std::ostream& operator<<(std::ostream& os, const MapPoint& map_point);

namespace sc2 {
    class IMBot : public sc2::Agent {
    private:
        IM_VEC m_IM_vec;



    public:
        // 游戏启动执行
        virtual void OnGameStart() final;
        // 游戏走帧执行 Your bots OnStep function will be called each time the coordinator steps the simulation forward.
        virtual void OnStep() final;


    private:

    };


}