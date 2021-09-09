#include "./INCLUDE/Sequence.h"
#include "./INCLUDE/Segment.h"
#include "./INCLUDE/LKH.h"
/*
 * This file contains the functions FindPermutation and FeasibleKOptMove.
 */

/*  
 * The FindPermutation function finds the permutation p[1:2k] corresponding 
 * to the sequence in which the nodes t[1:2k] occur on the tour.
 *   
 * The nodes are sorted using qsort. The BETWEEN function is used 
 * as comparator.
 *   
 * Postcondition:
 *   
 *     BETWEEN(t[p[i-1]], t[p[i]], t[p[i+1]]) for i = 2, ..., 2k-1
 */

//LKHAlg::Node **t;       /* The sequence of nodes to be used in a move */
//LKHAlg::Node **T;       /* The currently best t's */
//LKHAlg::Node **tSaved;  /* For saving t when using the BacktrackKOptMove function */
//int *p;         /* The permutation corresponding to the sequence in which
//	 the t's occur on the tour */
//int *q;         /* The inverse permutation of p */
//int *incl;      /* Array: incl[i] == j, if (t[i], t[j]) is an inclusion edge */
//int *cycle;     /* Array: cycle[i] is cycle number of t[i] */
//GainType *G;    /* For storing the G-values in the BestKOptMove function */
//int K;          /* The value K for the current K-opt move */
//
//static LKHAlg::Node *tp1;
namespace LKH {
	thread_local LKHAlg::Node **t = 0;       /* The sequence of nodes to be used in a move */
	thread_local LKHAlg::Node **T = 0;       /* The currently best t's */
	thread_local LKHAlg::Node **tSaved = 0;  /* For saving t when using the BacktrackKOptMove function */
	thread_local int *p = 0;         /* The permutation corresponding to the sequence in which
					   the t's occur on the tour */
	thread_local int *q = 0;         /* The inverse permutation of p */
	thread_local int *incl = 0;      /* Array: incl[i] == j, if (t[i], t[j]) is an inclusion edge */
	thread_local int *cycle = 0;     /* Array: cycle[i] is cycle number of t[i] */
	thread_local GainType *G = 0;    /* For storing the G-values in the BestKOptMove function */
	thread_local int K;          /* The value K for the current K-opt move */

	static thread_local LKHAlg::Node *tp1 = 0;

	//static thread_local unique_ptr<LKHAlg::Node> tp1;
	//thread_local unique_ptr<vector<LKHAlg::Node *> > t;       /* The sequence of nodes to be used in a move */
	//thread_local unique_ptr<vector<LKHAlg::Node *> > T;       /* The sequence of nodes to be used in a move */
	//thread_local unique_ptr<vector<LKHAlg::Node *> > tSaved;       /* The sequence of nodes to be used in a move */
	//thread_local unique_ptr<vector<int> > p;         /* The permutation corresponding to the sequence in which the t's occur on the tour */
	//thread_local unique_ptr<vector<int> > q;         /* The inverse permutation of p */
	//thread_local unique_ptr<vector<int> > incl;      /* Array: incl[i] == j, if (t[i], t[j]) is an inclusion edge */
	//thread_local unique_ptr<vector<int> > cycle;     /* Array: cycle[i] is cycle number of t[i] */
	//thread_local unique_ptr<vector<GainType> > G;    /* For storing the G-values in the BestKOptMove function */
	//thread_local unique_ptr<int> K;          /* The value K for the current K-opt move */

	void LKHAlg::freeSequence()
	{
		free(tp1);
		tp1 = 0;
	}


	static int compare(const void *pa, const void *pb)
	{
		//return BETWEEN(tp1, t[*(int *) pa], t[*(int *) pb]) ? -1 : 1;//
		//LKHAlg *Alg=0;
		//dynamic_cast<LKHAlg*>(LKHAlg::mp_algorithm.get());
		//LKHALG
		return lkh_ptr->Between_SL(tp1, t[*(int *)pa], t[*(int *)pb]) ? -1 : 1;
	}

	void FindPermutation(int k, LKHAlg *Alg)
	{
		//lkh_ptr = Alg;
		int i, j;

		for (i = j = 1; j <= k; i += 2, j++)
			p[j] = (Alg->Reversed == (t[i])->Parent->Reversed ? (t[i])->Suc : (t[i])->Pred) == t[i + 1] ? i : i + 1;//(Reversed == (t[i])->Parent->Reversed ? (t[i])->Suc : (t[i])->Pred)
		tp1 = t[p[1]];
		qsort(p + 2, k - 1, sizeof(int), compare);
		for (j = 2 * k; j >= 2; j -= 2) {
			p[j - 1] = i = p[j / 2];
			p[j] = i & 1 ? i + 1 : i - 1;
		}
		for (i = 1; i <= 2 * k; i++)
			q[p[i]] = i;
	}

