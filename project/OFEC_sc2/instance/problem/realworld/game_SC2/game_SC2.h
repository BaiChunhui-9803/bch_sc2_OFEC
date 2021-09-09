//realworld problem game_SC2

/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Chunhui Bai
* Email: bch980311@163.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://gitlab.com/OFEC/console for more information
*
*********************************************************************************/
// updated July 19, 2021 by Chunhui Bai


#ifndef OFEC_GAME_SC2_H
#define OFEC_GAME_SC2_H

#include "../../../../core/problem/optima.h"
#include "../../../../core/problem/problem.h"

namespace OFEC {

#define CAST_GAME_SC2 dynamic_cast<GAME_SC2::Game_sc2&>(global::ms_global->m_problem.get())

    namespace GAME_SC2 {


        class Game_sc2 : public Problem{

        protected:




        public:





        private:










        };













    }










}

#endif // !OFEC_GAME_SC2_H