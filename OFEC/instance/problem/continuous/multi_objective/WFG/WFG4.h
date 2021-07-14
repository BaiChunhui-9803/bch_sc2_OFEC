//Register WFG4 "WFG4" MOP,ConOP

#ifndef WFG4_H
#define WFG4_H

#include "WFG.h"

namespace OFEC {
	class WFG4 final : public WFG {
	public:
		WFG4(const ParamMap &v);
		WFG4(const std::string &name, size_t size_var, size_t size_obj);
		~WFG4() {}
		void initialize();
		std::vector<Real> t1(const std::vector<Real> &y);
		std::vector<Real> t2(const std::vector<Real> &y);
		std::vector<Real> shape(const std::vector<Real> &y);
	};
}


#endif
