//Register CEC2005_GOP_F19 "GOP_CEC2005_F19" GOP,ConOP,SOP

/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#ifndef OFEC_F19_ROTATED_HYBRID_NARROW_BASIN_H
#define OFEC_F19_ROTATED_HYBRID_NARROW_BASIN_H

#include "composition.h"
#include "../metrics_gop.h"

namespace OFEC {
	namespace CEC2005 {
		class RotatedHybridNarrowBasin final : public Composition, public MetricsGOP {
		protected:
			void initialize_() override;
			void evaluateObjective(Real *x, std::vector<Real>& obj) override;
			void setFunction();
			void setTranslation();
		};
	}
	using CEC2005_GOP_F19 = CEC2005::RotatedHybridNarrowBasin;
}
#endif // ! OFEC_F19_ROTATED_HYBRID_NARROW_BASIN_H

