/*
*  Copyright (C) 2007 Jolien Creighton, John Whelan
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with with program; see the file COPYING. If not, write to the
*  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
*  MA  02111-1307  USA
*/

/**
\author UTB Relativity Group; contact whelan@phys.utb.edu
\file
\ingroup stochastic

\brief A program to test <tt>LALOverlapReductionFunction()</tt>.

\heading{Usage}

\code
./OverlapReductionFunctionTest [options]
Options:
  -h             print usage message
  -q             quiet: run silently
  -v             verbose: print extra information
  -d level       set lalDebugLevel to level
  -s siteID1     calculate overlap red fcn for site siteID1
  -t siteID2       with site siteID2
  -f f0          set start frequency to f0
  -e deltaF      set frequency spacing to deltaF
  -n length      set number of points in frequency series to length
  -o filename    print overlap reduction function to file filename
\endcode

\heading{Description}


This program tests the function <tt>LALOverlapReductionFunction()</tt>, which calculates
the overlap reduction\eqref{stochastic_e_gamma(f)} for a pair of gravitational
wave detectors.

First, it tests that the correct error codes
(cf. \ref StochasticCrossCorrelation.h)
are generated for the following error conditions (tests in
\e italics are not performed if \c LAL_NDEBUG is set, as
the corresponding checks in the code are made using the ASSERT macro):
<ul>
<li> <em>null pointer to parameter structure</em></li>
<li> <em>zero length parameter</em></li>
<li> <em>negative frequency spacing</em></li>
<li> <em>zero frequency spacing</em></li>
<li> <em>null pointer to output series</em></li>
<li> <em>null pointer to data member of output series</em></li>
<li> mismatch between length of output series and length parameter</li>
<li> <em>null pointer to data member of data member of output series</em></li>
<li> negative start frequency</li>
<li> non-symmetric response tensor</li>
</ul>

It then verifies that the correct frequency series are generated for
some simple test cases:
<ol>
<li> coïncident, coäligned interferometers: \f$\gamma(f)=1\f$</li>
<li> coäligned interferometers lying parallel to the \f$x\f$--\f$y\f$ plane
  separated only in \f$z\f$:
  \f$\gamma(f)=\frac{5}{2}\alpha^{-1}\sin\alpha(1-3\alpha^{-2}+3\alpha^{-5})
  +\frac{5}{2}\alpha^{-1}\cos\alpha(2-3\alpha^{-2})\f$, where
  \f$\alpha=2\pi f|\Delta\vec x|/c\f$</li>
<li> completely misaligned interferometers lying parallel to the
  \f$x\f$--\f$y\f$ plane separated only in \f$z\f$: \f$\gamma(f)=0\f$.</li>
</ol>

If the \c filename argument is present, it also calculates a
spectrum based on user-specified data.
Figure\figref{stochastic_LHOLLO}-\figref{stochastic_GEO600TAMA300} illustrates
the output of the overlap reduction function with the corresponding command arguments of each detector.

\floatfig{htbp,stochastic_LHOLLO}
\image html stochasticOverlapLHOLLO.png  "Fig. [stochastic_LHOLLO]: Overlap reduction function for the LHO--LLO pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 0 -t 1 -o LHOLLOOverlap.dat"
\image latex stochasticOverlapLHOLLO.pdf "Overlap reduction function for the LHO--LLO pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 0 -t 1 -o LHOLLOOverlap.dat"

\floatfig{htbp,stochastic_LHOVIRGO}
\image html stochasticOverlapLHOVIRGO.png "Fig. [stochastic_LHOVIRGO]: Overlap reduction function for the LHO--VIRGO pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 0 -t 2 -o LHOVIRGOOverlap.dat"
\image latex stochasticOverlapLHOVIRGO.pdf "Overlap reduction function for the LHO--VIRGO pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 0 -t 2 -o LHOVIRGOOverlap.dat" width=4.5in

\floatfig{htbp,stochastic_LHOGEO600}
\image html  stochasticOverlapLHOGEO600.png "Fig. [stochastic_LHOGEO600]: Overlap reduction function for the LHO--GEO600 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 0 -t 3 -o LHOGEO600Overlap.dat"
\image latex stochasticOverlapLHOGEO600.pdf "Overlap reduction function for the LHO--GEO600 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 0 -t 3 -o LHOGEO600Overlap.dat"  width=4.4in

\floatfig{htbp,stochastic_LHOTAMA300}
\image html stochasticOverlapLHOTAMA300.png "Fig. [stochastic_LHOTAMA300]: Overlap reduction function for the LHO--TAMA300 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 0 -t 4 -o LHOTAMA300Overlap.dat"
\image latex stochasticOverlapLHOTAMA300.pdf "Overlap reduction function for the LHO--TAMA300 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 0 -t 4 -o LHOTAMA300Overlap.dat" width=4.4in

\floatfig{htbp,stochastic_LLOVIRGO}
\image html stochasticOverlapLLOVIRGO.png "Fig. [stochastic_LLOVIRGO]: Overlap reduction function for the LLO--VIRGO pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 1 -t 2 -o LLOVIRGOOverlap.dat"
\image latex stochasticOverlapLLOVIRGO.pdf "Overlap reduction function for the LHO--TAMA300 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 0 -t 4 -o LHOTAMA300Overlap.dat" width=4.4in

\floatfig{htbp,stochastic_LLOGEO600}
\image html stochasticOverlapLLOGEO600.png "Fig. [stochastic_LLOGEO600]: Overlap reduction function for the LLO--GEO600 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 1 -t 3 -o LLOGEO600Overlap.dat"
\image latex stochasticOverlapLLOGEO600.pdf "Overlap reduction function for the LLO--GEO600 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 1 -t 3 -o LLOGEO600Overlap.dat" width=4.4in

\floatfig{htbp,stochastic_LLOTAMA300}
\image html stochasticOverlapLLOTAMA300.png "Fig. [stochastic_LLOTAMA300]: Overlap reduction function for the LLO--TAMA300 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 1 -t 4 -o LLOTAMA300Overlap.dat"
\image latex stochasticOverlapLLOTAMA300.pdf "Overlap reduction function for the LLO--TAMA300 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 1 -t 4 -o LLOTAMA300Overlap.dat" width=4.4in

\floatfig{htbp,stochastic_VIRGOGEO600}
\image html stochasticOverlapVIRGOGEO600.png "Fig. [stochastic_VIRGOGEO600]: Overlap reduction function for the VIRGO--GEO600 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 2 -t 3 -o VIRGOGEO600Overlap.dat"
\image latex stochasticOverlapVIRGOGEO600.pdf "Overlap reduction function for the VIRGO--GEO600 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 2 -t 3 -o VIRGOGEO600Overlap.dat" width=4.4in

\floatfig{htbp,stochastic_VIRGOTAMA300}
\image html stochasticOverlapVIRGOTAMA300.png "Fig. [stochastic_VIRGOTAMA300]: Overlap reduction function for the VIRGO--TAMA300 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 2 -t 4 -o VIRGOTAMA300Overlap.dat"
\image latex stochasticOverlapVIRGOTAMA300.pdf "Overlap reduction function for the VIRGO--TAMA300 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 2 -t 4 -o VIRGOTAMA300Overlap.dat" width=4.4in

\floatfig{htbp,stochastic_GEO600TAMA300}
\image html stochasticOverlapGEO600TAMA300.png "Fig. [stochastic_GEO600TAMA300]: Overlap reduction function for the GEO600--TAMA300 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 3 -t 4 -o GEO600TAMA300Overlap.dat"
\image latex stochasticOverlapGEO600TAMA300.pdf "Overlap reduction function for the GEO600--TAMA300 pair. This was generated by the command: OverlapReductionFunctionTest -e 1 -n 1000 -s 3 -t 4 -o GEO600TAMA300Overlap.dat" width=4.4in

\heading{Uses}

\code
lalDebugLevel
getopt()
LALSCreateVector()
LALOverlapReductionFunction()
LALSPrintFrequencySeries()
LALSDestroyVector()
LALCheckMemoryLeaks()
\endcode

*/

