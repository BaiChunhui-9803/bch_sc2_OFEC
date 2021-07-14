//Register UF06 "MOP_UF06" MOP,ConOP

#ifndef UF6_H
#define UF6_H


#include "UF.h"

namespace OFEC {
	class UF06 final : public UF {
	public:
		UF06(const ParamMap &v);
		UF06(const std::string &name, size_t size_var);
		~UF06() {}
		void initialize();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif

