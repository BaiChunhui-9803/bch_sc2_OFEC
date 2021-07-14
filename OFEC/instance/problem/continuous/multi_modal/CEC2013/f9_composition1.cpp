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

#include "f9_composition1.h"
#include "../../global/classical/griewank.h"
#include "../../global/classical/weierstrass.h"
#include "../../global/classical/sphere.h"
#include "../../../../../core/instance_manager.h"
#include <numeric>

namespace OFEC::CEC2013 {
	void Composition1::initialize_() {
		Continuous::initialize_();
		auto &v = GET_PARAM(m_id_param);
		resizeVariable(std::get<int>(v.at("number of variables")));
		setDomain(-5., 5.);
		resizeObjective(1);
		m_opt_mode[0] = OptMode::Maximize;
		m_objective_accuracy = v.count("objective accuracy") > 0 ? std::get<Real>(v.at("objective accuracy")) : (OFEC::Real)1.e-4;
		m_variable_niche_radius = 0.01;

		m_num_function = 6;
		m_function.resize(m_num_function);
		m_fmax.resize(m_num_function);
		m_stretch_severity.resize(m_num_function);
		m_converge_severity.resize(m_num_function);
		m_height.resize(m_num_function);
		setFunction();
		loadRotation("/instance/problem/Continuous/multi_modal/CEC2013/data/");
		computeFmax();
		loadTranslation("/instance/problem/Continuous/multi_modal/CEC2013/data/");  //data path

		for (auto &i : m_function) {
			i->setGlobalOpt(i->translation().data());
			m_optima.appendVar(VarVec<Real>(i->translation()));
		}
		m_optima.setVariableGiven(true);
		std::vector<Real> temp_obj(1);
		for (auto &i : m_function) {
			evaluateObjective(i->translation().data(), temp_obj);
			m_optima.appendObj(0.);
		}
		m_optima.setObjectiveGiven(true);
	}

	void Composition1::setFunction() {
		basic_func f(3);
		f[0] = &create_function<Griewank>;
		f[1] = &create_function<Weierstrass>;
		f[2] = &create_function<Sphere>;
		
		for (size_t i = 0; i < m_num_function; ++i) {
			m_function[i] = dynamic_cast<Function*>(f[i / 2]());
			m_function[i]->setIdRnd(m_id_rnd);
			m_function[i]->setIdParam(m_id_param);
			m_function[i]->initialize();
			m_function[i]->setBias(0);
		}

		m_stretch_severity[0] = 1; m_stretch_severity[1] = 1;
		m_stretch_severity[2] = 8; m_stretch_severity[3] = 8;
		m_stretch_severity[4] = 1 / 5.; m_stretch_severity[5] = 1 / 5.;

		for (int i = 0; i<m_num_function; i++) {
			m_function[i]->setScale(m_stretch_severity[i]);
		}
			
		for (int i = 0; i<m_num_function; i++) {
			m_converge_severity[i] = 1;
		}

		for (int i = 0; i<m_num_function; i++) {
			m_height[i] = 0;
		}
	}

	void Composition1::evaluateObjective(Real *x, std::vector<Real> &obj) {
		Composition::evaluateObjective(x, obj);
		obj[0] = -obj[0];
	}

	void Composition1::setRotation() {
		for (auto i : m_function)
			i->rotation().identify();
	}
}
