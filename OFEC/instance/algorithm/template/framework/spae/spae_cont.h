#ifndef OFEC_SPAE_CONT_H
#define OFEC_SPAE_CONT_H

#include "hlc.h"
#include "../../../../../core/algorithm/multi_population.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../record/rcr_vec_real.h"
#include "../../../../problem/continuous/global/BBOB/bbob.h"
#include "../../../../../core/instance_manager.h"

#ifdef OFEC_DEMO
#include <core/global_ui.h>
#endif

namespace OFEC {
	class BaseContSPAE : public Algorithm {
	protected:
		std::unique_ptr<HLC> m_hlc;
		size_t m_subpop_size;
		int m_init_num_ssp;
		std::vector<std::pair<Real, Real>> m_base_cov_rate;
		std::vector<std::pair<Real, Real>> m_base_cml_cov_rate;
		std::array<std::pair<Real, Real>, 2> m_base_log_param;

		void initialize_() override {
			Algorithm::initialize_();
			const auto &v = GET_PARAM(m_id_param);
			m_subpop_size = std::get<int>(v.at("subpopulation size"));
			m_init_num_ssp = v.count("initial number of subspaces") > 0 ? std::get<int>(v.at("initial number of subspaces")) : 1;

			m_hlc.reset(new HLC(GET_CONOP(m_id_pro).numVariables(), m_subpop_size));
			m_hlc->initialize(GET_CONOP(m_id_pro).boundary(), GET_PRO(m_id_pro).optMode(0), m_init_num_ssp);
		}

#ifdef OFEC_DEMO
	public:
		HLC &getHLC() { return *m_hlc; }
		const std::vector<std::pair<Real, Real>> &baseCovRate() const { return m_base_cov_rate; }
		const std::vector<std::pair<Real, Real>> &baseCmlCovRate() const { return m_base_cml_cov_rate; }
		const std::array<std::pair<Real, Real>, 2> &baseLogParam() const { return m_base_log_param; }
#endif	
	};

	template<typename PopType>
	class ContSPAE : public BaseContSPAE {
	protected:
		std::string m_pop_type;
		MultiPopulation<PopType> m_multi_pop;
		Real m_rstrct_fctr;
		bool m_init_clstr;

		using IndType = typename PopType::IndType;
		std::vector<size_t> m_affil_bsn;
		std::vector<size_t> m_num_viol_bnd;

	public:
		ContSPAE() = default;
		void record() override;
#ifdef OFEC_DEMO
		void updateBuffer() override;
#endif	
	protected:
		void initialize_() override;
		void run_() override;

		virtual void setPopType() = 0;
		virtual bool isPopConverged(PopType &pop);
		virtual void initSubPop(PopType &pop, size_t id_atr_bsn, HLC &hlc, int id_pro, int id_alg);
		virtual void evolveSubPop(PopType &pop) = 0;
		void calCoverage(PopType &pop, size_t id_atr_bsn);
		void calRuggedness(PopType &pop, size_t id_atr_bsn);

		virtual void restrictPop(PopType &pop, size_t id_atr_bsn);
		virtual void initInd(HLC& hlc, int id_pro, int id_alg, size_t id_atr_bsn, size_t id_div, IndType &ind);

	private:
		void loadBaseCovLn();
		void calBaseCovLn();
		void initMultiPop();
	};

