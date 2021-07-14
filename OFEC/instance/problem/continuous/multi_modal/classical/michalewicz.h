//Register michalewicz "Classic_michalewicz" MMOP,ConOP,SOP

#ifndef OFEC_MICHALEWICZ_H
#define OFEC_MICHALEWICZ_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	
	class michalewicz : public function
	{
	public:
		michalewicz(const ParamMap &v);
		michalewicz(const std::string &name, size_t size_var, size_t size_obj);
		void initialize();
	protected:
		void evaluateObjective(Real *x, std::vector<Real>& obj) override;
		int m_m;
	private:
	};
	
}
#endif // !OFEC_MICHALEWICZ_H