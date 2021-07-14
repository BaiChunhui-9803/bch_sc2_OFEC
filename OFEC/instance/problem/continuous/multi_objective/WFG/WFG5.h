//Register WFG5 "WFG5" MOP,ConOP

#ifndef WFG5_H
#define WFG5_H

#include "WFG.h"

namespace OFEC {
	class WFG5 final : public WFG {
	public:
		WFG5(const ParamMap &v);
		WFG5(const std::string &name, size_t size_var, size_t size_obj);
		~WFG5() {}
		void initialize();
		std::vector<Real> t1(const std::vector<Real> &y);
		std::vector<Real> t2(const std::vector<Real> &y);
		std::vector<Real> shape(const std::vector<Real> &y);
	};
}


#endif