	template<typename PopType>
	void ContSPAE<PopType>::loadBaseCovLn() {
		std::string file_name = g_working_dir + "/instance/algorithm/template/framework/spae/data/"
			+ m_pop_type + "(SpS=" + std::to_string(m_subpop_size) + ").hlc";
		std::ifstream infile(file_name);
		if (!infile)
			calBaseCovLn();
		else {
			for (std::string line; std::getline(infile, line);) {
				if (line.find(":") != std::string::npos) {
					auto name = line.substr(0, line.find(':'));
					auto value = line.substr(line.find(':') + 2, line.size() - 1);
					if (name == "Mean of a")
						m_base_log_param[0].first = std::stod(value);
					else if (name == "Std of a")
						m_base_log_param[0].second = std::stod(value);
					else if (name == "Mean of b")
						m_base_log_param[1].first = std::stod(value);
					else if (name == "Std of b")
						m_base_log_param[1].second = std::stod(value);
					else if (name == "Mean and Std of coverage rates") {
						for (std::stringstream line_stream(value); line_stream; ) {
							m_base_cov_rate.resize(m_base_cov_rate.size() + 1);
							line_stream >> m_base_cov_rate.back().first >> m_base_cov_rate.back().second;
						}
						m_base_cov_rate.pop_back();
					}
					else if (name == "Mean and Std of cumulative coverage rates") {
						for (std::stringstream line_stream(value); line_stream; ) {
							m_base_cml_cov_rate.resize(m_base_cml_cov_rate.size() + 1);
							line_stream >> m_base_cml_cov_rate.back().first >> m_base_cml_cov_rate.back().second;
						}
						m_base_cml_cov_rate.pop_back();
					}
				}
			}
		}
	}

	template<typename PopType>
	void ContSPAE<PopType>::calBaseCovLn() {
		std::unique_ptr<Problem> pro(new BBOB_F01);
		ParamMap v;
		v["problem name"] = std::string("BBOB_F01");
		v["number of variables"] = (int)GET_CONOP(m_id_pro).numVariables();
		size_t num_runs = 50;
		int id_param(ADD_PARAM(v));
		int id_pro = ADD_PRO_EXST(id_param, pro, 0.5);
		GET_CONOP(id_pro).initialize();

		std::vector<std::vector<Real>> data_cov_rate;
		std::vector<std::vector<Real>> data_cml_cov_rate;
		std::vector<std::array<Real, 2>> data_log_param;
		size_t max_len = 0;
		for (size_t id_run = 0; id_run < num_runs; id_run++) {
			HLC h(GET_CONOP(id_pro).numVariables(), m_subpop_size);
			h.initialize(GET_CONOP(id_pro).boundary(), GET_CONOP(id_pro).optMode(0), 1);
			PopType p(m_subpop_size, id_pro);
			initSubPop(p, 0, h, id_pro, -1);
			std::vector<const Solution<> *> sub_pop(p.size());
			for (size_t i = 0; i < p.size(); i++)
				sub_pop[i] = &p[i].phenotype();
			h.calCoverage(0, sub_pop);
			while (!isPopConverged(p)) {
				p.evolve(id_pro, -1, m_id_rnd);
				sub_pop.resize(p.size());
				for (size_t i = 0; i < p.size(); i++)
					sub_pop[i] = &p[i].phenotype();
				h.calCoverage(0, sub_pop);
			}
			h.regressCovLines();
			data_cov_rate.push_back(h.infoBsnAtrct(0).coverages);
			data_cml_cov_rate.push_back(h.infoBsnAtrct(0).cuml_covs);
			data_log_param.push_back(h.infoBsnAtrct(0).log_param);
			if (max_len < h.infoBsnAtrct(0).coverages.size())
				max_len = h.infoBsnAtrct(0).coverages.size();
		}
		m_base_cov_rate.resize(max_len);
		for (auto &row : data_cov_rate) {
			while (row.size() < max_len)
				row.push_back(row.back());
		}
		for (size_t j = 0; j < max_len; j++) {
			std::vector<Real> col;
			for (auto &row : data_cov_rate)
				col.push_back(row[j]);
			calMeanAndStd(col, m_base_cov_rate[j].first, m_base_cov_rate[j].second);
		}
		m_base_cml_cov_rate.resize(max_len);
		for (auto &row : data_cml_cov_rate) {
			while (row.size() < max_len)
				row.push_back(row.back());
		}
		for (size_t j = 0; j < max_len; j++) {
			std::vector<Real> col;
			for (auto &row : data_cml_cov_rate)
				col.push_back(row[j]);
			calMeanAndStd(col, m_base_cml_cov_rate[j].first, m_base_cml_cov_rate[j].second);
		}
		for (size_t id_param = 0; id_param < 2; id_param++) {
			std::vector<Real> col;
			for (auto &row : data_log_param)
				col.push_back(row[id_param]);
			calMeanAndStd(col, m_base_log_param[id_param].first, m_base_log_param[id_param].second);
		}

		std::stringstream out_stream;
		out_stream << "Mean of a: " << m_base_log_param[0].first;
		out_stream << "\nStd of a: " << m_base_log_param[0].second;
		out_stream << "\nMean of b: " << m_base_log_param[1].first;
		out_stream << "\nStd of b: " << m_base_log_param[1].second;
		out_stream << "\nMean and Std of coverage rates:";
		for (auto &c : m_base_cov_rate)
			out_stream << " " << c.first << " " << c.second;
		out_stream << "\nMean and Std of cumulative coverage rates:";
		for (auto &c : m_base_cml_cov_rate)
			out_stream << " " << c.first << " " << c.second;

		std::string file_name = g_working_dir + "/instance/algorithm/template/framework/spae/data/"
			+ m_pop_type + "(SpS=" + std::to_string(m_subpop_size) + ").hlc";
		std::ofstream out_file(file_name);
		out_file << out_stream.str();
		out_file.close();

		DEL_PRO(id_pro);
		DEL_PARAM(id_param);
	}

