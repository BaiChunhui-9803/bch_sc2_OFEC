//Register selection_problem "SP" SP,ComOP,DOP

#ifndef SELECTION_PROBLEM_H
#define SELECTION_PROBLEM_H

#include "../../../../core/problem/problem.h"
#include "../../../../core/problem/optima.h"
#include "../../../../core/problem/domain.h"
#include "../../../../core/global.h"
#include"../../../../core/algorithm/solution.h"
#include<string>
#include<map>
#include<array>
#include"../../../../utility/vector.h"
#include"../../../../utility/CubicSpline/CubicSpline.h"
#include"../../../../utility/functional.h"
#include"../../../../utility/elementary_function.h"
#include"../../../../utility/noisy_fun/noisy_fun.h"
namespace OFEC {
#define CAST_DYN_SP dynamic_cast<SP::selection_problem*>(global::ms_global->m_problem.get())

	namespace SP {
		

		struct Tt_fun_parameter {
			int m_id_rnd;
			std::vector<std::pair<double, double>> m_x_from;
			std::vector<std::pair<double, double>> m_x_to;
			std::vector<std::pair<double, double>> m_y_to;

			std::vector<elementary_function::elementary_fun_type> m_fun_type;

			std::vector<int> m_fun_num;
			std::vector<int> m_sample_num;
			
		};



		struct Tt_fun {
			int m_id_rnd;
			enum fun_name_type {mean_par=0,variance_par,mean_val,variance_var };

			std::unique_ptr<elementary_function> m_mean_par_fun;
			std::unique_ptr<elementary_function> m_variance_par_fun;
			std::unique_ptr<elementary_function> m_mean_value_fun;
			std::unique_ptr<elementary_function> m_variance_fun;

			Real m_cur_A = 0;
			Real m_cur_B = 0;
			int m_T = -1;

			Real m_max_value = 0;
			Real m_min_value = 0;

			void get_max_value(Real& max_mean, Real& max_var) {
				max_mean = m_mean_par_fun->get_to_y_range().second * m_mean_value_fun->get_to_y_range().second;
				max_var = m_variance_par_fun->get_to_y_range().second * m_variance_fun->get_to_y_range().second;
			}
			Real get_cur_value(Real t) {
				Real cur_val(GET_RND(m_id_rnd).normal.nextNonStd<double>(m_cur_A * m_mean_value_fun->get_value(t), m_cur_B * m_variance_fun->get_value(t)));
				//Real cur_val(global::ms_global->m_normal[caller::Problem]->next_non_standard(m_cur_A * m_mean_value_fun->get_value(t), m_cur_B * m_variance_fun->get_value(t)));
				if (cur_val > m_max_value) cur_val = m_max_value;
				else if (cur_val < m_min_value) cur_val = m_min_value;
				return cur_val;
			}
			Real get_value_intT(int T, Real t) {
				if (T != m_T) {
					m_T = T;
					m_cur_A = m_mean_par_fun->get_value(T);
					m_cur_B = m_variance_par_fun->get_value(T);
				}
				return get_cur_value(t);
			//	return m_cur_A * m_mean_value_fun->get_value(t) + m_cur_B * m_variance_fun->get_value(t) * global::ms_global->m_uniform[caller::Problem]->next();

			}


			Real get_mean_value_intT(int T, Real t) {
				if (T != m_T) {
					m_T = T;
					m_cur_A = m_mean_par_fun->get_value(T);
					m_cur_B = m_variance_par_fun->get_value(T);
				}
				return m_cur_A * m_mean_value_fun->get_value(t);

			}

			Real get_value(Real T, Real t) {

				m_cur_A = m_mean_par_fun->get_value(T);
				m_cur_B = m_variance_par_fun->get_value(T);
				return get_cur_value(t);
				//return m_cur_A * (sum_s + m_C) + m_cur_B *( global::ms_global->m_uniform[caller::Problem]->next()*m_flag_noise+(1-m_flag_noise));
			}
			void get_origin_value(Real T, Real t, Real& a, Real& b,Real & cur_mean,Real & cur_var) {
			    m_cur_A = m_mean_par_fun->get_value(T);
                m_cur_B = m_variance_par_fun->get_value(T);
				a = m_cur_A;
				b = m_cur_B;
				cur_mean= m_mean_value_fun->get_value(t);
				cur_var = m_variance_fun->get_value(t);;
			}
			void get_mean_value(Real T, Real t,Real & mean,Real& var) {
				m_cur_A = m_mean_par_fun->get_value(T);
				m_cur_B = m_variance_par_fun->get_value(T);
				mean = m_cur_A * m_mean_value_fun->get_value(t);
				var=m_cur_B* m_variance_fun->get_value(t);
			}
		
