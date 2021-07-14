#pragma once
#include "sc2api/sc2_api.h"
#include <iostream>

using namespace sc2;

class MicroAgentBot : public sc2::Agent {
public:
    // 游戏启动执行
    virtual void OnGameStart() final;
    // 游戏走帧执行 Your bots OnStep function will be called each time the coordinator steps the simulation forward.
    virtual void OnStep() final;
    // 单位被摧毁执行
    virtual void OnUnitDestroyed(const Unit* unit) override;

private:
    // 获取单位为alliace的平均position 更新Point2D& position
    bool GetPosition(UNIT_TYPEID unit_type, Unit::Alliance alliace, Point2D& position);
    // 获取最近虫族单位 更新Point2D& from
    bool GetNearestZergling(const Point2D& from);

    const Unit* targeted_zergling_;
    bool move_back_;
    Point2D backup_target_;
    Point2D backup_start_;
};