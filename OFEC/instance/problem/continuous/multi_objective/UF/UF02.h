//Register UF02 "MOP_UF02" MOP,ConOP

#ifndef UF2_H
#define UF2_H


#include "UF.h"

namespace OFEC {
	class UF02 final : public UF {
	public:
		UF02(const ParamMap &v);
		UF02(const std::string &name, size_t size_var);
		~UF02() {}
		void initialize();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif
