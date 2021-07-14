#pragma once

#include "sc2api/sc2_interfaces.h"
#include "sc2api/sc2_agent.h"
#include "sc2api/sc2_map_info.h"

#include "Mineral_TSP.h"
#include "GA_TSP.h"

using namespace sc2;

enum Game_Stage {
    //空环节
    Blank_Flag = 0,
    //节点更新环节
    Update_Flag = 1,
    //执行算法环节
    Algorithm_Flag = 2,
    //单元移动环节
    Move_Flag = 3,
    //游戏结束环节
    End_Flag = 4
};

//友元类前向声明
class Mineral_TSP;
class GA_TSP;

class PathingBot :public Agent {
private:
    int i = 0;
    Game_Stage m_game_stage = Update_Flag;
public:
    //友元类
    friend class Mineral_TSP;
    friend class GA_TSP;
    //Mineral_TSP方法的执行器，供Bot使用
    Mineral_TSP m_Actuator;
    //GA_TSP方法的执行器，供Bot使用
    GA_TSP m_GA;
    //通过算法得到的最优部署解
    std::vector<Node_TSP> m_best_solution;
    bool flag = false;
    bool is_end(const sc2::ObservationInterface* observation);
    int mineral_now = 0;

public:
    virtual void OnGameStart() final {
        std::cout << "Hello, World!" << std::endl;
    }

    virtual void OnStep() final {
        //观测器
        const sc2::ObservationInterface* observation = Observation();
        //控制器
        sc2::ActionInterface* action = Actions();
        //单元容器
        sc2::Units units_vec = observation->GetUnits();

        mineral_now = observation->GetMinerals();

        if (mineral_max < observation->GetMinerals()) {
            mineral_max = observation->GetMinerals();
            std::cout << "GameLoop:" << observation->GetGameLoop() << '\t';
            std::cout << "Minerals:" << observation->GetMinerals() << std::endl;

        }

        if (m_game_stage == Update_Flag) {
            if (m_Actuator.update_Unit(units_vec)) {
                m_game_stage = Algorithm_Flag;
                std::cout << "结点信息更新完毕，进行优化" << std::endl;
            }
        }

        if (m_game_stage == Algorithm_Flag) {
            m_GA.initialize_Algorithm(m_Actuator.get_mineral_vec());
            m_best_solution = m_GA.GA();
            //m_GA.GA();
            m_game_stage = Move_Flag;
        }
        std::vector<sc2::Point2D> point_vec;
        for (auto it : m_best_solution) {
            sc2::Point2D point(it.m_x, it.m_y);
            point_vec.push_back(point);
        }

        if (flag == true && mineral_now % ((UnitSize - 2) * 100) != 0) {
            flag = false;
        }

        if (!point_vec.empty()) {
            for (const auto&u : units_vec) {
                if (u->unit_type == UNIT_TYPEID::TERRAN_MARINE && (Distance2D(u->pos, point_vec.at(point_vec.size() - 1)) > 1.5f)) {
                    sc2::Point2D point_begin(u->pos.x, u->pos.y);
                    if (m_game_stage == Move_Flag) {
                        action->UnitCommand(u, ABILITY_ID::MOVE, point_vec.at(i + 1));
                        i++;
                        m_game_stage = Blank_Flag;
                    }
                    else {
                        if (Distance2D(point_begin, point_vec.at(i)) < 1.5f) {
                            m_game_stage = Move_Flag;
                        }
                    }
                    if (flag == false && observation->GetMinerals() != 0 && observation->GetMinerals() % 2000 == 0) {
                        action->UnitCommand(u, ABILITY_ID::STOP);
                        point_vec.clear();
                        m_Actuator.get_mineral_vec().clear();
                        m_GA.~GA_TSP();
                        m_game_stage = Update_Flag;
                        flag = true;
                        mineral_now = observation->GetMinerals();
                        i = 0;
                        break;
                    }

                }
            }
        }


    }

    int mineral_max = 0;
};

bool PathingBot::is_end(const sc2::ObservationInterface* observation) {
    if (observation->GetMinerals() != 0 && observation->GetMinerals() % 2000 == 0) {
        return true;
    }
    else {
        return false;
    }
}