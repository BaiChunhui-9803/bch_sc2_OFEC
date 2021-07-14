#include"selection_problem.h"
using namespace OFEC;
const int OFEC::SP::wall_type_num = 4;


void OFEC::SP::wall_info::set_position_style(real* center, wall_type* type, real max_radius, real small_loop_radius, real large_loop_inner)
{
	if (center != nullptr)
		m_center = *center;
	if (type != nullptr)
		m_type = *type;

	switch (m_type)
	{
	case wall_type::small_loop: {
		m_inner_radius = 0;
		m_outer_radius = global::ms_global->m_uniform[caller::Problem]->next_non_standard<real>(0, small_loop_radius);
		break;
	}
	case wall_type::large_loop: {
		m_outer_radius = global::ms_global->m_uniform[caller::Problem]->next_non_standard<real>(large_loop_inner, max_radius);
		m_inner_radius = m_outer_radius - large_loop_inner;
		break;
	}

	case wall_type::needle: {
		m_outer_radius = global::ms_global->m_uniform[caller::Problem]->next_non_standard<real>(large_loop_inner + 0.1, max_radius);
		real loop_r = global::ms_global->m_uniform[caller::Problem]->next_non_standard<real>(0, m_outer_radius);
		m_inner_radius = m_outer_radius - loop_r;

		m_from_theta = global::ms_global->m_uniform[caller::Problem]->next_non_standard<real>(0, 2 * OFEC_PI);
		m_to_theta = global::ms_global->m_uniform[caller::Problem]->next_non_standard<real>(0, 2 * OFEC_PI);

		if (m_from_theta > m_to_theta) {
			std::swap(m_from_theta, m_to_theta);
		}
		//	generate_wall_points_needle(m_walls[dim_idx].m_center, m_walls[dim_idx].m_inner_radius, m_walls[dim_idx].m_outer_radius, m_walls[dim_idx].m_from_theta, m_walls[dim_idx].m_to_theta, m_walls[dim_idx].m_numPos, m_points[dim_idx]);
		break;
	}
	default: {
		m_inner_radius = 0;
		m_outer_radius = global::ms_global->m_uniform[caller::Problem]->next_non_standard<real>(small_loop_radius, max_radius);
		break;
	}

	}
}



int OFEC::SP::wall_info::cal_numPos(wall_type type, int num_small_points,
	int num_large_points,
	int num_circle_points,
	int num_needle_points) {

	int numPos(0);
	switch (type)
	{
	case wall_type::small_loop: {
		numPos = global::ms_global->m_uniform[caller::Problem]->next_non_standard<int>(5, num_small_points);
		//	generate_wall_points(m_walls[dim_idx].m_center, m_walls[dim_idx].m_inner_radius, m_walls[dim_idx].m_outer_radius, m_walls[dim_idx].m_numPos, m_points[dim_idx]);
		break;
	}
	case wall_type::large_loop: {
		numPos = global::ms_global->m_uniform[caller::Problem]->next_non_standard<int>(5, num_large_points);
		break;
	}

	case wall_type::needle: {
		numPos = global::ms_global->m_uniform[caller::Problem]->next_non_standard<int>(5, num_needle_points);
		break;
	}
	default: {
		numPos = global::ms_global->m_uniform[caller::Problem]->next_non_standard<int>(5, num_circle_points);
		break;
	}
	}
	return numPos;
}

void OFEC::SP::wall_info::set_numPos(int num_small_points, int num_large_points, int num_circle_points, int num_needle_points)
{

	m_numPos = cal_numPos(m_type, num_small_points, num_large_points, num_circle_points, num_needle_points);
}



