//Register moving_peak "Moving-Peaks" ConOP,DOP,SOP

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
* J. Branke, Memory enhanced evolutionary algorithms for changing optimization
* problems, Proc. of the 1999 Congr. on Evol. Comput, pp. 1875-1882, 1999.
*
*********************************************************************************/

// Created: 16 May 2013
// Last modified: 1 Agust 2018
#ifndef MOVINGPEAK_H
#define MOVINGPEAK_H
#include"../uncertianty_continuous.h"
//#include "../dynamic_continuous.h"
namespace OFEC {

	//! noted the noisy in moving peak means the noisy from variable
	class  moving_peak final : public UncertaintyContinuous {
	private:
		int m_F;
		Real m_vlength; /* distance by which the peaks are moved, severity */

		/* lambda determines whether there is a direction of the movement, or whether
		they are totally random. For lambda = 1.0 each move has the same direction,
		while for lambda = 0.0, each move has a random direction */
		Real m_lambda;
		Real  m_standardHeight;
		/* width chosen randomly when standardwidth = 0.0 */
		Real  m_standardWidth;
		std::vector<Real> m_shift;
		std::vector<std::vector<Real>>  m_pre_movement;/* to store every peak's previous movement */

	private:
		/* the following basis functions are provided :*/
		Real constant_basis_func(const Real *gen);
		Real five_peak_basis_func(const Real *gen);
		/* the following peak functions are provided: */
		Real peak_function1(const Real *gen, int peak_number);
		Real peakFunctionCone(const Real *gen, const int &peak_number);
		Real peak_function_hilly(const Real *gen, int peak_number);
		Real peak_function_twin(const Real  *gen, int peak_number);
		Real select_function(const Real  *gen, const int &peak_number);
		Real dummy_eval(const Real *gen);
	protected:
		void calculate_current_peak(const Real *gen);
		void change_random();
		void change_num_peak();
		void copy(const Problem & rP);
		void set_severity();
		void updateParameters();

		virtual void resetPro();
		
	public:
		moving_peak(const std::string &name,int rDimNumber, int rNumPeaks, Real rChangingRatio, int fre, Real vlength, bool rFlagDimChange = false,\
			 bool rFlagNumPeakChange = false, int peakNumChangeMode = 1, bool flagNoise = false,  bool flagTimelinkage = false);
		moving_peak(const ParamMap &v);

		void change_stepsize_random();
		void change_stepsize_linear();
		int get_right_peak();
		void set_vlength(Real s);
		Real get_vlength();
		EvalTag evaluateObjective(Real* x, std::vector<Real>& obj);
		void initialize();
	};

}
#endif // MOVINGPEAK_H
