//Register WFG8 "WFG8" MOP,ConOP

#ifndef WFG8_H
#define WFG8_H

#include "WFG.h"

namespace OFEC {
	class WFG8 final : public WFG {
	public:
		WFG8(const ParamMap &v);
		WFG8(const std::string &name, size_t size_var, size_t size_obj);
		~WFG8() {}
		void initialize();
		std::vector<Real> t1(const std::vector<Real> &y);
		std::vector<Real> t2(const std::vector<Real> &y);
		std::vector<Real> t3(const std::vector<Real> &y);
		std::vector<Real> shape(const std::vector<Real> &y);
	};
}


#endif
