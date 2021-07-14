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

#include "F16_rotated_hybrid_composition_F15.h"
#include "../classical/sphere.h"
#include "../classical/ackley.h"
#include "../classical/griewank.h"
#include "../classical/rastrigin.h"
#include "../classical/weierstrass.h"
#include "../../../../../core/instance_manager.h"
#include <numeric>

namespace OFEC {
	namespace CEC2005 {
		void RotatedHybridComposition::initialize_() {
			Composition::initialize_();
			auto &v = GET_PARAM(m_id_param);
			resizeVariable(std::get<int>(v.at("number of variables")));
			setDomain(-5., 5.);
			resizeObjective(1);
			m_opt_mode[0] = OptMode::Minimize;
			m_height_normalize_severity = 2000.;
			m_objective_accuracy = 1.0e-8;
			m_variable_niche_radius = 1. * m_num_vars;

			setFunction();
			for (auto &i : m_function)
				i->setOriginalGlobalOpt();
			loadRotation("/instance/problem/continuous/global/classical/CEC2005/data/");
			computeFmax();
			loadTranslation("/instance/problem/continuous/global/classical/CEC2005/data/");  //data path
			
			for (auto &i : m_function) {
				i->setGlobalOpt(i->translation().data());
			}
			m_optima.appendVar(VarVec<Real>(m_function[0]->translation()));
			m_optima.setVariableGiven(true);

			std::vector<Real> objs(1);
			evaluateObjective(m_function[0]->translation().data(), objs);
			m_optima.appendObj(objs);
			m_optima.setObjectiveGiven(true);
		}

		void RotatedHybridComposition::evaluateObjective(Real *x, std::vector<Real> &obj) {
			Composition::evaluateObjective(x, obj);
			obj[0] += 120.; // add m_bias
		}


		void RotatedHybridComposition::setFunction() {
			basic_func f(5);
			f[0] = &create_function<Rastrigin>;
			f[1] = &create_function<Weierstrass>;
			f[2] = &create_function<Griewank>;
			f[3] = &create_function<Ackley>;
			f[4] = &create_function<Sphere>;

			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<Function*>(f[i / 2]());
				m_function[i]->setIdRnd(m_id_rnd);
				m_function[i]->setIdParam(m_id_param);
				m_function[i]->initialize();
				m_function[i]->setBias(0);
			}

			for (auto &i : m_function)
				i->setConditionNumber(2.);

			for (int i = 0; i < m_num_function; i++) {
				m_height[i] = 100 * i;
				m_converge_severity[i] = 1.;
			}

			m_function[0]->setDomain(-5, 5);     m_function[1]->setDomain(-5, 5);
			m_function[2]->setDomain(-0.5, 0.5); m_function[3]->setDomain(-0.5, 0.5);
			m_function[4]->setDomain(-60, 60); m_function[5]->setDomain(-60, 60);
			m_function[6]->setDomain(-32, 32);   m_function[7]->setDomain(-32, 32);
			m_function[8]->setDomain(-100, 100); m_function[9]->setDomain(-100, 100);

			m_stretch_severity[0] = 1.;		m_stretch_severity[1] = 1.;
			m_stretch_severity[2] = 10.;		m_stretch_severity[3] = 10.;
			m_stretch_severity[4] = 5. / 60;  m_stretch_severity[5] = 5. / 60;
			m_stretch_severity[6] = 5. / 32;	m_stretch_severity[7] = 5. / 32;
			m_stretch_severity[8] = 5. / 100;  m_stretch_severity[9] = 5. / 100;

			for (int i = 0; i < m_num_function; i++) {
				m_function[i]->setScale(m_stretch_severity[i]);
			}

			//setBias(120.);
		}
	}
}