	template<typename PopType>
	void ContSPAE<PopType>::initMultiPop() {
		if (m_init_clstr) {
			size_t num_each_ssp = 5;
			size_t num_objs = GET_PRO(m_id_pro).numObjectives();
			size_t num_cons = GET_PRO(m_id_pro).numConstraints();
			size_t num_vars = GET_CONOP(m_id_pro).numVariables();
			Solution<> temp_sol(num_objs, num_cons, num_vars);
			for (size_t i = 0; i < m_init_num_ssp; i++) {
				for (size_t k = 0; k < num_each_ssp; k++) {
					m_hlc->randomSmpl(i, m_id_rnd, temp_sol.variable().vect());
					temp_sol.evaluate(m_id_pro, m_id_alg);
					m_hlc->inputSample(temp_sol);
				}
			}
			m_hlc->clustering();
		}
#ifdef OFEC_DEMO
		updateBuffer();
#endif
		for (size_t id_bsn = 0; id_bsn < m_hlc->numBsnsAtrct(); id_bsn++) {
			PopType pop(m_subpop_size, m_id_pro);
			size_t id_pop = m_multi_pop.size();
			m_multi_pop.append(std::move(pop));
			initSubPop(m_multi_pop[id_pop], id_bsn, *m_hlc, m_id_pro, m_id_alg);
			calCoverage(m_multi_pop[id_pop], id_bsn);
			m_multi_pop[id_pop].setActive(true);
			m_affil_bsn.push_back(id_bsn);
		}

#ifdef OFEC_DEMO
		updateBuffer();
#endif
	}

	template<typename PopType>
	void ContSPAE<PopType>::initialize_() {
		BaseContSPAE::initialize_();
		const auto &v = GET_PARAM(m_id_param);
		m_rstrct_fctr = v.count("restriction factor") > 0 ? std::get<Real>(v.at("restriction factor")) : 1;
		m_init_clstr = v.count("initial clustering") > 0 ? std::get<bool>(v.at("initial clustering")) : false;
		if (ID_RCR_VALID(m_id_rcr))
			m_keep_candidates_updated = true;
	}