#include <lal/LALStdlib.h>

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <lal/StochasticCrossCorrelation.h>
#include <lal/AVFactories.h>
#include <lal/PrintFTSeries.h>
#include <lal/Units.h>

#include "CheckStatus.h"

NRCSID(OVERLAPREDUCTIONFUNCTIONTESTC, "$Id$");

#define OVERLAPREDUCTIONFUNCTIONTESTC_LENGTH    8
#define OVERLAPREDUCTIONFUNCTIONTESTC_F0        0.0
#define OVERLAPREDUCTIONFUNCTIONTESTC_DELTAF    80.0
#define OVERLAPREDUCTIONFUNCTIONTESTC_TOL       1e-6
#define OVERLAPREDUCTIONFUNCTIONTESTC_Z         5e+6

#define OVERLAPREDUCTIONFUNCTIONTESTC_TRUE     1
#define OVERLAPREDUCTIONFUNCTIONTESTC_FALSE    0

extern char *optarg;
extern int   optind;

/* int lalDebugLevel = LALMSGLVL3; */
extern int lalDebugLevel;
BOOLEAN optVerbose    = OVERLAPREDUCTIONFUNCTIONTESTC_FALSE;
REAL8 optDeltaF     = -1;
UINT4 optLength     = 0;
REAL8 optF0       = 0.0;
UINT4  optDetector1 = LALNumCachedDetectors;
UINT4  optDetector2 = LALNumCachedDetectors;
CHAR optFile[LALNameLength] = "";

