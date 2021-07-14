#pragma once

#include "sc2api/sc2_api.h"
#include <iostream>

namespace sc2 {

class MicroKitingBot : public sc2::Agent {
public:
    // ��Ϸ����ִ��
    virtual void OnGameStart() final;
    // ��Ϸ��ִ֡�� Your bots OnStep function will be called each time the coordinator steps the simulation forward.
    virtual void OnStep() final;
    // ��λ���ݻ�ִ��
    virtual void OnUnitDestroyed(const Unit* unit) override;

private:
    // ��ȡ��λΪalliace��ƽ��position ����Point2D& position
    bool GetPosition(UNIT_TYPEID unit_type, Unit::Alliance alliace, Point2D& position);
    // ��ȡ������嵥λ ����Point2D& from
    bool GetNearestZergling(const Point2D& from);

    //����Ŀ�굥λ��ָ��Unit*
    const Unit* targeted_zergling_;
    //�ƶ���־λ
    bool move_back_;
    Point2D backup_target_;
    Point2D backup_start_;
};


}


