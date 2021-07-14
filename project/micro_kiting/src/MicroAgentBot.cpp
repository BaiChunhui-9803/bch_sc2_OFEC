#include "sc2api/sc2_api.h"
#include "MicroAgentBot.h"
#include <iostream>

using namespace sc2;

void MicroAgentBot::OnGameStart() {
	move_back_ = false;
	targeted_zergling_ = 0;
}

void MicroAgentBot::OnStep() {
	const ObservationInterface* observation = Observation();
	ActionInterface* action = Actions();

	Point2D mp, zp;

	if (!GetPosition(UNIT_TYPEID::TERRAN_MARINE, Unit::Alliance::Self, mp)) {
		return;
	}

	if (!GetPosition(UNIT_TYPEID::ZERG_ZERGLING, Unit::Alliance::Enemy, zp)) {
		return;
	}

	if (!GetNearestZergling(mp)) {
		return;
	}

	Units units = observation->GetUnits(Unit::Alliance::Self);
	for (const auto& u : units) {
		switch (static_cast<UNIT_TYPEID>(u->unit_type)) {
		case UNIT_TYPEID::TERRAN_MARINE: {
			if (!move_back_) {
				action->UnitCommand(u, ABILITY_ID::ATTACK, targeted_zergling_);

			}
			else {
				if (Distance2D(mp, backup_target_) < 1.5f) {
					move_back_ = false;
				}

				action->UnitCommand(u, ABILITY_ID::SMART, backup_target_);
			}
			break;
		}
		default: {
			break;
		}
		}
	}
}

void MicroAgentBot::OnUnitDestroyed(const Unit* unit) {
	if (unit == targeted_zergling_) {
		Point2D mp, zp;
		if (!GetPosition(UNIT_TYPEID::TERRAN_MARINE, Unit::Alliance::Self, mp)) {
			return;
		}

		if (!GetPosition(UNIT_TYPEID::ZERG_ZERGLING, Unit::Alliance::Enemy, zp)) {
			return;
		}

		Vector2D diff = mp - zp;
		Normalize2D(diff);

		targeted_zergling_ = 0;
		move_back_ = true;
		backup_start_ = mp;
		backup_target_ = mp + diff * 3.0f;
	}
}

bool MicroAgentBot::GetPosition(UNIT_TYPEID unit_type, Unit::Alliance alliace, Point2D& position) {
	const ObservationInterface* observation = Observation();
	Units units = observation->GetUnits(alliace);

	if (units.empty()) {
		return false;
	}

	position = Point2D(0.0f, 0.0f);
	unsigned int count = 0;

	for (const auto& u : units) {
		if (u->unit_type == unit_type) {
			position += u->pos;
			++count;
		}
	}

	position /= (float)count;

	return true;
}

bool MicroAgentBot::GetNearestZergling(const Point2D& from) {
	const ObservationInterface* observation = Observation();
	Units units = observation->GetUnits(Unit::Alliance::Enemy);

	if (units.empty()) {
		return false;
	}

	float distance = std::numeric_limits<float>::max();
	for (const auto& u : units) {
		if (u->unit_type == UNIT_TYPEID::ZERG_ZERGLING) {
			float d = DistanceSquared2D(u->pos, from);
			if (d < distance) {
				distance = d;
				targeted_zergling_ = u;
			}
		}
	}

	return true;
}

