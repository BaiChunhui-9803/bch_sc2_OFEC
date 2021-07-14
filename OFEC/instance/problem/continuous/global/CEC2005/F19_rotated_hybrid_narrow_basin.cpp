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

#include "F19_rotated_hybrid_narrow_basin.h"
#include "../classical/sphere.h"
#include "../classical/ackley.h"
#include "../classical/griewank.h"
#include "../classical/rastrigin.h"
#include "../classical/weierstrass.h"
#include "../../../../../core/instance_manager.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		void RotatedHybridNarrowBasin::initialize_() {
			Composition::initialize_();
			auto &v = GET_PARAM(m_id_param);
			resizeVariable(std::get<int>(v.at("number of variables")));
			setDomain(-5., 5.);
			resizeObjective(1);
			m_opt_mode[0] = OptMode::Minimize;
			m_objective_accuracy = 1.0e-8;
			m_variable_niche_radius = 1. * m_num_vars;
			m_height_normalize_severity = 2000.;

			setFunction();
			for (auto &i : m_function) 
				i->setOriginalGlobalOpt();
			loadRotation("/instance/problem/continuous/global/CEC2005/data/");
			computeFmax();
			loadTranslation("/instance/problem/continuous/global/CEC2005/data/");  //data path
			for (size_t j = 0; j < m_num_vars; ++j)
				m_function[9]->translation()[j] = 0;
			for (auto &i : m_function)
				i->setGlobalOpt(i->translation().data());
			m_optima.appendVar(VarVec<Real>(m_function[0]->translation()));
			m_optima.setVariableGiven(true);
			std::vector<Real> objs(1);
			evaluateObjective(m_function[0]->translation().data(), objs);
			m_optima.appendObj(objs);
			m_optima.setObjectiveGiven(true);	
		}

		void RotatedHybridNarrowBasin::setFunction() {
			basic_func f(5);
			f[0] = &create_function<Ackley>;
			f[1] = &create_function<Rastrigin>;
			f[2] = &create_function<Sphere>;
			f[3] = &create_function<Weierstrass>;
			f[4] = &create_function<Griewank>;

			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<Function*>(f[i / 2]());
				m_function[i]->setIdRnd(m_id_rnd);
				m_function[i]->setIdParam(m_id_param);
				m_function[i]->initialize();
				m_function[i]->setBias(0);
			}

			m_function[0]->setConditionNumber(2); m_function[1]->setConditionNumber(3);
			m_function[2]->setConditionNumber(2); m_function[3]->setConditionNumber(3);
			m_function[4]->setConditionNumber(2); m_function[5]->setConditionNumber(3);
			m_function[6]->setConditionNumber(20); m_function[7]->setConditionNumber(30);
			m_function[8]->setConditionNumber(200); m_function[9]->setConditionNumber(300);

			for (int i = 0; i < m_num_function; i++) {
				m_height[i] = 100 * i;
			}

			m_function[0]->setDomain(-32, 32);   m_function[1]->setDomain(-32, 32);
			m_function[2]->setDomain(-5, 5);     m_function[3]->setDomain(-5, 5);
			m_function[4]->setDomain(-100, 100); m_function[5]->setDomain(-100, 100);
			m_function[6]->setDomain(-0.5, 0.5); m_function[7]->setDomain(-0.5, 0.5);
			m_function[8]->setDomain(-60, 60); m_function[9]->setDomain(-60, 60);

			m_stretch_severity[0] = 0.1*5. / 32; m_stretch_severity[1] = 5. / 32;
			m_stretch_severity[2] = 2.;		m_stretch_severity[3] = 1.;
			m_stretch_severity[4] = 2 * 5. / 100;  m_stretch_severity[5] = 5. / 100;
			m_stretch_severity[6] = 20.;	m_stretch_severity[7] = 10.;
			m_stretch_severity[8] = 2. * 5 / 60;  m_stretch_severity[9] = 5. / 60;

			m_converge_severity[0] = 0.1; m_converge_severity[1] = 2.;
			m_converge_severity[2] = 1.5;	m_converge_severity[3] = 1.5;
			m_converge_severity[4] = 1.;  m_converge_severity[5] = 1.;
			m_converge_severity[6] = 1.5;	m_converge_severity[7] = 1.5;
			m_converge_severity[8] = 2.;  m_converge_severity[9] = 2.;

			for (int i = 0; i < m_num_function; i++) {
				m_function[i]->setScale(m_stretch_severity[i]);
			}

			//setBias(10.);
		}

		void RotatedHybridNarrowBasin::evaluateObjective(Real *x, std::vector<Real> &obj) {
			Composition::evaluateObjective(x, obj);
			obj[0] += 10.;
		}

		void RotatedHybridNarrowBasin::setTranslation() {
			for (size_t i = 0; i < m_num_function - 1; ++i) 
				for (size_t j = 0; j < m_num_vars; ++j) 
					m_function[i]->translation()[j] = m_domain[j].limit.first + (m_domain[j].limit.second - m_domain[j].limit.first)*(1 - GET_RND(m_id_rnd).uniform.next());
			
			for (size_t j = 0; j < m_num_vars; ++j) 
				m_function[m_num_function - 1]->translation()[j] = 0;
		}
	}
	
}


