//Register UF03 "MOP_UF03" MOP,ConOP

#ifndef UF3_H
#define UF3_H


#include "UF.h"

namespace OFEC {
	class UF03 final : public UF {
	public:
		UF03(const ParamMap &v);
		UF03(const std::string &name, size_t size_var);
		~UF03() {}
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif

