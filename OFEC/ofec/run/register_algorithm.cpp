#include "include_algorithm.h"
#include "custom_method.h"
#include "../utility/factory.h"

namespace OFEC {
	void registerAlgorithm() {
		REGISTER(Algorithm, MOEAD_DE, "MOEA/D-DE", std::set<ProTag>({ ProTag::ConOP, ProTag::MOP }));
		REGISTER(Algorithm, NSGAII_DE, "NSGAII-DE", std::set<ProTag>({ ProTag::ConOP, ProTag::MOP }));
	}
}