			void set_id_rnd(int id_rnd) {
				m_id_rnd = id_rnd;
			}
			void initialize(const Tt_fun_parameter& par) {

				fun_name_type fun_name;
				elementary_function::elementary_fun_type fun_type;
				int fun_num(0), sample_num(0);
				for (int i(0); i < 4; ++i) {
					fun_name = static_cast<fun_name_type>(i);
					fun_type = par.m_fun_type[i];
					fun_num = par.m_fun_num[i];
					sample_num = par.m_sample_num[i];
					//elementary_function_paramenter(v, par.m_x_from[i], par.m_x_to[i], par.m_y_to[i], &call, &fun_num, &sample_num);
					switch (fun_name)
					{
					case OFEC::SP::Tt_fun::mean_par:
						elementary_function_initialize(m_mean_par_fun, fun_type,
							par.m_x_from[i],par.m_x_to[i],par.m_y_to[i],call,fun_num,sample_num);
						m_mean_par_fun->initialize();
						break;
					case OFEC::SP::Tt_fun::variance_par:
						elementary_function_initialize(m_variance_par_fun, fun_type,
							par.m_x_from[i], par.m_x_to[i], par.m_y_to[i], call, fun_num, sample_num);
						m_variance_par_fun->initialize();
						break;
					case OFEC::SP::Tt_fun::mean_val:
						elementary_function_initialize(m_mean_value_fun, fun_type,
							par.m_x_from[i], par.m_x_to[i], par.m_y_to[i], call, fun_num, sample_num);
						m_mean_value_fun->initialize();
						break;
					case OFEC::SP::Tt_fun::variance_var:
						elementary_function_initialize(m_variance_fun, fun_type,
							par.m_x_from[i], par.m_x_to[i], par.m_y_to[i], call, fun_num, sample_num);
						m_variance_fun->initialize();
						break;
					default:
						break;
					}
				}


				m_max_value =
					m_mean_par_fun->get_to_y_range().second * m_mean_par_fun->get_to_y_range().second
					+ m_variance_fun->get_to_y_range().second * m_variance_par_fun->get_to_y_range().second * 3.0;
				m_min_value =
					m_mean_par_fun->get_to_y_range().first * m_mean_par_fun->get_to_y_range().first
					+ m_variance_fun->get_to_y_range().first * m_variance_par_fun->get_to_y_range().first;

			}


			void initialize(fun_name_type fun_name, elementary_function::elementary_fun_type fun_type, const param_map& v) {

				switch (fun_name)
				{
				case OFEC::SP::Tt_fun::mean_par:
					elementary_function_initialize(m_mean_par_fun, fun_type, v);
					m_mean_par_fun->initialize();
					break;
				case OFEC::SP::Tt_fun::variance_par:
					elementary_function_initialize(m_variance_par_fun, fun_type, v);
					m_variance_par_fun->initialize();
					break;
				case OFEC::SP::Tt_fun::mean_val:
					elementary_function_initialize(m_mean_value_fun, fun_type, v);
					m_mean_value_fun->initialize();
					break;
				case OFEC::SP::Tt_fun::variance_var:
					elementary_function_initialize(m_variance_fun, fun_type, v);
					m_variance_fun->initialize();
					break;
				default:
					break;
				}
			}
		};


		struct SP_par {
			
			int m_id_rnd;

			Tt_fun_parameter m_edge_par;
			int mc_T = 1000;

			// edge
			std::pair<double, double>  m_edge_delay_time = { 0.0,1.0 };
			std::pair<double, double>  m_edge_noisy_time = { 0, 0.4285 };
			int m_edge_max_cos_fun = 3;
			int m_edge_sample_num = 100;
			int m_edge_noisy_sample = 10;

			std::vector<elementary_function::elementary_fun_type> m_edge_fun_flag = {
				elementary_function::random_fun,elementary_function::random_fun,
				elementary_function::cos_fun,elementary_function::random_fun };


			void initialize_edge_par() {

				m_edge_par.m_fun_type = m_edge_fun_flag;
				m_edge_par.m_fun_num = { 0,0,1,0 };
				m_edge_par.m_sample_num = { m_edge_sample_num ,m_edge_sample_num ,0,m_edge_noisy_sample };
				m_edge_par.m_x_from = { {0,mc_T},{0,mc_T},{0,1},{0,1} };
				m_edge_par.m_x_to = { {0,1},{0,1},{0,3 * OFEC_PI},{0,1} };
				m_edge_par.m_y_to = { m_edge_delay_time,m_edge_noisy_time,{0,1},{0,1} };
			}

