//Register UF07 "MOP_UF07" MOP,ConOP

#ifndef UF7_H
#define UF7_H


#include "UF.h"

namespace OFEC {
	class UF07 final : public UF {
	public:
		UF07(const ParamMap &v);
		UF07(const std::string &name, size_t size_var);
		~UF07() {}
		void initialize();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif
