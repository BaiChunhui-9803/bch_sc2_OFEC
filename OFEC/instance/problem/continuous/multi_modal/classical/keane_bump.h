//Register KeaneBump "Classic_Keane_Bump" MMOP,ConOP,SOP

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
******************************************************************************************
*  Paper: Multimodal Optimization by Means of a Topological Species Conservation Algorithm
*		  IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, VOL.14,NO.6,DECEMBER 2010
*  Paper: Minimization of Keane��s Bump Function by the Repulsive Particle Swarm and the 
*			Differential Evolution Methods, Mishra, SK (2007)
*******************************************************************************************/

#ifndef OFEC_KEANE_BUMP_H
#define OFEC_KEANE_BUMP_H

#include "../metrics_mmop.h"

namespace ofec {	
	class KeaneBump : public MetricsMMOP {
	protected:
		void initialize_();
		void evaluateObjective(Real *x, std::vector<Real> &obj) override;
	};
}
#endif // !OFEC_KEANE_BUMP_H