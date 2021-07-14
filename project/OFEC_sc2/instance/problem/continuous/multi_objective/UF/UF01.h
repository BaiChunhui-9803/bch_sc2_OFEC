//Register UF01 "MOP_UF01" MOP,ConOP

#ifndef UF1_H
#define UF1_H


#include "UF.h"

namespace OFEC {
	class UF01 final : public UF {
	public:
		UF01(const ParamMap &v);
		UF01(const std::string &name, size_t size_var);
		~UF01() {}
		void initialize();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif
