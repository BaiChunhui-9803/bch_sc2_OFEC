//Register WFG9 "WFG9" MOP,ConOP

#ifndef WFG9_H
#define WFG9_H

#include "WFG.h"

namespace OFEC {
	class WFG9 final : public WFG {
	public:
		WFG9(const ParamMap &v);
		WFG9(const std::string &name, size_t size_var, size_t size_obj);
		~WFG9() {}
		void initialize();
		std::vector<Real> t1(const std::vector<Real> &y);
		std::vector<Real> t2(const std::vector<Real> &y);
		std::vector<Real> t3(const std::vector<Real> &y);
		std::vector<Real> shape(const std::vector<Real> &y);
	};
}


#endif