void  OFEC::SP::map_info::initialize_wall_type() {
	//m_points.clear();

	//problem::resize(variable_size);
	std::vector<wall_type> random_needle(m_parameter.m_wall_num[static_cast<int>(wall_type::circle)] + m_parameter.m_wall_num[static_cast<int>(wall_type::needle)], wall_type::circle);
	for (int i(0); i < m_parameter.m_wall_num[static_cast<int>(wall_type::needle)]; ++i) {
		random_needle[i] = wall_type::needle;
	}
	global::ms_global->m_uniform[caller::Problem]->shuffle(random_needle.begin(), random_needle.end());
	std::vector<wall_type> modility(m_parameter.m_wall_num[static_cast<int>(wall_type::small_loop)] + m_parameter.m_wall_num[static_cast<int>(wall_type::large_loop)], wall_type::small_loop);
	for (int i(0); i < m_parameter.m_wall_num[static_cast<int>(wall_type::large_loop)]; ++i) {
		modility[i] = wall_type::large_loop;
	}
	global::ms_global->m_uniform[caller::Problem]->shuffle(modility.begin(), modility.end());
	std::vector<wall_type> type;

	int type_idx(global::ms_global->m_uniform[caller::Problem]->next_non_standard<int>(0, random_needle.size()));
	type.insert(type.begin(), random_needle.begin(), random_needle.begin() + type_idx);
	type.insert(type.begin() + type.size(), modility.begin(), modility.begin() + modility.size());
	//type.emplace_back(modility);
	type.insert(type.begin() + type.size(), random_needle.begin() + type_idx, random_needle.begin() + random_needle.size());

	int variable_size(type.size());
	m_wall_types.resize(variable_size);
	for (int dim_idx(0); dim_idx < variable_size; ++dim_idx) {
		real wall_center(0);
		if (dim_idx) {
			wall_center = m_wall_types[dim_idx - 1].m_center + global::ms_global->m_uniform[caller::Problem]->next_non_standard<real>(m_parameter.m_width_range.first, m_parameter.m_width_range.second);
		}
		m_wall_types[dim_idx].set_position_style(&wall_center, &type[dim_idx], m_parameter.m_max_radius, m_parameter.m_small_loop_radius, m_parameter.m_large_loop_inner);
		m_wall_types[dim_idx].set_numPos(m_parameter.m_num_small_points, m_parameter.m_num_large_points, m_parameter.m_num_circle_points, m_parameter.m_num_needle_points);
		m_wall_types[dim_idx].m_bag_value_seed = global::ms_global->m_uniform[caller::Problem]->next();
		m_wall_types[dim_idx].m_center_id = global::ms_global->m_uniform[caller::Problem]->next_non_standard<int>(0, m_wall_types[dim_idx].m_numPos);
		m_parameter.update_price_basic_par();
		m_wall_types[dim_idx].m_bag_basic_value.initialize(m_parameter.m_price_basic_par);
	}
}


void  OFEC::SP::map_info::initialize() {

	m_parameter.initialize_edge_par();
	m_parameter.initialize_feasible_par();
	m_parameter.initialize_price_basic_par();
	m_parameter.initialize_station_basic_par();
	

	initialize_wall_type();
	m_discount.resize(m_parameter.m_station_type_num);
	for (auto& it: m_discount) {
		it.first = global::ms_global->m_uniform[caller::Problem]->next_non_standard<real>(m_parameter.m_ratio_level.first, m_parameter.m_ratio_level.second);
		it.second= global::ms_global->m_uniform[caller::Problem]->next_non_standard<real>(m_parameter.m_discount.first, m_parameter.m_discount.second);
	}

	m_stations.resize(m_wall_types.size());
	for (int wall_id(0); wall_id < m_wall_types.size(); ++wall_id) {
		m_stations[wall_id].resize(m_wall_types[wall_id].m_numPos);
		for (int pos_id(0); pos_id < m_wall_types[wall_id].m_numPos; ++pos_id) {
			auto& cur_station(m_stations[wall_id][pos_id]);

			cur_station.m_wall = &m_wall_types[wall_id];
			cur_station.m_station_wall_id = pos_id;
			cur_station.m_cor_id = global::ms_global->m_uniform[caller::Problem]->next_non_standard<int>(0, m_parameter.m_station_type_num);
			
			cur_station.m_static_flag = global::ms_global->m_uniform[caller::Problem]->next() > m_parameter.m_moving_station_ratio;

			m_parameter.update_station_basic_par(cur_station.m_static_flag);
			cur_station.m_radius.initialize(m_parameter.m_station_basic_par);
			m_parameter.update_station_basic_par(cur_station.m_static_flag);
			cur_station.m_angle.initialize(m_parameter.m_station_basic_par);
			m_parameter.update_feasible_par();
			cur_station.m_feasible.initialize(m_parameter.m_feasible_par);
			
		}
	}


	m_edges.resize(m_wall_types.size());
	for (size_t wall_id(0); wall_id+1 < m_wall_types.size(); ++wall_id) {
		m_edges[wall_id].resize(m_wall_types[wall_id].m_numPos);
		for (int from_id(0); from_id < m_wall_types[wall_id].m_numPos; ++from_id) {
			m_edges[wall_id][from_id].resize(m_wall_types[wall_id+1].m_numPos);
			for (int to_id(0); to_id < m_wall_types[wall_id+1].m_numPos; ++to_id) {
				m_edges[wall_id][from_id][to_id].m_station_from = from_id;
				m_edges[wall_id][from_id][to_id].m_station_to = to_id;
				m_parameter.update_edge_par();
				m_edges[wall_id][from_id][to_id].m_distance_t_scale.initialize(m_parameter.m_edge_par);

				m_parameter.update_feasible_par();
				m_edges[wall_id][from_id][to_id].m_feasible.initialize(m_parameter.m_feasible_par);
			}
		}
	}
	m_max_distance = 0;
	for (int dim(0); dim + 1 < m_wall_types.size(); ++dim) {
		m_max_distance += m_wall_types[dim].get_outer_position((dim % 2) * OFEC_PI).distance(m_wall_types[dim + 1].get_outer_position(((dim + 1) % 2) * OFEC_PI));
	}
	m_max_distance *= (1.0 + m_parameter.m_edge_noisy_time.second);
}


