/*----------------------------------------------------------------------- 
 * 
 * Author: Vallisneri, M.
 * 
 * Revision: $Id$
 * 
 *-----------------------------------------------------------------------
 */


#include <lal/Units.h>
#include <lal/LALInspiral.h>
#include <lal/SeqFactories.h>


NRCSID (LALSTPNWAVEFORMC, "$Id$");

/* private structure with PN parameters*/

typedef struct LALSTPNstructparams {
	REAL8 eta;
	REAL8 m1m2;
	REAL8 m2m1;
	REAL8 wdotnew;
	REAL8 wdotorb[9];
	REAL8 wspin15;
        REAL8 wspin20;
	REAL8 LNhdot15;
	REAL8 LNhdot20;
	REAL8 S1dot15;
	REAL8 S2dot15;
        REAL8 Sdot20;
} LALSTPNparams;

/* my function to set STPN derivatives*/

void LALSTPNderivatives(REAL8Vector *values, REAL8Vector *dvalues, void *mparams) {

    REAL8 s,omega,LNhx,LNhy,LNhz,LNmag,S1x,S1y,S1z,S2x,S2y,S2z;
    REAL8 alphadotcosi,tmpx,tmpy,tmpz,dotLNS1,dotLNS2,crossx,crossy,crossz;
    REAL8 ds,domega,domeganew,dLNhx,dLNhy,dLNhz,dS1x,dS1y,dS1z,dS2x,dS2y,dS2z;
    LALSTPNparams *params = (LALSTPNparams*)mparams;

    s     = values->data[0];
    omega = values->data[1];
      
    LNhx  = values->data[2];
    LNhy  = values->data[3];
    LNhz  = values->data[4];
    
    S1x  = values->data[5];
    S1y  = values->data[6];
    S1z  = values->data[7];

    S2x  = values->data[8];
    S2y  = values->data[9];
    S2z  = values->data[10];

    /* domega: could be optimized by computing omega^(1/3) and then multiplying it*/

    domeganew = params->wdotnew * pow(omega,11.0/3.0);

    domega =
	params->wdotorb[0] + 
	params->wdotorb[1] * pow(omega,1.0/3.0) +
	params->wdotorb[2] * pow(omega,2.0/3.0) +
	params->wdotorb[3] * omega +
	params->wdotorb[4] * pow(omega,4.0/3.0) +
	params->wdotorb[5] * pow(omega,5.0/3.0) +
	params->wdotorb[6] * omega * omega +
	params->wdotorb[7] * omega * omega * log(omega) +
	params->wdotorb[8] * pow(omega,7.0/3.0);

    domega += params->wspin15 * omega * 
	( LNhx * (113.0 * S1x + 113.0 * S2x + 75.0 * params->m2m1 * S1x + 75.0 * params->m1m2 * S2x) +
	  LNhy * (113.0 * S1y + 113.0 * S2y + 75.0 * params->m2m1 * S1y + 75.0 * params->m1m2 * S2y) +
	  LNhz * (113.0 * S1z + 113.0 * S2z + 75.0 * params->m2m1 * S1z + 75.0 * params->m1m2 * S2z) );

    dotLNS1 = (LNhx*S1x + LNhy*S1y + LNhz*S1z);
    dotLNS2 = (LNhx*S2x + LNhy*S2y + LNhz*S2z);

    domega += params->wspin20 * pow(omega,4.0/3.0) *
	( 247.0 * (S1x*S2x + S1y*S2y + S1z*S2z) -
	  721.0 * dotLNS1 * dotLNS2 );

    domega *= domeganew;

    /* dLN, 1.5PN*/

    tmpx = (4.0 + 3.0*params->m2m1) * S1x + (4.0 + 3.0*params->m1m2) * S2x;
    tmpy = (4.0 + 3.0*params->m2m1) * S1y + (4.0 + 3.0*params->m1m2) * S2y;
    tmpz = (4.0 + 3.0*params->m2m1) * S1z + (4.0 + 3.0*params->m1m2) * S2z;

    dLNhx = params->LNhdot15 * omega * omega * (-tmpz*LNhy + tmpy*LNhz);
    dLNhy = params->LNhdot15 * omega * omega * (-tmpx*LNhz + tmpz*LNhx);
    dLNhz = params->LNhdot15 * omega * omega * (-tmpy*LNhx + tmpx*LNhy);

    /* dLN, 2PN*/

    tmpx = dotLNS2 * S1x + dotLNS1 * S2x;
    tmpy = dotLNS2 * S1y + dotLNS1 * S2y;
    tmpz = dotLNS2 * S1z + dotLNS1 * S2z;

    dLNhx += params->LNhdot20 * pow(omega,7.0/3.0) * (-tmpz*LNhy + tmpy*LNhz);
    dLNhy += params->LNhdot20 * pow(omega,7.0/3.0) * (-tmpx*LNhz + tmpz*LNhx);
    dLNhz += params->LNhdot20 * pow(omega,7.0/3.0) * (-tmpy*LNhx + tmpx*LNhy);

    /* dS1, 1.5PN*/

    LNmag = params->eta * pow(omega,-1.0/3.0);

    crossx = (LNhy*S1z - LNhz*S1y);
    crossy = (LNhz*S1x - LNhx*S1z);
    crossz = (LNhx*S1y - LNhy*S1x);

    dS1x = params->S1dot15 * omega * omega * LNmag * crossx;
    dS1y = params->S1dot15 * omega * omega * LNmag * crossy;
    dS1z = params->S1dot15 * omega * omega * LNmag * crossz;

    /* dS1, 2PN*/

    tmpx = S1z*S2y - S1y*S2z;
    tmpy = S1x*S2z - S1z*S2x;
    tmpz = S1y*S2x - S1x*S2y;

    dS1x += params->Sdot20 * omega * omega *
	(tmpx - 3.0 * dotLNS2 * crossx);
    dS1y += params->Sdot20 * omega * omega *
	(tmpy - 3.0 * dotLNS2 * crossy);
    dS1z += params->Sdot20 * omega * omega *
	(tmpz - 3.0 * dotLNS2 * crossz);

    /* dS2, 1.5PN*/

    crossx = (-LNhz*S2y + LNhy*S2z);
    crossy = (-LNhx*S2z + LNhz*S2x);
    crossz = (-LNhy*S2x + LNhx*S2y);

    dS2x = params->S2dot15 * omega * omega * LNmag * crossx;
    dS2y = params->S2dot15 * omega * omega * LNmag * crossy;
    dS2z = params->S2dot15 * omega * omega * LNmag * crossz;

    /* dS2, 2PN*/

    dS2x += params->Sdot20 * omega * omega * (-tmpx - 3.0 * dotLNS1 * crossx);
    dS2y += params->Sdot20 * omega * omega * (-tmpy - 3.0 * dotLNS1 * crossy);
    dS2z += params->Sdot20 * omega * omega * (-tmpz - 3.0 * dotLNS1 * crossz);

    /* -? the original equations had one additional spin term*/

    /* dphi*/
    
    alphadotcosi = -LNhz * (LNhy*dLNhx - LNhx*dLNhy) / (LNhx*LNhx + LNhy*LNhy);
    ds = omega - alphadotcosi;

    /* copy back into dvalues structure*/

    dvalues->data[0] = ds;
    dvalues->data[1] = domega;
      
    dvalues->data[2] = dLNhx;
    dvalues->data[3] = dLNhy;
    dvalues->data[4] = dLNhz;
    
    dvalues->data[5] = dS1x;
    dvalues->data[6] = dS1y;
    dvalues->data[7] = dS1z;

    dvalues->data[8] = dS2x;
    dvalues->data[9] = dS2y;
    dvalues->data[10]= dS2z;
}

