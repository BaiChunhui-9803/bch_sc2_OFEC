#include "modified_vincent.h"

namespace OFEC {

	modified_vincent::modified_vincent(const ParamMap &v) :
		modified_vincent((v.at("problem name")), (v.at("number of variables")), 1) {

		
	}
	modified_vincent::modified_vincent(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {
		
	}

	void modified_vincent::initialize() {
		setDomain(-10, 10);
		setInitialDomain(-10, 10);
		
		m_variable_accuracy = 0.01;
		m_objective_accuracy = 1.e-4;
		m_obj_minmax_monitored = true;
		// 6^Dim gopt 
		size_t num = (int)pow(6, m_num_vars);
		
		std::vector<std::vector<Real>> obj_data(num, std::vector<Real>(1, 0));

		for (auto &i : obj_data) {
			m_original_optima.append(i[0]);
		}
		m_optima = m_original_optima;
		m_initialized = true;
	}
	EvalTag modified_vincent::evaluateObjective(Real *x, std::vector<Real> &obj) {
		if (m_translated)
			translate(x);
		if (m_scaled)
			scale(x);
		if (m_rotated)
			rotate(x);
		if (m_translated)
			translateOrigin(x);
		size_t i;
		obj[0] = 0.0;
		
		for (i = 0; i<m_num_vars; i++)
		{
			x[i] += 4.1112;
			if ((x[i] >= 0.25)&(x[i] <= 10.0))
			{
				obj[0] += -sin(10.0*log(x[i]));
			}
			else if (x[i]<0.25)
			{
				obj[0] += pow(0.25 - x[i], 2.0) - sin(10.0*log(2.5));
			}
			else
			{
				obj[0] += pow(x[i] - 10, 2.0) - sin(10.0*log(10.0));
			}
		}
		obj[0] /= m_num_vars;
		obj[0] += 1.0;
		return EvalTag::Normal;
	}

}