real OFEC::SP::map_info::get_effective_mean_value(int T,const std::vector<int>& x) {
	if (x.size() != m_wall_types.size()) {
		return 0.0;
	}
	else {
		std::vector<int> nei(x);
		bool feasible_tag = true;
		real x_obj(get_mean_value(T,x,feasible_tag));
		real y_obj(0);
		real eff_val(0);
		int variable_size(m_wall_types.size());
		for (int dim(0); dim < variable_size; ++dim) {
			real cur_val(0);
			for (int idx(0); idx < m_wall_types[dim].get_numPos(); ++idx) {
				if (idx == x[dim]) {
					cur_val += x_obj;
				}
				else {
					nei[dim] = idx;
					cur_val += get_mean_value(T, nei, feasible_tag);

				}
			}
			nei[dim] = x[dim];
			eff_val += cur_val / static_cast<double>(m_wall_types[dim].get_numPos());
		}
		eff_val /= static_cast<double>(variable_size);
		return eff_val;
	}
}


void  OFEC::SP::map_info::get_optimal(int time_omega, std::vector<std::pair<int, std::vector<int>>>& sols,real min_dis) {

	// as time is infinit, we can not find the optimal solutions by dp
//
//	// use dp to find the optimal solutions;
//	std::vector<solution_type> sols;
//	std::vector<std::vector<solution_type>> dp_time_pos_sols;
//	std::vector<std::vector<solution_type>> dp_time_pos_after_sols;
//	solution_type cur_sol;
//	cur_sol.resize_objective(2);
//	cur_sol.variable().vect().clear();
////	dp_sols.push_back(std::vector<solution_type>());
//	std::vector<solution_type> v_cur_sol;
	//
	//{

	//	int variable_size(m_wall_types.size());
	//	
	//	int dim(0);
	////	dp_sols.resize(m_wall_types[dim].get_numPos());
	//	for (int idx(0); idx < m_wall_types[dim].get_numPos(); ++idx) {
	//		dp_sols[idx].resize(1);
	//		dp_sols[idx].front().variable().vect().push_back(dim);
	//		dp_sols[idx].front().resize_objective(2);
	//	}

	//	for (++dim; dim < variable_size; ++dim) {
	//		real cur_val(0);
	//	//	dp_after_sols.resize(m_wall_types[dim].get_numPos());
	//		for (int idx(0); idx < m_wall_types[dim].get_numPos(); ++idx) {
	//			
	//		}
	//	}
	//}
	//

}

bool OFEC::SP::selection_problem::same(const solution_base& s1, const solution_base& s2) const
{
	auto& x1 = dynamic_cast<const solution<variable_vector<int>, real>&>(s1).variable();
	auto& x2 = dynamic_cast<const solution<variable_vector<int>, real>&>(s2).variable();
	return x1.vect() == x2.vect();
}

real OFEC::SP::selection_problem::variable_distance(const solution_base& s1, const solution_base& s2) const
{
	auto& x1 = dynamic_cast<const solution<variable_vector<int>, real>&>(s1).variable();
	auto& x2 = dynamic_cast<const solution<variable_vector<int>, real>&>(s2).variable();

	if (x1.size() != x2.size())return x1.size();

	//	auto& points(m_pro_piece[m_pro_time]->get_points());
	real dis(0);
	for (int i(0); i < m_variable_size; ++i) {
		if (x1[i] != x2[i]) {
			dis += 1;
			if (i + 1 < m_variable_size && x1[i + 1] != x2[i + 1]) {
				dis += m_variable_size;
			}

		}
	}
	return dis;
}