			void update_edge_par() {
				m_edge_par.m_fun_num[2] =
					GET_RND(m_id_rnd).uniform.nextNonStd<double>(1, m_edge_max_cos_fun + 1);
				m_edge_par.m_y_to[0].second =
					GET_RND(m_id_rnd).uniform.nextNonStd<double>(m_edge_delay_time.first, m_edge_delay_time.second);
				m_edge_par.m_y_to[1].second =
					GET_RND(m_id_rnd).uniform.nextNonStd<double>
					(m_edge_noisy_time.first, m_edge_noisy_time.second);

			}


            Real get_max_distance_scale() {
				return m_edge_delay_time.second + m_edge_noisy_time.second + 1.0;
			}


			void get_edge_par(Tt_fun::fun_name_type fun_name, elementary_function::elementary_fun_type& fun_type, param_map & v) {
				
				fun_type = m_edge_fun_flag[static_cast<int>(fun_name)];

				std::pair<double, double> from_x = { 0,1 };
				std::pair<double, double> to_x = { 0,1 };
				std::pair<double, double> to_y = { 0,1 };
				int fun_num(0);
				int sample_num(m_edge_sample_num);
				switch (fun_name)
				{
				case OFEC::SP::Tt_fun::mean_par:
					from_x = { 0,mc_T };
					to_y = m_edge_delay_time;
					break;
				case OFEC::SP::Tt_fun::variance_par:
					from_x = { 0,mc_T };
					to_y = m_edge_noisy_time;
					break;
				case OFEC::SP::Tt_fun::mean_val :
					to_x = { 0,3 * OFEC_PI };
					fun_num = GET_RND(m_id_rnd).uniform.nextNonStd<double>
					(0, m_edge_max_cos_fun) + 1;
					break;
				case OFEC::SP::Tt_fun::variance_var:
				{
					sample_num = m_edge_noisy_sample;
				}
					break;
				default:
					break;
				}
				elementary_function_paramenter(v, from_x, to_x, to_y, m_id_rnd, &fun_num, &sample_num);
			}


			Tt_fun_parameter m_price_seed_par;

			// price seed 
			std::pair<double, double>  m_price_seed_variation = { 0.0, 1.0 };
			std::pair<double, double>  m_price_seed_noisy = { 0,0 };
			int m_price_seed_sample_num = 10;
			std::vector<elementary_function::elementary_fun_type> m_price_seed_fun_flag =
			{ elementary_function::random_fun,elementary_function::constant_fun,
				elementary_function::constant_fun,elementary_function::constant_fun };




			void initialize_price_seed_par() {

				m_price_seed_par.m_fun_type = m_price_seed_fun_flag;
				m_price_seed_par.m_fun_num = { 0,0,0,0 };
				m_price_seed_par.m_sample_num = { m_price_seed_sample_num ,0 ,0,0 };
				m_price_seed_par.m_x_from = { {0,mc_T},{0,mc_T},{0,1},{0,1} };
				m_price_seed_par.m_x_to = { {0,1},{0,1},{0,1},{0,1} };
				m_price_seed_par.m_y_to = { m_price_seed_variation,m_price_seed_noisy,m_price_seed_noisy,m_price_seed_noisy };
			}

			void update_price_seed_par() {
				m_price_seed_par.m_y_to[0].second = 
					global::ms_global->m_uniform[caller::Problem]->next_non_standard<double>
					(m_price_seed_variation.first, m_price_seed_variation.second);
			}



			void get_price_par(Tt_fun::fun_name_type fun_name, elementary_function::elementary_fun_type& 
				fun_type, param_map& v) {


				fun_type = m_price_seed_fun_flag[static_cast<int>(fun_name)];

				std::pair<double, double> from_x = { 0,1 };
				std::pair<double, double> to_x = { 0,1 };
				std::pair<double, double> to_y = m_price_seed_noisy;

				caller call(caller::Problem);
				int fun_num(0);
				int sample_num(m_price_seed_sample_num);
				switch (fun_name)
				{
				case OFEC::SP::Tt_fun::mean_par:
					from_x = { 0,mc_T };
					to_y = m_price_seed_variation;
					break;
				case OFEC::SP::Tt_fun::variance_par:
					from_x = { 0,mc_T };
					break;
				case OFEC::SP::Tt_fun::mean_val:
					break;
				case OFEC::SP::Tt_fun::variance_var:
				{
					sample_num = m_edge_noisy_sample;
				}
				break;
				default:
					break;
				}
				elementary_function_paramenter(v, from_x, to_x, to_y, &call, &fun_num, &sample_num);
			}