static void
Usage (const char *program, int exitflag);

static void
ParseOptions (int argc, char *argv[]);

/**\name Error Codes */ /*@{*/
#define OVERLAPREDUCTIONFUNCTIONTESTC_ENOM 0
#define OVERLAPREDUCTIONFUNCTIONTESTC_EARG 1
#define OVERLAPREDUCTIONFUNCTIONTESTC_ECHK 2
#define OVERLAPREDUCTIONFUNCTIONTESTC_EFLS 3
#define OVERLAPREDUCTIONFUNCTIONTESTC_EUSE 4
#define OVERLAPREDUCTIONFUNCTIONTESTC_MSGENOM "Nominal exit"
#define OVERLAPREDUCTIONFUNCTIONTESTC_MSGEARG "Error parsing command-line arguments"
#define OVERLAPREDUCTIONFUNCTIONTESTC_MSGECHK "Error checking failed to catch bad data"
#define OVERLAPREDUCTIONFUNCTIONTESTC_MSGEFLS "Incorrect answer for valid data"
#define OVERLAPREDUCTIONFUNCTIONTESTC_MSGEUSE "Bad user-entered data"
/*@}*/

int main( int argc, char *argv[] )
{
  static LALStatus                status;

  OverlapReductionFunctionParameters   parameters;
  REAL4FrequencySeries     overlap;

  REAL4FrequencySeries     dummyOutput;

  const REAL4 expectedOutputDataData[8] = {1.0, .2113956922,
					   -.1372693019, .9606085997e-2,
					   .6120349226e-1, -.5375432760e-1,
					   .3700960588e-2, .3519147931e-1};
  LALDetectorPair     detectors;
  const LALDetector         plusAtOrigin  = { {0.0, 0.0, 0.0},
					      { {0.5, 0.0, 0.0},
						{0.0,-0.5, 0.0},
						{0.0, 0.0, 0.0} },
					      LALDETECTORTYPE_ABSENT,
					      { "", "", 0, 0, 0, 0, 0, 0, 0, 0, 0}
                                             };
  /*   LALDetector         crossAtOrigin = { {0.0, 0.0, 0.0},
                                        { {0.0, 0.5, 0.0},
                                          {0.5, 0.0, 0.0},
                                          {0.0, 0.0, 0.0} },
                                       LALDETECTORTYPE_ABSENT,
					      { "", 0, 0, 0, 0, 0, 0, 0}
                                             };
  */
  const LALDetector         plusOnZAxis   = { {0.0, 0.0, OVERLAPREDUCTIONFUNCTIONTESTC_Z},
					      { {0.5, 0.0, 0.0},
						{0.0,-0.5, 0.0},
						{0.0, 0.0, 0.0} },
					      LALDETECTORTYPE_ABSENT,
					      { "", "", 0, 0, 0, 0, 0, 0, 0, 0, 0}
                                             };
  const LALDetector         crossOnZAxis  = { {0.0, 0.0, OVERLAPREDUCTIONFUNCTIONTESTC_Z},
					      { {0.0, 0.5, 0.0},
						{0.5, 0.0, 0.0},
						{0.0, 0.0, 0.0} },
					      LALDETECTORTYPE_ABSENT,
					      { "", "", 0, 0, 0, 0, 0, 0, 0, 0, 0}
                                             };
  UINT4 i;
  REAL4 overlapVal, f;
  INT4 code;

  lalDebugLevel = LALNDEBUG;

  /* define valid parameters */

  parameters.length   = OVERLAPREDUCTIONFUNCTIONTESTC_LENGTH;
  parameters.f0       = OVERLAPREDUCTIONFUNCTIONTESTC_F0;
  parameters.deltaF   = OVERLAPREDUCTIONFUNCTIONTESTC_DELTAF;

  overlap.data = NULL;

  dummyOutput.data = NULL;

  detectors.detectorOne = detectors.detectorTwo = plusAtOrigin;

  ParseOptions( argc, argv );

  LALSCreateVector(&status, &(overlap.data), OVERLAPREDUCTIONFUNCTIONTESTC_LENGTH);
  if ( ( code = CheckStatus(&status, 0 , "",
			    OVERLAPREDUCTIONFUNCTIONTESTC_EFLS,
			    OVERLAPREDUCTIONFUNCTIONTESTC_MSGEFLS) ) )
  {
    return code;
  }

  /* TEST INVALID DATA HERE ------------------------------------------ */
#ifndef LAL_NDEBUG
  if ( ! lalNoDebug )
  {
    /* test behavior for null pointer to real frequency series for output */
    LALOverlapReductionFunction(&status, NULL, &detectors, &parameters);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      OVERLAPREDUCTIONFUNCTIONTESTC_ECHK,
			      OVERLAPREDUCTIONFUNCTIONTESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to output series results in error:       \n\"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);

    /* test behavior for null pointer to input structure */
    LALOverlapReductionFunction(&status, &overlap, NULL, &parameters);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      OVERLAPREDUCTIONFUNCTIONTESTC_ECHK,
			      OVERLAPREDUCTIONFUNCTIONTESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to input structure results in error:       \n\"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);


    /* test behavior for null pointer to parameter structure */
    LALOverlapReductionFunction(&status, &overlap, &detectors, NULL);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      OVERLAPREDUCTIONFUNCTIONTESTC_ECHK,
			      OVERLAPREDUCTIONFUNCTIONTESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to parameter structure results in error:       \n\"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);


    /* test behavior for null pointer to data member of real frequency
       series for output */
    LALOverlapReductionFunction(&status, &dummyOutput, &detectors, &parameters);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      OVERLAPREDUCTIONFUNCTIONTESTC_ECHK,
			      OVERLAPREDUCTIONFUNCTIONTESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to data member of output series results in error:       \n\"%s\"\n",
           STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);


    /* Create a vector for testing null data-data pointer */
    LALSCreateVector(&status, &(dummyOutput.data), OVERLAPREDUCTIONFUNCTIONTESTC_LENGTH);
    if ( ( code = CheckStatus(&status, 0 , "",
			      OVERLAPREDUCTIONFUNCTIONTESTC_EFLS,
			      OVERLAPREDUCTIONFUNCTIONTESTC_MSGEFLS) ) )
    {
      return code;
    }
    REAL4                *tempPtr;
    tempPtr = dummyOutput.data->data;
    dummyOutput.data->data = NULL;

    /* test behavior for null pointer to data member of data member of
       real frequency series for output */
    LALOverlapReductionFunction(&status, &dummyOutput, &detectors, &parameters);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENULLPTR,
			      STOCHASTICCROSSCORRELATIONH_MSGENULLPTR,
			      OVERLAPREDUCTIONFUNCTIONTESTC_ECHK,
			      OVERLAPREDUCTIONFUNCTIONTESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: null pointer to data member of data member of output series results in error:       \n\"%s\"\n",
           STOCHASTICCROSSCORRELATIONH_MSGENULLPTR);

    /* clean up */

    dummyOutput.data->data = tempPtr;
    LALSDestroyVector(&status, &(dummyOutput.data));
    if ( ( code = CheckStatus(&status, 0 , "",
			      OVERLAPREDUCTIONFUNCTIONTESTC_EFLS,
			      OVERLAPREDUCTIONFUNCTIONTESTC_MSGEFLS) ) )
    {
      return code;
    }

    /* test behavior for length parameter equal to zero */
    parameters.length = 0;
    LALOverlapReductionFunction(&status, &overlap, &detectors, &parameters);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_EZEROLEN,
			      STOCHASTICCROSSCORRELATIONH_MSGEZEROLEN,
			      OVERLAPREDUCTIONFUNCTIONTESTC_ECHK,
			      OVERLAPREDUCTIONFUNCTIONTESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: zero length parameter results in error:       \n\"%s\"\n",
	   STOCHASTICCROSSCORRELATIONH_MSGEZEROLEN);
    /* assign valid length parameter */
    parameters.length = OVERLAPREDUCTIONFUNCTIONTESTC_LENGTH;

    /* test behavior for frequency spacing less than or equal to zero */
    parameters.deltaF = -1;
    LALOverlapReductionFunction(&status, &overlap, &detectors, &parameters);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENONPOSDELTAF,
			      STOCHASTICCROSSCORRELATIONH_MSGENONPOSDELTAF,
			      OVERLAPREDUCTIONFUNCTIONTESTC_ECHK,
			      OVERLAPREDUCTIONFUNCTIONTESTC_MSGECHK) ) )
    {
      return code;
    }
    printf("  PASS: negative frequency spacing results in error:       \n\"%s\"\n",
	   STOCHASTICCROSSCORRELATIONH_MSGENONPOSDELTAF);

    parameters.deltaF = 0;
    LALOverlapReductionFunction(&status, &overlap, &detectors, &parameters);
    if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENONPOSDELTAF,
			      STOCHASTICCROSSCORRELATIONH_MSGENONPOSDELTAF,
			      OVERLAPREDUCTIONFUNCTIONTESTC_ECHK,
			      OVERLAPREDUCTIONFUNCTIONTESTC_MSGECHK) ) )
    {
        return code;
    }
    printf("  PASS: zero frequency spacing results in error:       \n\"%s\"\n",
	   STOCHASTICCROSSCORRELATIONH_MSGENONPOSDELTAF);
    /* assign valid frequency spacing */
    parameters.deltaF = OVERLAPREDUCTIONFUNCTIONTESTC_DELTAF;
  }

