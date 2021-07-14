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
* 1. Changhe Li and Shengxiang Yang. A Generalized Approach to Construct Benchmark Problems for Dynamic Optimization.
* 2008. Berlin, Heidelberg: Springer Berlin Heidelberg.
* 2. Changhe Li, Shengxiang Yang, D. Pelta. Benchmark Generator for CEC'2012 Competition on Evolutionary Computation for
* Dynamic Optimization Problems,Tech. Rep., the School of Computer Science, China University of Geosciences, Wuhan, China, 2012.
*
*********************************************************************************/
// Created: 11 May 2011
// modified: 30 July 2018
// modified: 10 June 2021 by DYY
#ifndef DYNAMICCONTINUOUS_H
#define DYNAMICCONTINUOUS_H

#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../core/problem/uncertainty/dynamic.h"
#include "../../../../core/problem/uncertainty/noisy.h"

namespace OFEC {
#define CAST_UNCERTIANTY_CONT dynamic_cast<UncertaintyContinuous*>(global::ms_global->m_problem.get())

	class UncertaintyContinuous : public Dynamic, public Noisy,public Continuous{
	public:
		enum ChangeType { CT_SmallStep = 0, CT_LargeStep, CT_Random, CT_Recurrent, CT_Chaotic, CT_RecurrentNoisy };
		struct Change {
			ChangeType type;
			int counter;
		};
	protected:
		const static std::vector<std::string> ms_type;

		Change m_change;
		Real m_recurrent_noisy_severity;			// deviation servity from the trajactory of recurrent change
		bool m_synchronize;                 // default=true all dimensions change at the same time

		int m_temp_dimension;                //a temporal variable for dimensional change only

		int m_num_peaks;
		bool m_flag_num_peak_change;                  // flag of the change of the number of peaks
		bool m_dir_num_peak_change;                   // true for increasing the number of peaks, otherwise decreasing the number of peaks
		int m_temp_num_peak;                         // temporal varibal for number of peaks change only


		unsigned m_max_dimension;
		unsigned m_min_dimension;     //should be greater than 1

		int m_max_peaks;
		int m_min_peaks;

		int m_init_peaks, m_init_dimensions;
		Real m_alpha, m_max_alpha;              // to control step severity
		Real m_chaotic_constant;

		// features below added on NOV 22 2012
		int m_mode;		// for the number of peaks change; 1: periodic with fixed step, 2: periodic with random step, 3: chaotic change


		std::vector<VarVec<Real>> m_peak;					    	// positions of local or global optima(local optima in Rotation_DBG,
		std::vector<VarVec<Real>> m_pre_peak;							// global optima of basic function in Composition_DBG)
		std::vector<VarVec<Real>> m_ini_peak;				        // save the initial positions
		std::vector<Real> m_height;							// peak height in Rotation_DBG, height of global optima in Composition_DBG
		std::vector<Real> m_width;                           // weight value of each basic function in Composition_DBG,  peak width in Rotation_DBG

		///TODO preHeight and preWidth not considered in current version
		std::vector<Real> m_pre_height;
		std::vector<Real>m_pre_width;

		Real m_min_height, m_max_height;		// minimum\maximum height of all peaks(local optima) in Rotation_DBG(Composition_DBG)
		std::vector<Real> m_height_severity;
		Real m_min_width, m_max_width;
		std::vector<Real> m_width_severity;

		std::vector<Real> m_fitness;						    	// objective value of each basic funciton in Composition_DBG, peak height in Rotation_DBG
		std::vector<bool> m_flag_global_optima;                      // the index of the global optimal peak

		int m_current_peak;                         // the peak where the best individual is located
		std::vector<bool> m_flag_change;                      // whether peaks change or not
		int m_num_changing_peak;                       // the number of peaks that change
		Real m_ratio_changing_peak = 1.0;                    // the ratio of changing peaks

		int m_num_visable_peak;                      // number of visable peaks, a peak is visable only if no peak is on top of it
		std::vector<int> m_num_tracking;							// accumulated number of peak[i] being tracked
		std::vector<int> m_height_order;
		int m_num_peak_tracked;
		std::vector<bool> m_tracked;
		std::vector<Real> m_time_linkage;
	
	
	public:

		UncertaintyContinuous(const std::string& name,size_t num_peak, size_t num_vars, size_t num_objs = 1, size_t num_cons = 0);
		


		void setType(ChangeType rT);
		void setFlagNumPeaksChange(bool rPC) {
			m_flag_num_peak_change = rPC;
			m_params["Flag number of peaks change"] = m_flag_num_peak_change;

		}
		bool getFlagNumPeaksChange() {
			return m_flag_num_peak_change;
		}
		void setFlagSynchronize(bool rFlag) {
			m_synchronize = rFlag;
		}
		void setRecurrentNoisySeverity(Real rSeverity) {
			m_recurrent_noisy_severity = rSeverity;
		}

		void setAlpha(Real rAlpha) {
			m_alpha = rAlpha;
		};
		void setMaxAlpha(Real rMaxAlpha) {
			m_max_alpha = rMaxAlpha;
		};
		void setChoaticConstant(Real rValue) {
			m_chaotic_constant = rValue;
		}
		ChangeType getType() const {
			return m_change.type;
		};

		bool getFlagSynchronizeChange()const {
			return m_synchronize;
		};

		void setNumPeakChangeMode(int mode);
		int getNumPeakChangeMode();
		int getNumPeak()const {
			return m_num_peaks;
		}

		virtual void change();
		Real getRecurrentNoise(int x, Real min, Real max, Real amplitude, Real angle, Real noisy_severity = 1.);
		Real chaoticStep(Real x, Real min, Real max, Real scale = 1.0);
		bool predictChange(int evalsMore);

		void set_num_change(Real rRatio);
		void set_height_severity(const Real rS);
		void set_width_severity(const Real rS);
		void set_height(const Real* h);
		void set_location(const std::vector<std::vector<Real>>&);
		void set_initial_location(const std::vector<std::vector<Real>>&);
		virtual void set_width(const Real w);
		int get_num_visible_peak();
		bool is_visible(int rIdx);
		bool is_tracked(Real* gen, Real obj);// is any peak tracked for the first time
		int get_num_peak_found();

		//15-07-2013
		bool isGloOptTracked();
		const std::vector<Real>& getNearestPeak(const std::vector<Real>&);

	protected:
		virtual void changeRandom() {};
		virtual void changeSmallStep() {};
		virtual void changeLargeStep() {};
		virtual void changeRecurrent() {};
		virtual void changeChaotic() {};
		virtual void changeRecurrentNoisy() {};

		virtual void changeVarMem();
		virtual void changeNumPeak() {};


	protected:
		EvalTag evaluate_(SolBase& s, bool effective);
		void calculate_global_optima();
		void update_num_change();
		void update_num_visable_peak();
		void add_noise(Real *x);
		void update_time_linkage();
		void move_peak( int idx);
		void copy(const Problem&);
		void updateParameters();
		void update_pre_peak();


	};
}
#endif // DYNAMICCONTINUOUS_H