			Tt_fun_parameter m_price_basic_par;

			// basic price
			std::pair<double, double>  m_price_variation = { 1.0, 1.035 };
			std::pair<double, double>  m_price_noisy = { 0,0};
			int m_price_sample_num = 10;
			std::vector<elementary_function::elementary_fun_type> m_basic_price_fun_flag = 
			{ elementary_function::random_fun,elementary_function::constant_fun,
				elementary_function::constant_fun,elementary_function::constant_fun };


			void initialize_price_basic_par() {

				m_price_basic_par.m_fun_type = m_basic_price_fun_flag;
				m_price_basic_par.m_fun_num = { 0,0,0,0 };
				m_price_basic_par.m_sample_num = { m_price_sample_num ,0 ,0,0 };
				m_price_basic_par.m_x_from = { {0,mc_T},{0,mc_T},{0,1},{0,1} };
				m_price_basic_par.m_x_to = { {0,1},{0,1},{0,1},{0,1} };
				m_price_basic_par.m_y_to = { m_price_variation,m_price_noisy,m_price_noisy,m_price_noisy };
			}

			void update_price_basic_par() {
				m_price_basic_par.m_y_to[0].second =
					global::ms_global->m_uniform[caller::Problem]->next_non_standard<double>
					(m_price_variation.first, m_price_variation.second);
			}


			Tt_fun_parameter m_station_basic_par;

			// station position 
			double m_moving_station_ratio = 0.3;
			std::pair<double, double> m_s_position_static_var = { 1.0,1.0 };
			std::pair<double, double>  m_s_position_variation = { 0.0,1.0 };
			std::pair<double, double>  m_s_position_noisy = { 0, 0.01};
			int m_position_sample_num = 10;
			std::vector<elementary_function::elementary_fun_type> m_position_fun_flag = {
				elementary_function::constant_fun,elementary_function::constant_fun,
				elementary_function::random_fun,elementary_function::random_fun};

			std::vector<elementary_function::elementary_fun_type> m_position_fun_static_flag = {
	          elementary_function::constant_fun,elementary_function::constant_fun,
	          elementary_function::constant_fun,elementary_function::constant_fun };
  


			void initialize_station_basic_par() {

				m_station_basic_par.m_fun_type = m_position_fun_flag;
				m_station_basic_par.m_fun_num = { 0,0,0,0 };
				m_station_basic_par.m_sample_num = { 0 ,0 ,m_position_sample_num,m_position_sample_num };
				m_station_basic_par.m_x_from = { {0,mc_T},{0,mc_T},{0,1},{0,1} };
				m_station_basic_par.m_x_to = { {0,1},{0,1},{0,1},{0,1} };

				m_station_basic_par.m_y_to = { m_s_position_static_var,m_s_position_static_var,
					m_s_position_variation,m_s_position_noisy };
			}

			void update_station_basic_par(bool static_flag) {
				if (static_flag) {
					m_station_basic_par.m_fun_type = m_position_fun_static_flag;
					 m_station_basic_par.m_y_to[2].first
						 =m_station_basic_par.m_y_to[2].second =
						global::ms_global->m_uniform[caller::Problem]->next_non_standard<double>
						(m_s_position_variation.first, m_s_position_variation.second);
					m_station_basic_par.m_y_to[3].first = 
					m_station_basic_par.m_y_to[3].second =
						global::ms_global->m_uniform[caller::Problem]->next_non_standard<double>
						(m_s_position_noisy.first, m_s_position_noisy.second);
				}
				else {
					m_station_basic_par.m_fun_type = m_position_fun_flag;
					m_station_basic_par.m_y_to[2].first = 0;
					m_station_basic_par.m_y_to[2].second =
						global::ms_global->m_uniform[caller::Problem]->next_non_standard<double>
						(m_s_position_variation.first, m_s_position_variation.second);
					m_station_basic_par.m_y_to[3].first = 0;
					m_station_basic_par.m_y_to[3].second =
						global::ms_global->m_uniform[caller::Problem]->next_non_standard<double>
						(m_s_position_noisy.first, m_s_position_noisy.second);
				}


			}



			Tt_fun_parameter m_feasible_par;

			std::pair<double, double> m_feasible_mean = { 0,1.0 };
			std::pair<double, double> m_feasible_range = { 0.9,1.0 };
			std::pair<double, double>  m_feasible_noise = { 0, 0.01 };
			int m_feasible_sample_num = 10;
			int m_feasible_large_sample_num = 100;
			std::vector<elementary_function::elementary_fun_type> m_feasible_flag = {
				elementary_function::random_fun,elementary_function::random_fun,
				elementary_function::random_fun,elementary_function::random_fun };


