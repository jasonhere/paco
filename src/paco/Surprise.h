/* This file is part of PACO-PArtitioning Clustering Optimization a program
* to find network partitions using modular solvers and quality functions.
*
*  Copyright (C) 2016 Carlo Nicolini <carlo.nicolini@iit.it>
*
*  PACO is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 3 of the License, or (at your option) any later version.
*
*  Alternatively, you can redistribute it and/or
*  modify it under the terms of the GNU General Public License as
*  published by the Free Software Foundation; either version 2 of
*  the License, or (at your option) any later version.
*
*  PACO is distributed in the hope that it will be useful, but WITHOUT ANY
*  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
*  FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License and a copy of the GNU General Public License along with
*  PACO. If not, see <http://www.gnu.org/licenses/>.
*
* If you use PACO for you publication please cite:
*
* "Modular structure of brain functional networks: breaking the resolution limit by Surprise"
* C. Nicolini and A. Bifone, Scientific Reports
* doi:10.1038/srep19250
*
* "Community detection in weighted brain connectivity networks beyond the resolution limit", 
* C.Nicolini. C.Bordier, A.Bifone, arxiv 1609.04316
* https://arxiv.org/abs/1609.04316
*/



/****************************************************************************
* Copyright (C) 2012 Rodrigo Aldecoa and Ignacio Marín *
* *
* This program is free software: you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation, either version 3 of the License, or *
* (at your option) any later version. *
* *
* This program is distributed in the hope that it will be useful, *
* but WITHOUT ANY WARRANTY; without even the implied warranty of *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the *
* GNU General Public License for more details. *
* *
* You should have received a copy of the GNU General Public License *
* along with this program. If not, see <http://www.gnu.org/licenses/>. *
* *
* Contact info: Rodrigo Aldecoa <raldecoa@ibv.csic.es> *
****************************************************************************/
/*
If you use this program, please cite:
Aldecoa R, Marín I (2011)
Deciphering network community structure by Surprise
PLoS ONE 6(9): e24195
*/

#ifndef SURPRISE_H
#define SURPRISE_H

bool checkArguments(const long p, const long pi,
                    const long m, const long mi);

/**
 * @brief computeSurprise Receives the four parameters p, pi, m and mi and returns the value of Surprise
 * @param p
 * @param pi
 * @param m
 * @param mi
 * @return
 */
long double computeSurprise(const long p, const long pi,
                            const long m, const long mi);

/**
 * @brief computeAsymptoticSurprise
 * @param p
 * @param pi
 * @param m
 * @param mi
 * @return
 */
long double computeAsymptoticSurprise(const long p, const long pi, const long m, const long mi);

/**
 * @brief logHyperProbability Computes one term of the summation, (a single hypergeometric probability)
 * @param F
 * @param M
 * @param n
 * @param j
 * @return
 */
long double logHyperProbability(const long& F, const long& M, const long& n, const long& j);

/**
 * @brief logC Computes log(n k)-logarithm of a binomial coefficient
 * @param n
 * @param k
 * @return
 */
long double logC(const long& n, const long& k);

/**
 * @brief sumRange Function needed to simplify the division of factorials
 * @param min
 * @param max
 * @return
 */
long double sumRange(const long& min, const long& max);

/**
 * @brief sumFactorial Computes log(n!)
 * @param n
 * @return
 */
long double sumFactorial(const long& n);

/**
 * @brief sumLogProbabilities Computes the sum of the past and current terms of the cumulative summation
 * @param nextLogP
 * @param logP
 * @return
 */
bool sumLogProbabilities(const long double& nextLogP, long double& logP);

/**
 * @brief computeConditionedSurprise
 * @param p
 * @param pi
 * @param m
 * @param mi
 * @param ni
 * @return
 */
long double computeConditionedSurprise(const long p, const long pi,
                            const long m, const long mi, const long ni);

/**
 * @brief cumulativeHyperGeometricDistribution
 * @param totalBallsInUrn
 * @param whiteBallsInUrn
 * @param drawnBalls
 * @param drawnWhiteBalls
 * @return
 */
long double cumulativeHyperGeometricDistribution(const long totalBallsInUrn, const long whiteBallsInUrn, const long drawnBalls, const long drawnWhiteBalls);
#endif
