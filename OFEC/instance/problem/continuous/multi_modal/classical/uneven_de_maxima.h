//Register UnevenDeMaxima "Classic_uneven_de_maxima" MMOP,ConOP,SOP

#ifndef OFEC_UNEVEN_DE_MAXIMA_H
#define OFEC_UNEVEN_DE_MAXIMA_H

#include "../../../../../core/problem/continuous/function.h"
#include "../metrics_mmop.h"

namespace OFEC {
	//Beasley's F4 function
	class UnevenDeMaxima : public Function, public MetricsMMOP { 	
	protected:
		void initialize_() override;
		void evaluateObjective(Real *x, std::vector<Real>& obj) override;
	};	
}
#endif // !OFEC_UNEVEN_DE_MAXIMA_H