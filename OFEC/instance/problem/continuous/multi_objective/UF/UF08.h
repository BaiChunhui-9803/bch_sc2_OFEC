//Register UF08 "MOP_UF08" MOP,ConOP

#ifndef UF8_H
#define UF8_H


#include "UF.h"

namespace OFEC {
	class UF08 final : public UF {
	public:
		UF08(const ParamMap &v);
		UF08(const std::string &name, size_t size_var);
		~UF08() {}
		void initialize();
	private:
		void evaluateObjective(Real *x, std::vector<Real> &obj);
	};
}

#endif