/* -? not sure what this does (what's the "CP"?);
   changed "LALEOBWaveformForInjectionCP" to "LALSTPNWaveformForInjectionCP" */

/*  <lalVerbatim file="LALSTPNWaveformForInjectionCP"> */
void 
LALSTPNWaveformForInjection (
			    LALStatus        *status,
			    CoherentGW       *waveform,
			    InspiralTemplate *params
			    ) 
{ 
  /* </lalVerbatim> */

  /* -? I have been a bit arbitrary in declaring some things as REAL8 and some as REAL4;*/
  /*    I hope it does not matter*/
  
  /* declare model parameters*/
  REAL8 m1, m2, mTot, eta, mu;
  REAL4 norb, nspn, nspnevl, thetahat;

  LALSTPNparams STPNparameters;
  LALSTPNparams *mparams;

  /* declare code parameters and variables*/
  INT4 nn = 11;              /* number of dynamical variables*/
  INT4 count;                /* integration steps performed*/
  rk4In in4;                 /* used to setup the Runge-Kutta integration*/
  REAL8Vector dummy, values, dvalues, newvalues, yt, dym, dyt;

  INT4 length;               /* memory allocation structure*/
  REAL4Vector *a = NULL;
  REAL4Vector *ff = NULL ;
  REAL8Vector *phi = NULL;
  REAL4Vector *shift = NULL;

  REAL8 lengths, chirpm, m;

  REAL8 t;                   /* time (units of total mass)*/

  INT4 j;                    /* counter*/
  REAL8 phiC;                /* temp var for final phase*/
  CreateVectorSequenceIn in; /* used to set the CoherentGW structure*/

  /* declare units*/
  REAL8 unitHz, dt;
  REAL8 LNhztol = 1.0e-8;

  /* declare initial values of dynamical variables*/
  REAL8 initphi, initf, initLNhx, initLNhy, initLNhz, initS1x, initS1y, initS1z, initS2x, initS2y, initS2z;

  /* declare dynamical variables*/
  REAL8 vphi, omega, LNhx, LNhy, LNhz, S1x, S1y, S1z, S2x, S2y, S2z;
  REAL8 alpha, i, omegadot;
  REAL8 f2a, apcommon;

  /* -? I am not too sure what this does. How should it be changed? */

  INITSTATUS(status, "LALSTPNWaveform", LALSTPNWAVEFORMC);
  ATTATCHSTATUSPTR(status);

  /* set parameters from InspiralTemplate structure*/

  /* I'm getting parameters from the "InspiralTemplate" structure: this is how it looks.
     typedef struct tagInspiralTemplate
     {
     REAL8 alpha;
     REAL8 alpha1;
     REAL8 alpha2;
     Approximant approximant;
     REAL8 beta;
     REAL8 chirpMass; 
     REAL8 distance; ... USE AS DISTANCE (meters)
     REAL8 eccentricity;
     REAL8 eta; ... SET TO symmetric mass ratio
     REAL8 fCutoff;
     REAL8 fendBCV;
     REAL8 fFinal;
     REAL8 fLower; ... USE AS INITIAL FREQUENCY (Hz)
     INT4  ieta;
     REAL8 inclination; ... NOT USED
     INT4  level;
     REAL4 minMatch;
     REAL8 mass1; ... USE AS MASS1 
     REAL8 mass2; ... USE AS MASS2
     InputMasses massChoice;
     REAL8 mu; ... SET TO reduced mass
     INT4  number;
     INT4  nStartPad;
     INT4  nEndPad;
     REAL8 OmegaS;
     REAL8 orbitTheta0; ... USE TO SET initLNh; CANNOT BE ZERO!
     REAL8 orbitPhi0; ... USE TO SET initLNh
     Order order; ... USE AS common PN order
     REAL8 psi0;
     REAL8 psi3;
     REAL8 rFinal;
     REAL8 rInitial;
     REAL8 rLightRing;
     REAL8 signalAmplitude; ... NOT USED
     INT4Vector *segmentIdVec;
     REAL8 spin1[3]; ... USE AS spin1 (not normalized) in units of mass1^2
     REAL8 spin2[3]; ... USE AS spin2 (not normalized) in units of mass2^2
     REAL8 sourceTheta; ... this is the position of the source (colatitude)
     REAL8 sourcePhi;   ... this is the position of the source (azimuth)
     REAL8 startPhase; ... NOT USED
     REAL8 startTime; ... NOT USED
     REAL8 t0; 
     REAL8 t2; 
     REAL8 t3; 
     REAL8 t4; 
     REAL8 t5; 
     REAL8 tC; 
     REAL8 Theta;
     REAL8 totalMass; ... SET TO mass1 + mass2
     REAL8 tSampling; ... USE AS SAMPLING AND INTEGRATION TIME
     REAL8 vFinal;
     REAL8 vInitial;
     REAL8 Zeta2;
     struct tagInspiralTemplate *next;
     struct tagInspiralTemplate *fine;
     } InspiralTemplate; */

  m1 = params->mass1;
  m2 = params->mass2;

  mTot =  m1 + m2;
  params->totalMass = mTot;

  eta = (m1 * m2) / (mTot * mTot);
  params->eta = eta;

  mu = eta * mTot;
  params->mu = mu;

  /* -- where should we get this from?*/

  thetahat = 0.0;

  /* -- I hope this is in Hz*/

  initf = params->fLower;

  /* PN-order switches*/

  switch(params->order) {

  case twoPN:
      norb = 2.0;
      nspn = 2.0;
      nspnevl = 2.0;
      break;
  case twoPointFivePN:
      norb = 2.5;
      nspn = 2.5;
      nspnevl = 2.5;
      break;
  case threePN:
      norb = 3.0;
      nspn = 3.0;
      nspnevl = 3.0;
      break;
  default:
  case threePointFivePN:
      norb = 3.5;
      nspn = 3.5;
      nspnevl = 3.5;
      break;
  }

  mparams = &STPNparameters;

  /* -? I hope the units in the following are correct*/
  apcommon = -4.0 * mu * LAL_MRSUN_SI/params->distance;

  /* set code parameters*/

  /* set units*/

  m = mTot * LAL_MTSUN_SI;
  unitHz = mTot * LAL_MTSUN_SI * (REAL8)LAL_PI;

  /* -? dt is set from params; but Thomas' code also contained instructions to*/
  /*    set it as -1. * eta / ( params->tSampling * 5.0*LAL_MTSUN_SI*mTot );*/

  /*    tSampling is in Hz, so dt is in seconds*/

  dt = 1.0/params->tSampling;

  /* -? if the integration timestep is too large, it could be set to a fraction of the total mass*/
  /* dt = 10e-3 * m;*/

  /* -- length in seconds from Newtonian formula; chirpm in seconds*/

  chirpm = mTot * LAL_MTSUN_SI * pow(eta,3.0/5.0);
  lengths = (5.0/256.0) * pow(LAL_PI,-8.0/3.0) * pow(chirpm*initf,-5.0/3.0) / initf;

  length = ceil(log10(lengths/dt)/log10(2.0));
  length = pow(2,length);

  /* set initial values of dynamical variables*/

  initphi = 0.0; /* -? see code at the end; initial phase is disabled for the moment*/

  /* -- initial omega is set from initf*/

  /* note that Theta0 cannot be 0.0!*/
  initLNhx = sin(params->orbitTheta0)*cos(params->orbitPhi0);
  initLNhy = sin(params->orbitTheta0)*sin(params->orbitPhi0);
  initLNhz = cos(params->orbitTheta0);

  initS1x = params->spin1[0] * (m1 * m1) / (mTot * mTot);
  initS1y = params->spin1[1] * (m1 * m1) / (mTot * mTot);
  initS1z = params->spin1[2] * (m1 * m1) / (mTot * mTot);

  initS2x = params->spin2[0] * (m2 * m2) / (mTot * mTot);
  initS2y = params->spin2[1] * (m2 * m2) / (mTot * mTot);
  initS2z = params->spin2[2] * (m2 * m2) / (mTot * mTot);
  
  /* -? this set of assertions is probably incomplete */

  /* Make sure parameter and waveform structures exist. */

  ASSERT(params,   status, LALINSPIRALH_ENULL, LALINSPIRALH_MSGENULL);
  ASSERT(waveform, status, LALINSPIRALH_ENULL, LALINSPIRALH_MSGENULL);  

  /* Make sure waveform fields don't exist. */

  ASSERT(!( waveform->a ),     status, LALINSPIRALH_ENULL, LALINSPIRALH_MSGENULL);
  ASSERT(!( waveform->f ),     status, LALINSPIRALH_ENULL, LALINSPIRALH_MSGENULL);
  ASSERT(!( waveform->phi ),   status, LALINSPIRALH_ENULL, LALINSPIRALH_MSGENULL);
  ASSERT(!( waveform->shift ), status, LALINSPIRALH_ENULL, LALINSPIRALH_MSGENULL);

  /* Some params should be greater than 0. Fine */

  ASSERT(params,                 status, LALINSPIRALH_ENULL, LALINSPIRALH_MSGENULL);
  ASSERT(params->fLower > 0.,    status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
  ASSERT(params->tSampling > 0., status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);
  ASSERT(params->totalMass > 0., status, LALINSPIRALH_ESIZE, LALINSPIRALH_MSGESIZE);

  /* and now we can allocate memory for some time series */

  LALSCreateVector(status->statusPtr, &ff, length);
  CHECKSTATUSPTR(status);

  LALSCreateVector(status->statusPtr, &a, 2*length);
  CHECKSTATUSPTR(status);

  LALDCreateVector(status->statusPtr, &phi, length);
  CHECKSTATUSPTR(status);

  LALSCreateVector(status->statusPtr, &shift, length);
  CHECKSTATUSPTR(status);
  
  /* Allocate all the memory required to dummy and then point the various
     arrays to dummy - this makes it easier to handle memory failures */

  dummy.length = nn * 6;
  
  values.length = dvalues.length = newvalues.length =
    yt.length = dym.length = dyt.length = nn;
  
  if (!(dummy.data = (REAL8 * ) LALMalloc(sizeof(REAL8) * nn * 6))) {
    ABORT(status, LALINSPIRALH_EMEM, LALINSPIRALH_MSGEMEM);
  }

  values.data = &dummy.data[0];
  dvalues.data = &dummy.data[nn];
  newvalues.data = &dummy.data[2*nn];
  yt.data = &dummy.data[3*nn];
  dym.data = &dummy.data[4*nn];
  dyt.data = &dummy.data[5*nn];
  
  /* setup coefficients for PN equations*/

  mparams->wdotnew = (96.0/5.0) * eta;

  mparams->wdotorb[0] = 1.0;                                                                       /* 0PN*/
  mparams->wdotorb[1] = 0.0;                                                                       /* 0.5PN*/
  mparams->wdotorb[2] = norb < 1.0 ? 0.0 : ( -(1.0/336.0) * (743.0 + 924.0*eta) );                 /* 1PN*/
  mparams->wdotorb[3] = norb < 1.5 ? 0.0 : ( 4.0 * LAL_PI );                                       /* 1.5PN*/
  mparams->wdotorb[4] = norb < 2.0 ? 0.0 : ( (34103.0 + 122949.0*eta + 59472.0*eta*eta)/18144.0 ); /* 2PN*/
  mparams->wdotorb[5] = norb < 2.5 ? 0.0 : ( -(1.0/672.0) * LAL_PI * (4159.0 + 14532.0*eta) );     /* 2.5PN*/
  mparams->wdotorb[6] = norb < 3.0 ? 0.0 : ( (16447322263.0/139708800.0)
					     - (1712.0/105.0)*0.577216 /* EulerGamma*/
					     - (273811877.0/1088640.0)*eta - (88.0/3.0)*thetahat*eta 
					     + (541.0/806.0)*eta*eta - (5605.0/2592.0)*eta*eta*eta
					     + (1.0/48.0) * LAL_PI*LAL_PI * (256.0 + 451.0*eta)
					     - (856.0/105.0)*log(16.0) );                          /* 3PN*/
  mparams->wdotorb[7] = norb < 3.0 ? 0.0 : ( -(1712.0/315.0) );                                    /* 3PN*/
  mparams->wdotorb[8] = norb < 3.5 ? 0.0 : (LAL_PI/12096.0) * (-13245.0 + 661775.0*eta + 599156.0*eta*eta); /* 3.5PN*/
  
  mparams->wspin15 = nspn < 1.5 ? 0.0 : ( -(1.0/12.0) );
  mparams->wspin20 = nspn < 2.0 ? 0.0 : ( -(1.0/48.0) / eta );
  
  mparams->m2m1 = m2/m1;
  mparams->m1m2 = m1/m2;
  
  mparams->eta = eta;

  mparams->LNhdot15 = nspnevl < 1.5 ? 0.0 : 0.5;
  mparams->LNhdot20 = nspnevl < 2.0 ? 0.0 : ( -1.5/eta );

  mparams->S1dot15 = nspnevl < 1.5 ? 0.0 : ( (4.0 + 3.0*m2/m1)/2.0 );
  mparams->S2dot15 = nspnevl < 1.5 ? 0.0 : ( (4.0 + 3.0*m1/m2)/2.0 );

  mparams->Sdot20 = nspnevl < 2.0 ? 0.0 : 0.5;

  /* setup initial conditions for dynamical variables*/

  vphi = initphi;
  omega = initf * unitHz;

  LNhx = initLNhx;
  LNhy = initLNhy;
  LNhz = initLNhz;

  S1x = initS1x;
  S1y = initS1y;
  S1z = initS1z;

  S2x = initS2x;
  S2y = initS2y;
  S2z = initS2z;

  /* copy everything in the "values" structure*/

  values.data[0] = vphi;
  values.data[1] = omega;

  values.data[2] = LNhx;
  values.data[3] = LNhy;
  values.data[4] = LNhz;

  values.data[5] = S1x;
  values.data[6] = S1y;
  values.data[7] = S1z;

  values.data[8] = S2x;
  values.data[9] = S2y;
  values.data[10]= S2z;
  
  /* setup LALRungeKutta4: parameters are
     - "newvalues" (some kind of array allocated above with "dummy")
     - "in4":
     TestFunction *function -> is set to the derivative function
     REAL8 x (time)         -> is set to the time over m
     REAL8Vector *y         -> is a pointer set to "&values": current variables
     REAL8Vector *dydx      -> is a pointer set to "&dvalues": current derivatives
     REAL8Vector *yt        -> is a pointer set to "&yt"; probably a workbuffer
     REAL8Vector *dym       -> is a pointer set to "&dym"; probably a workbuffer
     REAL8Vector *dyt       -> is a pointer set to "&dyt"; probably a workbuffer
     REAL8 h                -> is the timestep, set to dt/m
     INT4 n
     - "funcParams": void pointer to some parameters needed by derivatives */

  in4.function = LALSTPNderivatives;
  in4.y = &values;
  in4.dydx = &dvalues;
  in4.h = dt/m;
  in4.n = nn;
  in4.yt = &yt;
  in4.dym = &dym;
  in4.dyt = &dyt;

  /* main integration loop*/

  t = 0.0;
  count = 0;

  /* -- for the first step let's assume omegadot is positive;*/
  /*    otherwise I should call the derivative function here*/

  LALSTPNderivatives(&values,&dvalues,(void*)mparams);
  omegadot = dvalues.data[1];

  /* -? the original BCV target model had one additional stopping test*/
  /*    that I should probably reinstate*/

  while(omegadot > 0 && LNhz*LNhz < 1.0 - LNhztol && omega/unitHz < 1000.0) {
      ASSERT(count < length, status, LALINSPIRALH_EMEM, "Out of memory during integration");
      /* From SimulateCoherentGW.h:
	 <<We therefore write the waveforms in terms of two polarization amplitudes $A_1(t)$
	 and $A_2(t)$, a single phase function $\phi(t)$, and a polarization shift $\Phi(t)$:
	 h_+(t) = A_1(t)\cos\Phi(t)\cos\phi(t) - A_2(t)\sin\Phi(t)\sin\phi(t)
	 h_\times(t) = A_1(t)\sin\Phi(t)\cos\phi(t) + A_2(t)\cos\Phi(t)\sin\phi(t)>>
	 
	 Note: I can only do it if BCV2's Theta = 0. Hopefully that degree of freedom can
	 be recovered from the orientation of the initial angular momentum. */

      /* now setting the wave from the dynamical variables*/

      alpha = atan2(LNhy, LNhx);

      /* I don't really need i, because I can use the explicit formulae below*/
      /* i = acos(LNhz);*/

      /* -? if omega is in units of total masses, then the following conversion is correct*/
      /*    was previously f2a = pow (f2aFac * omega, 2./3.)*/
      f2a = pow(omega, 2./3.);

      /* the ff assignment is fine, since unitHz is pi M Msun [s]*/

      ff->data[count]= (REAL4)(omega/unitHz);

      /* a->data[2*count]          = (REAL4)(apcommon * f2a * (-0.25) * (3.0 + cos(2*i)));*/
      /* a->data[2*count+1]        = (REAL4)(apcommon * f2a * (-cos(i)));                 */

      a->data[2*count]          = (REAL4)(apcommon * f2a * 0.5 * (1 + LNhz*LNhz));
      a->data[2*count+1]        = (REAL4)(apcommon * f2a * LNhz);                 
      
      phi->data[count]          = (REAL8)(2.0 * vphi);
      shift->data[count]        = (REAL4)(2.0 * alpha);

      /* Debugging: it can be occasionally useful to store dynamical variables
	 in the waveform output structure. Keep this here.

	 ff->data[count] = omega; phi->data[count] = vphi;
	 a->data[2*count] = LNhx; a->data[2*count+1] = LNhy; shift->data[count] = LNhz; */

      /* advancing time and calling stepper; time is in units of total mass*/

      in4.x = t/m;

      /* This integrator should be replaced, eventually,*/
      /* by a variable-timestep algorithm*/

      LALRungeKutta4(status->statusPtr, &newvalues, &in4, (void*)mparams);
      CHECKSTATUSPTR(status);

      /* updating values of dynamical variables*/
         
      vphi  = values.data[0] = newvalues.data[0];
      omega = values.data[1] = newvalues.data[1];
      
      LNhx  = values.data[2] = newvalues.data[2];
      LNhy  = values.data[3] = newvalues.data[3];
      LNhz  = values.data[4] = newvalues.data[4];
    
      S1x   = values.data[5] = newvalues.data[5];
      S1y   = values.data[6] = newvalues.data[6];
      S1z   = values.data[7] = newvalues.data[7];

      S2x   = values.data[8] = newvalues.data[8];
      S2y   = values.data[9] = newvalues.data[9];
      S2z   = values.data[10]= newvalues.data[10];

      LALSTPNderivatives(&values,&dvalues,(void*)mparams);
      omegadot = dvalues.data[1];

      t = (++count - params->nStartPad) * dt;
  }  

  /* -? the EOB version saves some final values in params; I'm doing only fFinal*/

  params->fFinal = (REAL4)(omega/unitHz);

  /* -? this looks like the final phase is being subtracted from the phase history*/
  /*    this operations negates the use of initphi, which is set to 0.0 anyway*/
  /* -? I will comment this out to compare with my Mathematica code*/

  /* phiC = phi->data[count-1] ;*/

  /* for (j=0; j<count;j++)*/
  /*    phi->data[j] = phi->data[j] - phiC;*/

  /* Allocate the waveform buffers, to be filled with what we have computed*/

  if ( ( waveform->a = (REAL4TimeVectorSeries *)LALMalloc( sizeof(REAL4TimeVectorSeries) ) ) == NULL ) {
    ABORT( status, LALINSPIRALH_EMEM, LALINSPIRALH_MSGEMEM );
  }
  memset( waveform->a, 0, sizeof(REAL4TimeVectorSeries) );
  if ( ( waveform->f = (REAL4TimeSeries *)LALMalloc( sizeof(REAL4TimeSeries) ) ) == NULL ) {
    LALFree( waveform->a ); waveform->a = NULL;
    ABORT( status, LALINSPIRALH_EMEM, LALINSPIRALH_MSGEMEM );
  }
  memset( waveform->f, 0, sizeof(REAL4TimeSeries) );
  if ( ( waveform->phi = (REAL8TimeSeries *)LALMalloc( sizeof(REAL8TimeSeries) ) ) == NULL ) {
    LALFree( waveform->a ); waveform->a = NULL;
    LALFree( waveform->f ); waveform->f = NULL;
    ABORT( status, LALINSPIRALH_EMEM, LALINSPIRALH_MSGEMEM );
  }
  memset( waveform->phi, 0, sizeof(REAL8TimeSeries) );
  if ( ( waveform->shift = (REAL4TimeSeries *)LALMalloc( sizeof(REAL4TimeSeries) ) ) == NULL ) {
    LALFree( waveform->phi ); waveform->phi = NULL;
    LALFree( waveform->a ); waveform->a = NULL;
    LALFree( waveform->f ); waveform->f = NULL;
    ABORT( status, LALINSPIRALH_EMEM, LALINSPIRALH_MSGEMEM );
  }
  memset( waveform->shift, 0, sizeof(REAL4TimeSeries) );

  in.length = (UINT4)count;
  in.vectorLength = 2;
  LALSCreateVectorSequence( status->statusPtr, &( waveform->a->data ), &in );
  CHECKSTATUSPTR(status);      
  LALSCreateVector( status->statusPtr, &( waveform->f->data ), count);
  CHECKSTATUSPTR(status);      
  LALDCreateVector( status->statusPtr, &( waveform->phi->data ), count );
  CHECKSTATUSPTR(status);        
  LALSCreateVector( status->statusPtr, &( waveform->shift->data ), count );
  CHECKSTATUSPTR(status);        

  memcpy(waveform->f->data->data, ff->data, count*(sizeof(REAL4)));
  memcpy(waveform->a->data->data, a->data, 2*count*(sizeof(REAL4)));
  memcpy(waveform->phi->data->data, phi->data, count*(sizeof(REAL8)));
  memcpy(waveform->shift->data->data, shift->data, count*(sizeof(REAL4)));

  /* -? previously this was 1./params->tSampling, but I think it should be just dt (secs) */
 
  waveform->a->deltaT = waveform->f->deltaT = waveform->phi->deltaT = waveform->shift->deltaT = dt;

  waveform->a->sampleUnits = lalStrainUnit;
  waveform->f->sampleUnits = lalHertzUnit;
  waveform->phi->sampleUnits = lalDimensionlessUnit;
  waveform->shift->sampleUnits = lalDimensionlessUnit;

  /* -? should add this ? 
     waveform->position = params->position;
     waveform->psi = params->psi; */

  LALSnprintf( waveform->a->name, LALNameLength, "STPN inspiral amplitudes" );
  LALSnprintf( waveform->f->name, LALNameLength, "STPN inspiral frequency" );  
  LALSnprintf( waveform->phi->name, LALNameLength, "STPN inspiral phase" );  
  LALSnprintf( waveform->shift->name, LALNameLength, "STPN inspiral polshift" );

  params->tC = count / params->tSampling ;

  /* -? I am not sure I understand this! Why is tSampling derived this way?*/
  /*    it seems to me that these parameters are used to return other values*/
  /*    but what should I do? */

  params->tSampling = (REAL4)(waveform->f->data->data[count-1] - waveform->f->data->data[count-2]);
  params->tSampling /= (REAL4)dt;
  params->nStartPad = count;

  LALFree(shift->data);
  LALFree(a->data);
  LALFree(ff->data);
  LALFree(phi->data);
 
  LALFree(dummy.data);
  DETATCHSTATUSPTR(status);
  RETURN(status);
}