	template<typename PopType>
	void ContSPAE<PopType>::record() {
		std::vector<Real> entry;
		entry.push_back(m_effective_eval);
		if (GET_PRO(m_id_pro).hasTag(ProTag::MMOP)) {
			size_t num_optima_found = GET_PRO(m_id_pro).numOptimaFound(m_candidates);
			size_t num_optima = GET_CONOP(m_id_pro).getOptima().numberObjectives();
			entry.push_back(num_optima_found);
			entry.push_back(num_optima_found == num_optima ? 1 : 0);
		}
		else
			entry.push_back(m_candidates.front()->objectiveDistance(GET_CONOP(m_id_pro).getOptima().objective(0)));
		dynamic_cast<RecordVecReal &>(GET_RCR(m_id_rcr)).record(m_id_alg, entry);
	}

#ifdef OFEC_DEMO
	template<typename PopType>
	void ContSPAE<PopType>::updateBuffer() {
		m_solution.clear();
		m_solution.resize(m_multi_pop.size());
		for (size_t k = 0; k < m_multi_pop.size(); ++k) {
			for (size_t i = 0; i < m_multi_pop[k].size(); ++i) {
				m_solution[k].push_back(&m_multi_pop[k][i].phenotype());
			}
		}
		Demo::g_buffer->appendAlgBuffer(m_id_alg);
	}
#endif

	template<typename PopType>
	bool ContSPAE<PopType>::isPopConverged(PopType &pop) {
		pop.updateBest(m_id_pro);
		if (pop.iteration() - pop.timeBestUpdated() > 20)
			return true;
		else
			return false;
		//size_t id_worst = 0;
		//for (size_t i = 1; i < pop.size(); i++)	{
		//	if (pop[id_worst].phenotype().dominate(pop[i].phenotype(), m_id_pro))
		//		id_worst = i;
		//}
		//auto max_obj_dif = pop.best().front()->objectiveDistance(pop[id_worst].phenotype());
		//if (max_obj_dif < 1e-6)
		//	return true;
		//else
		//	return false;
	}

	template<typename PopType>
	void ContSPAE<PopType>::initSubPop(PopType &pop, size_t id_atr_bsn, HLC &hlc, int id_pro, int id_alg) {
		for (size_t i = 0; i < pop.size(); i++) {
			initInd(hlc, id_pro, id_alg, id_atr_bsn, i, pop[i]);
			//m_hlc->randomSmpl(id_atr_bsn, i, m_id_rnd, pop[i].variable().vect());
			//pop[i].evaluate(m_id_pro, m_id_alg);
			//m_hlc->inputSample(pop[i].solut());
		}
	}

	template<typename PopType>
	void ContSPAE<PopType>::run_() {
		setPopType();
		loadBaseCovLn();
		initMultiPop();
		int iter = 0;
		while (!terminating()) {
			std::vector<int> iters_for_convergence(m_multi_pop.size(), 0);
			while (!terminating() && m_multi_pop.isActive()) {
				for (size_t k = 0; k < m_multi_pop.size(); k++) {
					if (m_multi_pop[k].isActive()) {
						evolveSubPop(m_multi_pop[k]);
						restrictPop(m_multi_pop[k], m_affil_bsn[k]);
						iters_for_convergence[k]++;
						if (isPopConverged(m_multi_pop[k]))
							m_multi_pop[k].setActive(false);
						calCoverage(m_multi_pop[k], m_affil_bsn[k]);
					}
				}
#ifdef OFEC_DEMO   
				updateBuffer();
#endif
				iter++;
			}
			for (size_t k = 0; k < m_multi_pop.size(); ++k)
				m_hlc->calRuggedness(m_affil_bsn[k], m_id_rnd, m_id_pro);
#ifdef OFEC_DEMO   
			updateBuffer();
#endif
			m_hlc->regressCovLines();
#ifdef OFEC_DEMO  
			m_hlc->setLogParamReady(true);
			updateBuffer();
			m_hlc->setLogParamReady(false);
#endif
			for (size_t k = 0; k < m_multi_pop.size(); ++k) {
				if (m_hlc->infoBsnAtrct(m_affil_bsn[k]).ruggedness > 0.5)
					continue;
				size_t num_pre = m_hlc->infoBsnAtrct(m_affil_bsn[k]).subspaces.size();
				auto times = log(m_base_log_param[1].first) / log(m_hlc->infoBsnAtrct(m_affil_bsn[k]).log_param[1]);
				if (times > m_multi_pop[k].size())
					times = m_multi_pop[k].size();
					size_t num_new = ceil(num_pre * times);
				m_hlc->splitBasin(m_affil_bsn[k], num_new);
			}
#ifdef OFEC_DEMO   
			updateBuffer();
#endif
			m_hlc->interpolateMissing();
#ifdef OFEC_DEMO   
			updateBuffer();
#endif
			m_hlc->clustering();
#ifdef OFEC_DEMO
			if (Demo::g_term_alg) return;
			updateBuffer();
#endif
			m_hlc->updatePotential();
			m_multi_pop.clear();
			m_affil_bsn.clear();
			for (size_t id_bsn = 0; id_bsn < m_hlc->numBsnsAtrct(); id_bsn++) {
				if (GET_RND(m_id_rnd).uniform.next() < m_hlc->infoBsnAtrct(id_bsn).potential) {
					PopType pop(m_subpop_size, m_id_pro);
					size_t id_pop = m_multi_pop.size();
					m_multi_pop.append(std::move(pop));
					initSubPop(m_multi_pop[id_pop], id_bsn, *m_hlc, m_id_pro, m_id_alg);
					calCoverage(m_multi_pop[id_pop], id_bsn);
					m_multi_pop[id_pop].setActive(true);
					m_affil_bsn.push_back(id_bsn);
				}
			}
			m_num_viol_bnd.assign(m_hlc->numBsnsAtrct(), 0);
			iter++;
#ifdef OFEC_DEMO
			updateBuffer();
#endif
		}
	}

