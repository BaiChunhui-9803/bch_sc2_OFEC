/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* macros used in OFEC
*
*
*********************************************************************************/

#ifndef OFEC_DEFINITION_H
#define OFEC_DEFINITION_H

#include <cmath>

namespace OFEC {
#define OFEC_PI std::acos(-1.0)
#define OFEC_E std::exp(1.0)

#ifdef USING_FLOAT                     
	using Real = float;						// set precision type to float
#else
	using Real = double;				  // set precision type to double
#endif

	enum class Dominance { Equal, Dominant, Dominated, NonDominated, NonComparable };
	enum class OptMode { Minimize, Maximize };
	enum class Violation { Constraint, Boundary, None };
	enum class Constraint{ Inequality, Equality };
	enum class Validation { Ignore, Reinitialize, Remap, SetToBound };
	enum class EvalTag {
		Normal, Terminate, Infeasible, 
		ChangeNextEval, ChangeNonMemory, ChangeObjectiveMemory, ChangeVariableMemory, ChangeObjAndVarMemory
	};
	//SOP: single objective problem
	//MOP: multi-objective problem
	//DOP: dynamic optimization problem
	//MMOP: multi-modal optimization problem
	//GOP: global optimization problem
	//ROOT: robust optimzation problem
	//ConOP: Continuous optimization problem
	//ComOP: combinatorial optimization problem
	//TSP: travelling salesman problem
	//COP: constraint optimization problem
	//VRP: vehicle routing problem
	//TTP: timetabling problem
	//JSP: job shop problem
	//KOP: knapsack optimization problem
	//SAT: boolean satisfiability problem
	//ONEMAX: one max problem
	//QAP: quadratic assignment problem
	//MKP: multi-dimensional knapsack problem
	//EOP: expensive optimization problem
	//LSOP: large scale optimization problem
	enum class ProTag {NullTag,
		SOP, MOP, DOP,DMOP, MMOP, GOP, ROOT, ConOP, ComOP, TSP, COP, VRP, TTP, JSP,
		KOP, SAT, ONEMAX, QAP, MKP, EOP, LSOP, epanet, DVRP, SP
	};
	//for epanet
	enum class InitType { Random, Distance, KMeans, BeVisited };
}
#endif // !OFEC_DEFINITION_H