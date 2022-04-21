#include "include_problem.h"
#include "custom_method.h"
#include "../utility/factory.h"

namespace OFEC {
	void registerProblem() {
		REGISTER(Problem, DTLZ1, "MOP_DTLZ1", std::set<ProTag>({ ProTag::ConOP, ProTag::MOP }));
		REGISTER(Problem, DTLZ2, "MOP_DTLZ2", std::set<ProTag>({ ProTag::ConOP, ProTag::MOP }));
	}
}