			void initialize_feasible_par() {

				m_feasible_par.m_fun_type = m_feasible_flag;
				m_feasible_par.m_fun_num = { 0,0,0,0 };
				m_feasible_par.m_sample_num = { m_feasible_large_sample_num ,m_feasible_large_sample_num ,m_feasible_sample_num,m_feasible_sample_num };
				m_feasible_par.m_x_from = { {0,mc_T},{0,mc_T},{0,1},{0,1} };
				m_feasible_par.m_x_to = { {0,1},{0,1},{0,1},{0,1} };

				m_feasible_par.m_y_to = { m_feasible_mean,m_feasible_mean,
					m_feasible_mean,m_feasible_noise };
			}
			void update_feasible_par() {
				m_feasible_par.m_y_to[0].second = global::ms_global->m_uniform[caller::Problem]->next_non_standard<Real>(m_feasible_range.first, m_feasible_range.second);
			}


			//! parameters
			std::pair<Real, Real> m_width_range = { 0.01,2.0 };
			// wall character
			Real m_max_radius = 2;    
			Real m_small_loop_radius = 0.1;

			Real m_large_loop_inner = 0.1;
			int m_num_small_points = 10;
			int m_num_large_points = 20;
			int m_num_circle_points = 30;
			int m_num_needle_points = 20;

			std::vector<int> m_wall_num = { 3,3,7,4 };

			int m_station_type_num = 10;
			std::pair<Real, Real> m_ratio_level = { 0.6,0.9 };
			std::pair<Real, Real> m_discount = { 0.8,1.0 };
		};

		enum class wall_type { small_loop = 0, large_loop, circle, needle };
		extern const int wall_type_num;
		struct wall_info {
			//	const static Real ms_default_bag_basic_value;
			Real m_center = 0;
			Real m_inner_radius = 0;
			Real m_outer_radius = 0;
			Real m_from_theta = 0;
			Real m_to_theta = 0;
			int m_numPos = 0;
			int m_center_id = 0;
			wall_type m_type = wall_type::small_loop;
			Real m_bag_value_seed;
			Tt_fun m_bag_basic_value;
			
			Vector get_outer_position(Real angle) {
				Vector pos(3,0);
				pos[0] = m_center;
				pos[1] = m_outer_radius * sin(angle);
				pos[2] = m_outer_radius * cos(angle);
				return std::move(pos);
			}

			int get_numPos()const {
				return m_numPos;
			}

			void set_position_style(Real* center,
				wall_type* type,
				Real max_radius = 2.0, Real small_loop_radius = 0.1,
				Real large_loop_inner = 0.1);
			void set_position_style(const wall_info& wall) {
				m_center = wall.m_center;
				m_type = wall.m_type;
				m_inner_radius = wall.m_inner_radius;
				m_outer_radius = wall.m_outer_radius;
				m_from_theta = wall.m_from_theta;
				m_to_theta = wall.m_to_theta;
			}

			Real get_mean_val(int T, Real t, const Vector& pos) {
				return m_bag_basic_value.get_mean_value_intT(T, t) * NOISY_FUN::m_ofNoise(pos[1], pos[2], m_bag_value_seed);

			}


			Real get_value(int T, Real t, const Vector& pos) {
				return m_bag_basic_value.get_value_intT(T, t) * NOISY_FUN::m_ofNoise(pos[1], pos[2], m_bag_value_seed);
			}

			// range [0,1]
			Vector get_position(int pos_id,Real radius_seed,Real angle_seed) {
				Vector pos(3, 0);
				pos[0] = m_center;
				Real r(0), angle(0);
				if (m_type == wall_type::needle&&pos_id==m_center_id) {
					r = (m_inner_radius + 2 * OFEC_PI + m_outer_radius) / 2.0;
				}
				else {
					r = map_real<Real>(radius_seed, 0, 1, m_inner_radius, m_outer_radius);
			
				}
				angle = map_real<Real>(angle_seed, 0, 1, m_from_theta, m_to_theta);

				pos[1] = r * sin(angle);
				pos[2] = r * cos(angle);
				return std::move(pos);
			}

			static int cal_numPos(wall_type type, int num_small_points = 10,
				int num_large_points = 20,
				int num_circle_points = 50,
				int num_needle_points = 30);

			void set_numPos(int num_small_points = 10,
				int num_large_points = 20,
				int num_circle_points = 50,
				int num_needle_points = 30);

			void set_numPos(const wall_info& wall) {
				m_numPos = wall.m_numPos;
			}
		};
		struct station {


			wall_info* m_wall = nullptr;
			int m_station_wall_id = 0;
			int m_cor_id;