#endif /* LAL_NDEBUG */

  /* test behavior for negative start frequency */
  parameters.f0 = -20.0;
  LALOverlapReductionFunction(&status, &overlap, &detectors, &parameters);
  if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_ENEGFMIN,
			    STOCHASTICCROSSCORRELATIONH_MSGENEGFMIN,
			    OVERLAPREDUCTIONFUNCTIONTESTC_ECHK,
			    OVERLAPREDUCTIONFUNCTIONTESTC_MSGECHK) ) )
  {
    return code;
  }
  printf("  PASS: negative start frequency results in error:\n       \"%s\"\n",
         STOCHASTICCROSSCORRELATIONH_MSGENEGFMIN);

  /* reassign valid start frequency */
  parameters.f0 = OVERLAPREDUCTIONFUNCTIONTESTC_F0;

  /* test behavior for length of data member of real frequency series
     for output not equal to length specified in input parameters */
  parameters.length += 1;
  LALOverlapReductionFunction(&status, &overlap, &detectors, &parameters);
  if ( ( code = CheckStatus(&status, STOCHASTICCROSSCORRELATIONH_EMMLEN,
			    STOCHASTICCROSSCORRELATIONH_MSGEMMLEN,
			    OVERLAPREDUCTIONFUNCTIONTESTC_ECHK,
			    OVERLAPREDUCTIONFUNCTIONTESTC_MSGECHK) ) )
  {
    return code;
  }
  printf("  PASS: mismatch between length of output series and length parameter results in error:       \n\"%s\"\n", STOCHASTICCROSSCORRELATIONH_MSGEMMLEN);
  /* reassign valid length to data member of dummy output */
  parameters.length -= 1;

  /* TEST VALID DATA HERE -------------------------------------------- */


  /* generate overlap red fcn */
  LALOverlapReductionFunction(&status, &overlap, &detectors, &parameters);
  if ( ( code = CheckStatus(&status,0, "",
			    OVERLAPREDUCTIONFUNCTIONTESTC_EFLS,
			    OVERLAPREDUCTIONFUNCTIONTESTC_MSGEFLS) ) )
  {
    return code;
  }

  /* test values */

  overlapVal = 1.0;
  for (i=0; i<OVERLAPREDUCTIONFUNCTIONTESTC_LENGTH; ++i)
  {
    f = i * OVERLAPREDUCTIONFUNCTIONTESTC_DELTAF;
    if (optVerbose) {
      printf("gamma(%f Hz)=%f, should be %f\n",
             f, overlap.data->data[i], overlapVal);
    }
    if ( (overlap.data->data[i] - overlapVal) &&
         abs((overlap.data->data[i] - overlapVal)/overlapVal)
         > OVERLAPREDUCTIONFUNCTIONTESTC_TOL )
    {
      printf("  FAIL: Valid data test #1 (coincident, coaligned IFOs)\n");
      return OVERLAPREDUCTIONFUNCTIONTESTC_EFLS;
    }
  }
  printf("  PASS: Valid data test #1 (coincident, coaligned IFOs)\n");

  /* change parameters */
  detectors.detectorTwo = plusOnZAxis;

  /* generate overlap red fcn */
  LALOverlapReductionFunction(&status, &overlap, &detectors, &parameters);
  if ( ( code = CheckStatus(&status,0, "",
			    OVERLAPREDUCTIONFUNCTIONTESTC_EFLS,
			    OVERLAPREDUCTIONFUNCTIONTESTC_MSGEFLS) ) )
  {
    return code;
  }

  /* test values */

  for (i=0; i<OVERLAPREDUCTIONFUNCTIONTESTC_LENGTH; ++i)
  {
    f = i * OVERLAPREDUCTIONFUNCTIONTESTC_DELTAF;
    if (optVerbose) {
      printf("gamma(%f Hz)=%f, should be %f\n",
             f, overlap.data->data[i], expectedOutputDataData[i]);
    }
    if ( (overlap.data->data[i] - expectedOutputDataData[i]) &&
         abs((overlap.data->data[i] - expectedOutputDataData[i])
	     / expectedOutputDataData[i])
	 > OVERLAPREDUCTIONFUNCTIONTESTC_TOL )
    {
      printf("  FAIL: Valid data test #2 (coaligned, non-coincident IFOs)\n");
      return OVERLAPREDUCTIONFUNCTIONTESTC_EFLS;
    }
  }
  printf("  PASS: Valid data test #2 (coaligned, non-coincident IFOs)\n");
  /* change parameters */
  detectors.detectorTwo = crossOnZAxis;

  /* generate overlap red fcn */
  LALOverlapReductionFunction(&status, &overlap, &detectors, &parameters);
  if ( ( code = CheckStatus(&status,0, "",
			    OVERLAPREDUCTIONFUNCTIONTESTC_EFLS,
			    OVERLAPREDUCTIONFUNCTIONTESTC_MSGEFLS) ) )
  {
    return code;
  }

  /* test values */

  overlapVal = 0.0;
  for (i=0; i<OVERLAPREDUCTIONFUNCTIONTESTC_LENGTH; ++i)
  {
    f = i * OVERLAPREDUCTIONFUNCTIONTESTC_DELTAF;
    if (optVerbose) {
      printf("gamma(%f Hz)=%f, should be %f\n",
             f, overlap.data->data[i], overlapVal);
    }
    if ( (overlap.data->data[i] - overlapVal) &&
         abs(overlap.data->data[i] - overlapVal)
	 > OVERLAPREDUCTIONFUNCTIONTESTC_TOL )
    {
      printf("  FAIL: Valid data test #3 (misaligned IFOs)\n");
      return OVERLAPREDUCTIONFUNCTIONTESTC_EFLS;
    }
  }
  printf("  PASS: Valid data test #3 (misaligned IFOs)\n");

  /* change parameters */
  /*
     detectors.detectorTwo = plusOnZAxis;
   */

  /* clean up valid data */
  LALSDestroyVector(&status, &(overlap.data));
  if ( ( code = CheckStatus(&status,0, "",
			    OVERLAPREDUCTIONFUNCTIONTESTC_EFLS,
			    OVERLAPREDUCTIONFUNCTIONTESTC_MSGEFLS) ) )
  {
    return code;
  }

  LALCheckMemoryLeaks();

  printf("PASS: all tests\n");

  if (optFile[0]) {
    parameters.length = optLength;
    parameters.f0 = optF0;
    parameters.deltaF = optDeltaF;

    if (optDetector1 < LALNumCachedDetectors)
    {
      detectors.detectorOne = lalCachedDetectors[optDetector1];
    }
    else {
      return OVERLAPREDUCTIONFUNCTIONTESTC_EUSE;
    }

    if (optDetector2 < LALNumCachedDetectors)
    {
      detectors.detectorTwo = lalCachedDetectors[optDetector2];
    }
    else {
      return OVERLAPREDUCTIONFUNCTIONTESTC_EUSE;
    }

    LALSCreateVector(&status, &(overlap.data), optLength);
    if ( ( code = CheckStatus(&status, 0 , "",
			      OVERLAPREDUCTIONFUNCTIONTESTC_EUSE,
			      OVERLAPREDUCTIONFUNCTIONTESTC_MSGEUSE) ) )
    {
      return code;
    }
    LALOverlapReductionFunction(&status, &overlap, &detectors, &parameters);
    if ( ( code = CheckStatus(&status, 0 , "",
			      OVERLAPREDUCTIONFUNCTIONTESTC_EUSE,
			      OVERLAPREDUCTIONFUNCTIONTESTC_MSGEUSE) ) )
    {
      return code;
    }
    LALSPrintFrequencySeries( &overlap, optFile );

    printf("======== Overlap Reduction Function Written to File %s ========\n", optFile);

    LALSDestroyVector(&status, &(overlap.data));
    if ( ( code = CheckStatus(&status, 0 , "",
			      OVERLAPREDUCTIONFUNCTIONTESTC_EUSE,
			      OVERLAPREDUCTIONFUNCTIONTESTC_MSGEUSE) ) )
    {
      return code;
    }

    LALCheckMemoryLeaks();
  }

  return OVERLAPREDUCTIONFUNCTIONTESTC_ENOM;

}