	template<typename PopType>
	void ContSPAE<PopType>::calCoverage(PopType &pop, size_t id_atr_bsn) {
		std::vector<const Solution<> *> sub_pop;
		for (size_t i = 0; i < pop.size(); i++) {
			bool same = false;
			for (auto ind : sub_pop) {
				if (ind->variableDistance(pop[i].phenotype(), m_id_pro) < 1e-2) {
					same = true;
					break;
				}
			}
			if (!same)
				sub_pop.push_back(&pop[i].phenotype());
		}
		m_hlc->calCoverage(id_atr_bsn, sub_pop);
	}

	template<typename PopType>
	void ContSPAE<PopType>::calRuggedness(PopType &pop, size_t id_atr_bsn) {
		std::vector<const Solution<> *> sub_pop;
		for (size_t i = 0; i < pop.size(); i++)
			sub_pop.push_back(&pop[i].phenotype());
		m_hlc->calRuggedness(id_atr_bsn, sub_pop);
	}

	template<typename PopType>
	void ContSPAE<PopType>::restrictPop(PopType &pop, size_t id_atr_bsn) {
		size_t affiliatedSubspace, id_div;
		for (size_t i = 0; i < pop.size(); i++) {
			affiliatedSubspace = m_hlc->subspaceTree().getRegionIdx(pop[i].phenotype().variable().vect());
			if (m_hlc->infoSubspace(affiliatedSubspace).id_bsn_atr != id_atr_bsn) {
				//if (GET_RND(m_id_rnd).uniform.next() < pow(1. / (m_num_viol_bnd[id_atr_bsn] + 1), m_rstrct_fctr)) {
				//	initInd(id_atr_bsn, pop[i]);
				//	m_num_viol_bnd[id_atr_bsn]++;
				//}	
				if (GET_RND(m_id_rnd).uniform.next() < pow(1. / pop.iteration(), m_rstrct_fctr)) {
					id_div = GET_RND(m_id_rnd).uniform.nextNonStd<size_t>(0, m_subpop_size);
					initInd(*m_hlc, m_id_pro, m_id_alg, id_atr_bsn, id_div, pop[i]);
				}
			}
		}
	}

	template<typename PopType>
	void ContSPAE<PopType>::initInd(HLC &hlc, int id_pro, int id_alg, size_t id_atr_bsn, size_t id_div, IndType &ind) {
		hlc.randomSmpl(id_atr_bsn, id_div, m_id_rnd, ind.variable().vect());
		ind.evaluate(id_pro, id_alg);
		hlc.inputSample(ind.solut());
	}
}

#endif // !OFEC_SPAE_CONT_H