real OFEC::SP::selection_problem::variable_distance(const variable_base& s1, const variable_base& s2) const
{
	auto& x1 = dynamic_cast<const solution<variable_vector<int>, real>&>(s1).variable();
	auto& x2 = dynamic_cast<const solution<variable_vector<int>, real>&>(s2).variable();



	if (x1.size() != x2.size())return x1.size();

	//	auto& points(m_pro_piece[m_pro_time]->get_points());
	real dis(0);
	for (int i(0); i < m_variable_size; ++i) {
		if (x1[i] != x2[i]) {
			dis += 1;
			if (i + 1 < m_variable_size&&x1[i+1]!=x2[i+1]) {
				dis += m_variable_size;
			}
		}
	}

	
	return dis;
}

evaluation_tag OFEC::SP::selection_problem::evaluate_(solution_base& s, caller call, bool effective, bool initialized)
{

	auto& x = dynamic_cast<solution_type&>(s).variable();
	auto& obj = dynamic_cast<solution_type&>(s).objective();
	bool feasible_flag(false);
	obj.resize(1);
	obj[0] = m_map.get_value(m_T, x.vect(), feasible_flag);

	if (initialized) {
		if (effective)		m_effective_eval++;
		if (call == caller::Algorithm && global::ms_global->m_algorithm && global::ms_global->m_algorithm->terminating())
			return evaluation_tag::Terminate;
		if (is_change())change();
	}
	return evaluation_tag::Normal;
}

void OFEC::SP::selection_problem::initialize_solution(solution_base& s) const
{
	auto& x = dynamic_cast<solution_type&>(s).variable();
	x.resize(m_variable_size);
	for (int i(0); i < x.size(); ++i) {
		x[i] = global::ms_global->m_uniform[caller::Problem]->next_non_standard<int>(0, m_map.m_wall_types[i].m_numPos);
	}
	
}

void OFEC::SP::selection_problem::initialize()
{
	m_T = 0;
	m_map.initialize();
	m_variable_size = m_map.m_wall_types.size();
}

void OFEC::SP::selection_problem::generate_level_samples(std::vector<solution_type>& samples,caller call)
{
	samples.clear();
	int max_nodes(0);
	std::vector<std::vector<int>> solus(m_map.m_wall_types.size());

	int variable_size(m_map.m_wall_types.size());
	for (int dim(0); dim < m_map.m_wall_types.size(); ++dim) {
		solus[dim].resize(m_map.m_wall_types[dim].m_numPos);
		max_nodes = std::max(max_nodes, m_map.m_wall_types[dim].m_numPos);
		for (int idx(0); idx < solus[dim].size(); ++idx) {
			solus[dim][idx] = idx;
		}
		global::ms_global->m_uniform[call]->shuffle(solus[dim].begin(), solus[dim].end());	
	}

	samples.resize(max_nodes);
	for (auto& it : samples) {
		it.variable().resize(variable_size);
		it.objective().resize(1);
	}

	for (int idx(0); idx < max_nodes; ++idx) {
		for (int dim(0); dim < variable_size; ++dim) {
			samples[idx].variable()[dim] = solus[dim][idx % m_map.m_wall_types[dim].m_numPos];
		}
	}
	for (auto& it : samples) {
		it.evaluate();
	}

}

evaluation_tag OFEC::SP::selection_problem::evaluate_effective_value(solution_base& s, caller call, bool effective, bool initialized)
{
	evaluation_tag rf_tag(evaluate_(s, call, effective, initialized));
	if (rf_tag == evaluation_tag::Terminate) return rf_tag;
	solution_type neighbors(dynamic_cast<const solution_type&>(s));
	solution_type cur_sol(dynamic_cast<const solution_type&>(s));
	real eff_val(0);
	int variable_size(m_map.m_wall_types.size());
	for (int dim(0); dim < variable_size; ++dim) {
		real cur_val(0);
		for (int idx(0); idx < m_map.m_wall_types[dim].get_numPos(); ++idx) {
			if (idx == cur_sol.variable()[dim]) {
				cur_val += cur_sol.objective()[0];
			}
			else {
				neighbors.variable()[dim] = idx;
				rf_tag=evaluate_(neighbors, call, effective, initialized);
				if (rf_tag == evaluation_tag::Terminate) return rf_tag;
				cur_val += neighbors.objective()[0];

			}
		}
		neighbors.variable()[dim] = cur_sol.variable()[dim];
		eff_val+=cur_val / static_cast<double>(m_map.m_wall_types[dim].get_numPos());
	}
	eff_val /= static_cast<double>(variable_size);
	cur_sol.constraint_value().front() = eff_val;
	return rf_tag;
}
