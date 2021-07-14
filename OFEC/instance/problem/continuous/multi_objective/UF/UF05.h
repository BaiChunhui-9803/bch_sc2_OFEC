//Register UF05 "MOP_UF05" MOP,ConOP

#ifndef UF5_H
#define UF5_H


#include "UF.h"

namespace OFEC {
	class UF05 final : public UF {
	public:
		UF05(const ParamMap &v);
		UF05(const std::string &name, size_t size_var);
		~UF05() {}
		void initialize();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif

