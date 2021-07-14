//Register UF10 "MOP_UF10" MOP,ConOP

#ifndef UF10_H
#define UF10_H


#include "UF.h"

namespace OFEC {
	class UF10 final : public UF {
	public:
		UF10(const ParamMap &v);
		UF10(const std::string &name, size_t size_var);
		~UF10() {}
		void initialize();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif

