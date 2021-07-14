#include "./adaptor_cont.h"

namespace OFEC {
	adaptor_cont::adaptor_cont(Real alpha, size_t num_dim, size_t size_pop) : adaptor<individual<>>(alpha), m_proba(num_dim, std::vector<Infor>(m_num)), m_acc_proba(num_dim, std::vector<Real>(m_num)), \
		m_num(100), m_limit(num_dim) {
		for (auto& i : m_limit) i.as.resize(size_pop);
		m_pos.resize(size_pop);
		for (auto& i : m_pos) i.resize(num_dim);
	}

	void adaptor_cont::update_probability(std::vector<std::unique_ptr<individual<>>>& pop, const std::vector<Real>& weight, const std::vector<int>* index) {
		int popsize = pop.size();
		for (auto& j : m_proba) for (auto& i : j) {
			i.val = 0;
			i.idx.clear();
		}
		for (size_t j = 0; j < m_limit.size(); ++j) {
			m_limit[j].boundary.first = CONTINUOUS_CAST->range(j).second;
			m_limit[j].boundary.second = CONTINUOUS_CAST->range(j).first;
		}
		if (index == nullptr) {
			for (int j = 0; j < m_proba.size(); ++j) {
				for (int i = 0; i < popsize; ++i) {
					if (m_limit[j].boundary.first > pop[i]->variable()[j])m_limit[j].boundary.first = pop[i]->variable()[j];
					if (m_limit[j].boundary.second < pop[i]->variable()[j])m_limit[j].boundary.second = pop[i]->variable()[j];
				}
			}
		}
		else {
			for (int j = 0; j < m_proba.size(); ++j) {
				for (auto i : (*index)) {
					if (m_limit[j].boundary.first > pop[i]->variable()[j])m_limit[j].boundary.first = pop[i]->variable()[j];
					if (m_limit[j].boundary.second < pop[i]->variable()[j])m_limit[j].boundary.second = pop[i]->variable()[j];
				}
			}
		}
		if (index == nullptr) {
			for (int i = 0; i < m_proba.size(); ++i) {
				m_limit[i].range = m_limit[i].boundary.second - m_limit[i].boundary.first;
				m_limit[i].step = m_limit[i].range / m_num;
				if (m_limit[i].step > 0) {
					for (int j = 0; j < popsize; ++j) {
						if (!pop[j]->is_active()) continue;
						int reg_idx = (pop[j]->variable()[i] - m_limit[i].boundary.first) / m_limit[i].step;
						if (reg_idx == m_num) reg_idx--;
						m_proba[i][reg_idx].val += weight[j];
						m_proba[i][reg_idx].idx.push_back(j);

						m_pos[j][i] = reg_idx;
					}
				}
				else {
					for (int j = 0; j < popsize; ++j) {
						if (!pop[j]->is_active()) continue;
						m_proba[i][0].val += weight[j];
						m_proba[i][0].idx.push_back(j);

						m_pos[j][i] = 0;
					}
				}
			}
		}
		else {
			for (int i = 0; i < m_proba.size(); ++i) {
				m_limit[i].range = m_limit[i].boundary.second - m_limit[i].boundary.first;
				m_limit[i].step = m_limit[i].range / m_num;
				if (m_limit[i].step > 0) {
					for (auto j : (*index)) {
						if (!pop[j]->is_active()) continue;
						int reg_idx = (pop[j]->variable()[i] - m_limit[i].boundary.first) / m_limit[i].step;
						if (reg_idx == m_num) reg_idx--;
						m_proba[i][reg_idx].val += weight[j];
						m_proba[i][reg_idx].idx.push_back(j);

						m_pos[j][i] = reg_idx;
					}
				}
				else {
					for (auto j : (*index)) {
						if (!pop[j]->is_active()) continue;
						m_proba[i][0].val += weight[j];
						m_proba[i][0].idx.push_back(j);

						m_pos[j][i] = 0;
					}
				}
			}
		}
		accumlate_probability();
	}

	void adaptor_cont::accumlate_probability() {
		for (auto i = 0; i < m_acc_proba.size(); ++i) {
			for (auto j = 0; j < m_acc_proba[i].size(); ++j) {
				m_acc_proba[i][j] = m_proba[i][j].val;
				if (j > 0) m_acc_proba[i][j] += m_acc_proba[i][j - 1];
			}
		}
	}

	void adaptor_cont::create_solution(std::vector<std::unique_ptr<individual<>>>& pop, std::vector<individual<>>& offspring) {
		Real p;
		for (int i = 0; i < pop.size(); i++) {
			p = global::ms_global->m_uniform[caller::Algorithm]->next();
			if (p <= m_alpha)
				local_search(i, pop, offspring);
			else
				global_search(i, pop, offspring);
		}
	}

	void adaptor_cont::local_search(size_t i, std::vector<std::unique_ptr<individual<>>>& pop, std::vector<individual<>>& offspring) {
		Real center = 0, var = 0, x;
		const auto& range = CONTINUOUS_CAST->range();
		for (int j = 0; j < offspring[i].variable().size(); j++) {
			center = pop[i]->variable()[j];
			var = m_limit[j].as[i];
			if (center < range[j].limit.first) center = range[j].limit.first;
			else if (center > range[j].limit.second) center = range[j].limit.second;
			x = center + var * global::ms_global->m_cauchy[caller::Algorithm]->next();
			if (x < range[j].limit.first) {
				x = (center + range[j].limit.first) / 2;
			}
			else if (x > range[j].limit.second) {
				x = (center + range[j].limit.second) / 2;
			}
			offspring[i].variable()[j] = x;
		}
	}

	void adaptor_cont::global_search(size_t i, std::vector<std::unique_ptr<individual<>>>& pop, std::vector<individual<>>& offspring) {
		Real center, var, x, p;
		const auto& range = CONTINUOUS_CAST->range();
		for (int j = 0; j < offspring[i].variable().size(); j++) {
			center = 0, var = 0;
			std::vector<Real> pro(m_num), acc_pro(m_num);
			for (int k = 0; k < m_num; ++k) {
				pro[k] = m_proba[j][k].val * exp(-fabs(m_pos[i][j] - k) / m_num);
				acc_pro[k] = pro[k];
				if (k > 0) acc_pro[k] += acc_pro[k - 1];
			}
			p = global::ms_global->m_uniform[caller::Algorithm]->next() * acc_pro[m_num - 1];
			int idex = lower_bound(acc_pro.begin(), acc_pro.end(), p) - acc_pro.begin();
			if (m_proba[j][idex].idx.size() > 1) {
				for (auto k = 0; k < m_proba[j][idex].idx.size(); ++k) {
					center += pop[m_proba[j][idex].idx[k]]->variable()[j];
				}
				center /= m_proba[j][idex].idx.size();
				for (auto k = 0; k < m_proba[j][idex].idx.size(); ++k) {
					var += (pop[m_proba[j][idex].idx[k]]->variable()[j] - center) * (pop[m_proba[j][idex].idx[k]]->variable()[j] - center);
				}
				var = sqrt(var / m_proba[j][idex].idx.size());
			}
			else {
				center = pop[i]->variable()[j];
				var = m_limit[j].as[i];
			}
			if (center < range[j].limit.first) center = range[j].limit.first;
			else if (center > range[j].limit.second) center = range[j].limit.second;
			x = center + var * global::ms_global->m_cauchy[caller::Algorithm]->next();
			if (x < range[j].limit.first) {
				x = (center + range[j].limit.first) / 2;
			}
			else if (x > range[j].limit.second) {
				x = (center + range[j].limit.second) / 2;
			}
			offspring[i].variable()[j] = x;
		}
	}

	EvalTag adaptor_cont::update_solution(std::vector<std::unique_ptr<individual<>>>& pop, std::vector<individual<>>& offspring, int& num_improve) {
		EvalTag rf = EvalTag::Normal;
		int size_pop = pop.size();
		num_improve = 0;
		for (int i = 0; i < size_pop; i++) {
			if (!pop[i]->is_active()) continue;
			rf = offspring[i].evaluate();
			if (offspring[i].dominate(*pop[i])) {
				*pop[i] = offspring[i];
				pop[i]->set_improved_flag(true);
				num_improve++;
			}
			else {
				pop[i]->set_improved_flag(false);
			}
			if (rf == EvalTag::Terminate) break;
		}
		return rf;
	}

	void adaptor_cont::update_step(std::vector<std::unique_ptr<individual<>>>& pop) {
		int popsize = pop.size();
		int numDim = m_proba.size();
		std::vector<Real*> loc(popsize);
		std::vector<int> idx(popsize);
		for (int d = 0; d < numDim; ++d) {
			for (int i = 0; i < popsize; i++)
			{
				loc[i] = &pop[i]->variable()[d];
			}
			merge_sort(loc, popsize, idx);
			Real lb = CONTINUOUS_CAST->range(d).first, ub = CONTINUOUS_CAST->range(d).second;

			for (int i = 0; i < popsize; i++)
			{
				if (i == 0) { //lower boundary
					m_limit[d].as[idx[i]] = *loc[idx[i + 1]] - *loc[idx[i]];
					//m_limit[d].as[idx[i]] = (*loc[idx[i + 1]] - lb) / 2;
				}
				else if (i == popsize - 1) { //upper boundary
					m_limit[d].as[idx[i]] = *loc[idx[i]] - *loc[idx[i - 1]];
					//m_limit[d].as[idx[i]] = (ub - *loc[idx[i - 1]]) / 2;
				}
				else {
					Real d1 = *loc[idx[i + 1]] - *loc[idx[i]], d2 = *loc[idx[i]] - *loc[idx[i - 1]];
					m_limit[d].as[idx[i]] = (d1 + d2) / 2;//d1 < d2 ? d1 : d2;
				}
			}
		}
	}
}