			bool m_static_flag = false;
			Tt_fun m_radius;
			Tt_fun m_angle;
			Tt_fun m_feasible;



			Vector get_position(int T, Real t) {
				return std::move(m_wall->get_position(m_station_wall_id, m_radius.get_value_intT(T, t), m_angle.get_value_intT(T, t)));
			}
			Real get_feasible(int T, Real t) {
				return m_feasible.get_value_intT(T, t);
			}
			Real get_mean_feasible(int T, Real t) {
				return m_feasible.get_mean_value_intT(T, t);
			}

			Real get_value(int T, Real t) {
				return m_wall->get_value(T, t, get_position(T, t));
			}

			Real get_mean_value(int T, Real t) {
				return m_wall->get_mean_val(T, t, get_mean_position(T, t));
			}

			Vector get_mean_position(int T, Real t) {
				return std::move(m_wall->get_position(m_station_wall_id, m_radius.get_mean_value_intT(T, t), m_angle.get_mean_value_intT(T, t)));

			}


			virtual void initialize_radius(const Tt_fun_parameter& para) {
				m_radius.initialize(para);
			}
			virtual void initialize_angle(const Tt_fun_parameter& par) {
				m_angle.initialize(par);
			}
			virtual void initialize_feasible(const Tt_fun_parameter& par) {
				m_feasible.initialize(par);
			}
		};


		struct edge {
			// parameter 
			int m_station_from = 0;
			int m_station_to = 0;
			Tt_fun m_distance_t_scale;
			Tt_fun m_feasible;


			////![open =1  close =0 ,breakdown =3]
			//int m_state =1;
			Real get_mean_distance_scale(int T, Real t) {
				return 1.0 + m_distance_t_scale.get_mean_value_intT(T, t);
			}
			Real get_distance_scale(int T, Real t) {
				return 1.0 + m_distance_t_scale.get_value_intT(T, t);
	/*			if (m_cur_T != T) {
					m_cur_T = T;
					m_cur_A = m_distance_t_scale.m_A_fun->get_value(T);
					m_cur_B = m_distance_t_scale.m_B_fun->get_value(T);
				}
				return 1.0 + m_cur_A * m_distance_t_scale.m_mean_value_fun->get_value(t) + m_cur_B * m_distance_t_scale.m_variance_fun->get_value(t);*/
			}
			Real get_feasible(int T, Real t) {
				return m_feasible.get_value_intT(T, t);
			}

			void initialize_scale(const Tt_fun_parameter& par) {
				m_distance_t_scale.initialize(par);
			}
			void initialize_feasible(const Tt_fun_parameter& par) {
				m_feasible.initialize(par);
			}
		};


		struct map_info  {
			using solution_type = solution<variable_vector<int>, Real>;
			using variable_type = variable_vector<int>;
		
			//wall_info m_shape;
			std::vector<wall_info> m_wall_types;
			// station id in wall 
			std::vector<std::vector<station>> m_stations;
			//std::vector<station> m_station
	        std::vector<std::vector<std::vector<edge>>> m_edges;
	
			std::vector<std::pair<Real, Real>> m_discount;
			

			Real m_feasible_pro = 0.9;
			Real m_max_distance = 0.0;
			SP_par m_parameter;
			

			int get_candidate_num(int dim) {
				return m_wall_types[dim].m_numPos;
			}
			
			int get_cor_id(int dim, int from) {
				return m_stations[dim][from].m_cor_id;
			}

			Real get_price(int T, Real t,int dim,int from) {
				return m_stations[dim][from].get_value(T, t);
			}

			Real get_mean_price(int T, Real t, int dim, int from) {
				return m_stations[dim][from].get_mean_value(T, t);
			}




			Real get_distance(int T, Real t, int dim, int from, int to) {
				{
					// for test
					auto pos1(m_stations[dim][from].get_position(T, t));
					auto pos2(m_stations[dim + 1][to].get_position(T, t));
					auto edge_scale(m_edges[dim][from][to].get_distance_scale(T, t));
					
					Real dis(pos1.distance(pos2) * edge_scale);
					if (dis<0||dis>1e9) {
						int stop = -1;
					}
				}
				return m_stations[dim][from].get_position(T, t)
					.distance(m_stations[dim+1][to].get_position(T, t))
					* m_edges[dim][from][to].get_distance_scale(T, t);
			
			}
			Real get_mean_distance(int T, Real t, int dim, int from, int to) {
				return m_stations[dim][from].get_mean_position(T, t)
					.distance(m_stations[dim + 1][to].get_mean_position(T, t))
					* m_edges[dim][from][to].get_mean_distance_scale(T, t);
			}

			bool is_feasible(int T,Real t,int dim, int pos) {
				return m_stations[dim][pos].get_feasible(T, t) <= m_feasible_pro;
			}
			bool is_feasible(int T, Real t, int dim, int from, int to) {
				return m_edges[dim][from][to].get_feasible(T, t) <= m_feasible_pro;
			}


		
			void  initialize_wall_type();

			void initialize();

			Real get_heuristic_info(int T,int dim,int from, int to) {
				if (dim == -1) {
					return get_price(T, dim+1, 0, to);
				}
				else {

					Real cur_dis = m_max_distance * static_cast<double>(dim) / static_cast<double>(m_wall_types.size());
					return get_distance(T, cur_dis, dim, from, to) * get_price(T, cur_dis, dim, from);
				}
			}

			Real get_obj(int T, const std::vector<int>& x,int from_dim,int to_dim,
				Real & sum_dis, std::vector<Real> & type_dis, std::vector<Real>& type_val) {
				Real obj(0);
				Real next_dis(0);
				Real next_val(0);
				std::vector<Real> cur_dis(x.size(), 0);
				std::vector<Real> cur_val(x.size(), 0);
				for (int dim(from_dim+1); dim < to_dim; ++dim) {
					next_dis = get_distance(T, sum_dis, dim, x[dim-1], x[dim]) / m_max_distance;
					next_val = get_price(T, sum_dis, dim, x[dim-1]);
					type_dis[get_cor_id(dim, x[dim-1])] += next_dis;
					type_val[get_cor_id(dim, x[dim-1])] += next_dis * next_val;
					cur_dis[dim] = next_dis;
					cur_val[dim] = next_val;
					sum_dis += next_dis;
				}
				for (int cor_id(0); cor_id < type_dis.size(); ++cor_id) {
					if (type_dis[cor_id] >= m_discount[cor_id].first) {

						obj+= type_val[cor_id] * m_discount[cor_id].second;
					}
					else {
						obj += type_val[cor_id];
					}
				}
				return obj;
			}
			
			Real get_mean_value(int T, const std::vector<int>& x, bool& feasible_flag) {
				feasible_flag = true;
				if (x.size() != m_wall_types.size()) {
					feasible_flag = false;
					return 0.0;
				}
				{
					Real sum_dis(0), next_dis(0);
					Real next_val(0);
					std::vector<Real> type_dis(m_discount.size(), 0);
					std::vector<Real> type_val(m_discount.size(), 0);
					for (int dim(0); dim < x.size(); ++dim) {
						feasible_flag = feasible_flag && (!is_feasible(T, sum_dis, dim, x[dim]));
						if (dim + 1 == x.size())break;
						feasible_flag = feasible_flag && (!is_feasible(T, sum_dis, dim, x[dim], x[dim + 1]));
						next_dis = get_distance(T, sum_dis, dim, x[dim], x[dim + 1]) / m_max_distance;
						next_val = get_price(T, sum_dis, dim, x[dim]);
						type_dis[get_cor_id(dim, x[dim])] += next_dis;
						type_val[get_cor_id(dim, x[dim])] += next_dis * next_val;
						sum_dis += next_dis;
					}
					for (int cor_id(0); cor_id < type_dis.size(); ++cor_id) {
						if (type_dis[cor_id] >= m_discount[cor_id].first) {
							type_val[cor_id] *= m_discount[cor_id].second;
						}
					}
					Real obj(0);
					for (auto& it : type_val) obj += it;
					return obj;
				}
			}

			

