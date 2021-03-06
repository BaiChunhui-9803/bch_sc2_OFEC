#ifndef _GENETIC_H
#define _GENETIC_H
#include "LKH.h"
/*
 * This header specifies the interface for the genetic algorithm part of LKH.
 */
namespace LKH {
	typedef void(*CrossoverFunction) (LKH::LKHAlg *Alg);
	extern thread_local int MaxPopulationSize; /* The maximum size of the population */
	extern thread_local int PopulationSize;    /* The current size of the population */

	extern thread_local CrossoverFunction Crossover;

	extern thread_local int **Population;      /* Array of individuals (solution tours) */
	extern thread_local GainType *PenaltyFitness;  /* The f itnesslty  (tour penalty) of each
	i                             individual */
	extern thread_local GainType *Fitness;     /* The fitness (tour cost) of each individual */

	//extern thread_local unique_ptr<int> MaxPopulationSize; /* The maximum size of the population */
	//extern thread_local unique_ptr<int> PopulationSize;    /* The current size of the population */
	//extern thread_local unique_ptr<CrossoverFunction> Crossover;
	//extern thread_local unique_ptr<int*> Population;      /* Array of individuals (solution tours) */
	//extern thread_local unique_ptr<GainType> Fitness;     /* The fitness (tour cost) of each individual */
	//extern thread_local unique_ptr<GainType*> PenaltyFitness;     /* The fitness (tour cost) of each individual */

#ifdef __cplusplus
	extern "C" {
#endif
#define SmallerFitness(Penalty, Cost, i)\
    (((Penalty) < PenaltyFitness[i]) ||\
     ((Penalty) == PenaltyFitness[i] && (Cost) < Fitness[i]))

#define LargerFitness(Penalty, Cost, i)\
    (((Penalty) > PenaltyFitness[i]) ||\
     ((Penalty) == PenaltyFitness[i] && (Cost) > Fitness[i]))



		void AddToPopulation(GainType Penalty, GainType Cost, LKH::LKHAlg *Alg);
		void ApplyCrossover(int i, int j, LKH::LKHAlg *Alg);
		void FreePopulation();
		int HasFitness(GainType Penalty, GainType Cost);
		int LinearSelection(int Size, double Bias, LKH::LKHAlg *Alg);
		GainType MergeTourWithIndividual(int i, LKH::LKHAlg *Alg);
		void PrintPopulation(LKH::LKHAlg *Alg);
		void ReplaceIndividualWithTour(int i, GainType Penalty, GainType Cost, LKH::LKHAlg *Alg);
		int ReplacementIndividual(GainType Penalty, GainType Cost, LKH::LKHAlg *Alg);

		void ERXT(LKH::LKHAlg *Alg);
#ifdef __cplusplus
	}
#endif
}
#endif