	/*
	 * The FeasibleKOptMove function tests whether the move given by
	 * t[1..2k] and incl[1..2k] represents a feasible k-opt move,
	 * i.e., making the move on the current tour will result in a tour.
	 *
	 * In that case, 1 is returned. Otherwise, 0 is returned.
	 */

	int FeasibleKOptMove(int k)
	{
		int Count, i;

		FindPermutation(k, lkh_ptr);
		for (Count = 1, i = 2 * k; (i = q[incl[p[i]]] ^ 1); Count++);
		return Count == k;
	}

	/*
	 * The Cycles function returns the number of cycles that would appear if
	 * the move given by t[1..2k] and incl[1..2k] was made.
	 * In addition, cycle[i] is assigned the number of the cycle that node t[i]
	 * is a part of (an integer from 1 to Cycles).
	 */

	int Cycles(int k)
	{
		int i, j, Count = 0;

		for (i = 1; i <= 2 * k; i++)
			cycle[i] = 0;
		for (i = 1; i <= 2 * k; i++) {
			if (!cycle[p[i]]) {
				Count++;
				j = i;
				do {
					cycle[p[j]] = Count;
					j = q[incl[p[j]]];
					cycle[p[j]] = Count;
					if ((j ^= 1) > 2 * k)
						j = 1;
				} while (j != i);
			}
		}
		return Count;
	}

	/*
	 * The Added function is used to test if an edge, (ta,tb),
	 * has been added in the submove under construction.
	 */

	int Added(const LKHAlg::Node * ta, const LKHAlg::Node * tb)
	{
		return ta->Added1 == tb || ta->Added2 == tb;
	}

	/*
	 * The Deleted function is used to test if an edge, (ta,tb),
	 * of the tour has been deleted in the submove under construction.
	 */

	int Deleted(const LKHAlg::Node * ta, const LKHAlg::Node * tb)
	{
		return ta->Deleted1 == tb || ta->Deleted2 == tb;
	}

	/*
	 * The MarkAdded function is used mark an edge, (ta,tb), as added
	 * in the submove under construction.
	 */

	void MarkAdded(LKHAlg::Node * ta, LKHAlg::Node * tb)
	{
		if (!ta->Added1)
			ta->Added1 = tb;
		else if (!ta->Added2)
			ta->Added2 = tb;
		if (!tb->Added1)
			tb->Added1 = ta;
		else if (!tb->Added2)
			tb->Added2 = ta;
	}

	/*
	 * The MarkDeletedfunction is used to mark an edge, (ta,tb), as deleted
	 * in the submove under construction.
	 */

	void MarkDeleted(LKHAlg::Node * ta, LKHAlg::Node * tb)
	{
		if (!ta->Deleted1)
			ta->Deleted1 = tb;
		else if (!ta->Deleted2)
			ta->Deleted2 = tb;
		if (!tb->Deleted1)
			tb->Deleted1 = ta;
		else if (!tb->Deleted2)
			tb->Deleted2 = ta;
	}

	/*
	 * The UnmarkAdded function is used mark the edge, (ta,tb), as not
	 * added.
	 */

	void UnmarkAdded(LKHAlg::Node * ta, LKHAlg::Node * tb)
	{
		if (ta->Added1 == tb)
			ta->Added1 = 0;
		else if (ta->Added2 == tb)
			ta->Added2 = 0;
		if (tb->Added1 == ta)
			tb->Added1 = 0;
		else if (tb->Added2 == ta)
			tb->Added2 = 0;
	}

	/*
	 * The UnmarkDeleted function is used mark the edge, (ta,tb), as not
	 * deleted.
	 */

	void UnmarkDeleted(LKHAlg::Node * ta, LKHAlg::Node * tb)
	{
		if (ta->Deleted1 == tb)
			ta->Deleted1 = 0;
		else if (ta->Deleted2 == tb)
			ta->Deleted2 = 0;
		if (tb->Deleted1 == ta)
			tb->Deleted1 = 0;
		else if (tb->Deleted2 == ta)
			tb->Deleted2 = 0;
	}
}