			Real get_heuristic_info(int T, const std::vector<int>& x,int cur_dim) {

				{


					Real sum_dis(0), next_dis(0);
					Real next_val(0);
					std::vector<Real> type_dis(m_discount.size(), 0);
					std::vector<Real> type_val(m_discount.size(), 0);
					std::vector<Real> discount_val;
					std::vector<Real> cur_dis(cur_dim, 0);
					std::vector<Real> cur_val(cur_dim, 0);

					for (int dim(0); dim+1 < cur_dim; ++dim) {
						next_dis = get_distance(T, sum_dis, dim, x[dim], x[dim + 1]) / m_max_distance;
						next_val = get_price(T, sum_dis, dim, x[dim]);
						type_dis[get_cor_id(dim, x[dim])] += next_dis;
						type_val[get_cor_id(dim, x[dim])] += next_dis * next_val;
						cur_dis[dim] = next_dis;
						cur_val[dim] = next_val;
						sum_dis += next_dis;
					}

					discount_val = type_val;

					for (int cor_id(0); cor_id < type_dis.size(); ++cor_id) {
						if (type_dis[cor_id] >= m_discount[cor_id].first) {
							discount_val[cor_id] *= m_discount[cor_id].second;
						}
					}
					Real obj(0);
					for (auto& it : discount_val) obj += it;
					for (int dim(cur_dim-1); dim < cur_dim; ++dim) {
						next_dis = get_distance(T, sum_dis, dim, x[dim], x[dim + 1]) / m_max_distance;
						next_val = get_price(T, sum_dis, dim, x[dim]);
						type_dis[get_cor_id(dim, x[dim])] += next_dis;
						type_val[get_cor_id(dim, x[dim])] += next_dis * next_val;
						cur_dis[dim] = next_dis;
						cur_val[dim] = next_val;
						sum_dis += next_dis;
					}
					discount_val = type_val;

					for (int cor_id(0); cor_id < type_dis.size(); ++cor_id) {
						if (type_dis[cor_id] >= m_discount[cor_id].first) {
							discount_val[cor_id] *= m_discount[cor_id].second;
						}
					}
					Real obj2(0);
					for (auto& it : discount_val) obj2 += it;
					return obj2 - obj;
				}
			}




			Real get_value(int T,const std::vector<int>& x,bool & feasible_flag) {
				feasible_flag = true;
				if (x.size() != m_wall_types.size()) {
					feasible_flag = false;
					return 0.0;
				}
				{

				
					Real sum_dis(0), next_dis(0);
					Real next_val(0);
					std::vector<Real> type_dis(m_discount.size(),0);
					std::vector<Real> type_val(m_discount.size(), 0);


					std::vector<Real> cur_dis(x.size(), 0);
					std::vector<Real> cur_val(x.size(), 0);

					for (int dim(0); dim < x.size(); ++dim) {
						feasible_flag = feasible_flag&&(!is_feasible(T, sum_dis, dim, x[dim]));
						if (dim + 1 == x.size())break;
						feasible_flag = feasible_flag&&(!is_feasible(T, sum_dis, dim, x[dim], x[dim + 1]));
						next_dis = get_distance(T, sum_dis, dim, x[dim], x[dim+1])/m_max_distance;
						next_val = get_price(T, sum_dis, dim, x[dim]);
						type_dis[get_cor_id(dim, x[dim])] += next_dis;
						type_val[get_cor_id(dim, x[dim])] += next_dis * next_val;
						cur_dis[dim] = next_dis;
						cur_val[dim] = next_val;
						sum_dis += next_dis;
					}
					for (int cor_id(0); cor_id < type_dis.size(); ++cor_id) {
						if (type_dis[cor_id] >= m_discount[cor_id].first) {
							type_val[cor_id] *= m_discount[cor_id].second;
						}
					}
					Real obj(0);
					for (auto& it : type_val) obj += it;
					return obj;
				}
			}

			Real get_effective_mean_value(int T,const std::vector<int>& x);
			

		    void get_optimal(int time_omega, std::vector<std::pair<int,std::vector<int>>> &sols,Real min_dis);
		   
		};
		class selection_problem: virtual public problem {

		public:
			using solution_type = solution<variable_vector<int>, Real>;
			using variable_type = variable_vector<int>;
		protected:
			map_info m_map;
			int m_T = 0;
			int m_change_fre = 1000;
		public:

			selection_problem(param_map& v) :problem(v.at("proName"), v.at("numDim"), 1)
			{}
			virtual bool is_change() {
				return m_effective_eval % m_change_fre == 0;
			}
			void change() {
				++m_T;
			}

			const map_info& get_map()const {
				return m_map;
			}

			Real get_heuristic_info(const std::vector<int>& cur_x, int dim) {
				return m_map.get_heuristic_info(m_T, cur_x, dim);
			}
			Real get_heuristic_info(int dim,int from, int to) {
				return m_map.get_heuristic_info(m_T, dim, from, to);
			}
			// Í¨¹ý problem ¼Ì³Ð
			virtual bool same(const solution_base& s1, const solution_base& s2) const override;

			virtual Real variable_distance(const solution_base& s1, const solution_base& s2) const override;

			virtual Real variable_distance(const variable_base& s1, const variable_base& s2) const override;

			virtual evaluation_tag evaluate_(solution_base& s, caller call, bool effective, bool initialized) override;

			virtual void initialize_solution(solution_base& s) const override;

			virtual void initialize() override;


			void generate_level_samples(std::vector<solution_type> & samples,caller call);
			
			virtual evaluation_tag evaluate_effective_value(solution_base& s,  caller call, bool effective, bool initialized);
			
			
		};

	}


	using ComOP_DSP = SP::selection_problem;
}



#endif