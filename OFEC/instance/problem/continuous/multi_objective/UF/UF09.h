//Register UF09 "MOP_UF09" MOP,ConOP

#ifndef UF9_H
#define UF9_H


#include "UF.h"

namespace OFEC {
	class UF09 final : public UF {
	public:
		UF09(const ParamMap &v);
		UF09(const std::string &name, size_t size_var);
		~UF09() {}
		void initialize();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif
