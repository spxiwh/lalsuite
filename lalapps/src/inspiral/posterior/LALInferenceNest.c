/*
 *  InferenceNest.c:  Nested Sampling using LALInference
 *
 *  Copyright (C) 2009 Ilya Mandel, Vivien Raymond, Christian Roever, Marc van der Sluys and John Veitch
 *
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


#include <stdio.h>
#include <lal/Date.h>
#include <lal/GenerateInspiral.h>
#include <lal/LALInference.h>
#include <lal/FrequencySeries.h>
#include <lal/Units.h>
#include <lal/StringInput.h>
#include <lal/LIGOLwXMLInspiralRead.h>
#include <lal/TimeSeries.h>
#include <lalapps.h>
#include <lal/LALInferenceNestedSampler.h>
#include <lal/LALInferencePrior.h>
#include <lal/LALInferenceReadData.h>
#include <lal/LALInferenceLikelihood.h>
#include <lal/LALInferenceTemplate.h>
#include <lal/LALInferenceProposal.h>
#include <lal/LALInferenceInit.h>
#include <lal/LALInferenceCalibrationErrors.h>
#include <LALAppsVCSInfo.h>


/*************** MAIN **********************/


int main(int argc, char *argv[]){
  char help[]="\
  LALInferenceNest:\n\
  Bayesian analysis tool using Nested Sampling algorithm\n\
  for CBC analysis. Uses LALInference library for back-end.\n\n\
  Arguments for each section follow:\n\n";

  LALInferenceRunState *state;
  ProcessParamsTable *procParams=NULL;
  LALInferenceIFOData *data = NULL;

  /* Read command line and parse */
  procParams=LALInferenceParseCommandLine(argc,argv);
  if(LALInferenceGetProcParamVal(procParams,"--help"))
  {
    fprintf(stdout,"%s",help);
  }
  /* write down git information */
  fprintf(stdout,"\n\nLALInference version:%s,%s,%s,%s,%s\n\n", lalAppsVCSId,lalAppsVCSDate,lalAppsVCSBranch,lalAppsVCSAuthor,lalAppsVCSStatus);
  
  /* initialise runstate based on command line */
  /* This includes reading in the data */
  /* And performing any injections specified */
  /* And allocating memory */
  state = LALInferenceInitRunState(procParams);
  /* Create header */
  if (state!=NULL){
    ProcessParamsTable *ppt=NULL;
    ppt=LALInferenceGetProcParamVal(state->commandLine,"--outfile");
    if(!ppt){
      fprintf(stderr,"Must specify --outfile <filename.dat>\n");
      exit(1);
    }
    char *outfile=ppt->value;
    char headerfile[FILENAME_MAX];
    FILE *fpout=NULL;
    sprintf(headerfile,"%s_header.txt",outfile);
    fpout=fopen(headerfile,"w");
    fprintf(fpout,"LALInference version:%s,%s,%s,%s,%s\n", lalAppsVCSId,lalAppsVCSDate,lalAppsVCSBranch,lalAppsVCSAuthor,lalAppsVCSStatus);
    fprintf(fpout,"%s\n",LALInferencePrintCommandLine(state->commandLine));
    fclose(fpout);
    }
  if (state == NULL) {
      if (!LALInferenceGetProcParamVal(procParams, "--help")) {
          fprintf(stderr, "run state not allocated (%s, line %d).\n",
                  __FILE__, __LINE__);
      }
  } else {
      data = state->data;
  }

  /* Perform injections if data successful read or created */
  if (state){
    LALInferenceInjectInspiralSignal(data, state->commandLine);
  }

  /* Simulate calibration errors. 
  * NOTE: this must be called after both ReadData and (if relevant) 
  * injectInspiralTD/FD are called! */
  LALInferenceApplyCalibrationErrors(data, procParams);

  /* Set up the appropriate functions for the nested sampling algorithm */
  if (state){
    state->algorithm=&LALInferenceNestedSamplingAlgorithm;
    state->evolve=&LALInferenceNestedSamplingOneStep;

    state->proposalArgs = LALInferenceParseProposalArgs(state);
  }

  /* Set up the threads */
  LALInferenceInitCBCThreads(state,1);

  /* Init the prior */
  LALInferenceInitCBCPrior(state);

  /* Set up structures for nested sampling */
  LALInferenceNestedSamplingAlgorithmInit(state);

  if (state){
    for(INT4 i=0;i<state->nthreads;i++)
    {
      state->threads[i]->cycle=LALInferenceSetupDefaultInspiralProposalCycle(state->threads[i]->proposalArgs);
      LALInferenceRandomizeProposalCycle(state->threads[i]->cycle,state->GSLrandom);
    }
  }

  /* Choose the likelihood and set some auxiliary variables */
  LALInferenceInitLikelihood(state);

  /* Exit since we printed all command line arguments */
  if(state == NULL || LALInferenceGetProcParamVal(state->commandLine,"--help"))
  {
    exit(0);
  }

  /* Call setupLivePointsArray() to populate live points structures */
  LALInferenceSetupLivePointsArray(state);

  /* write injection with noise evidence information from algorithm */
  LALInferencePrintInjectionSample(state);

  /* Call nested sampling algorithm */
  state->algorithm(state);

  /* end */
  return(0);
}