/*
 * Usage ()
 *
 * Prints a usage message for program program and exits with code exitcode.
 *
 */
static void
Usage (const char *program, int exitcode)
{
  INT4 i;

  fprintf (stderr, "Usage: %s [options]\n", program);
  fprintf (stderr, "Options:\n");
  fprintf (stderr, "  -h             print this message\n");
  fprintf (stderr, "  -q             quiet: run silently\n");
  fprintf (stderr, "  -v             verbose: print extra information\n");
  fprintf (stderr, "  -d level       set lalDebugLevel to level\n");
  fprintf (stderr, "  -s siteID1     calculate overlap red fcn for site siteID1\n");
  fprintf (stderr, "  -t siteID2       with site siteID2\n");
  for (i=0; i<LALNumCachedDetectors; ++i)
  {
    fprintf (stderr, "                   %d = %s\n",
             i, lalCachedDetectors[i].frDetector.name);
  }
  fprintf (stderr, "  -f f0          set start frequency to f0\n");
  fprintf (stderr, "  -e deltaF      set frequency spacing to deltaF\n");
  fprintf (stderr, "  -n length      set number of points in frequency series to length\n");
  fprintf (stderr, "  -o filename    print overlap reduction function to file filename\n");
  exit (exitcode);
}

/*
 * ParseOptions ()
 *
 * Parses the argc - 1 option strings in argv[].
 *
 */
static void
ParseOptions (int argc, char *argv[])
{
  while (1)
  {
    int c = -1;

    c = getopt (argc, argv, "hqvd:s:t:f:e:n:o:");
    if (c == -1)
    {
      break;
    }

    switch (c)
    {
      case 'o': /* specify output file */
        strncpy (optFile, optarg, LALNameLength);
        break;

      case 'n': /* specify number of points in frequency series */
        optLength = atoi (optarg);
        break;

      case 'e': /* specify frequency resolution */
        optDeltaF = atof (optarg);
        break;

      case 'f': /* specify start frequency */
        optF0 = atof (optarg);
        break;

      case 's': /* specify detector #1 */
        optDetector1 = atoi (optarg);
	break;

      case 't': /* specify detector #2 */
        optDetector2 = atoi (optarg);
	break;

      case 'd': /* set debug level */
        lalDebugLevel = atoi (optarg);
        break;

      case 'v': /* optVerbose */
        optVerbose = OVERLAPREDUCTIONFUNCTIONTESTC_TRUE;
        break;

      case 'q': /* quiet: run silently (ignore error messages) */
        freopen ("/dev/null", "w", stderr);
        freopen ("/dev/null", "w", stdout);
        break;

      case 'h':
        Usage (argv[0], 0);
        break;

      default:
        Usage (argv[0], 1);
    }

  }

  if (optind < argc)
  {
    Usage (argv[0], 1);
  }

  return;
}
