//
//  DetectorFixedSkyCoords.c
//  
//
//  Created by John Veitch on 19/06/2014.
//
//

#include <stdio.h>
#include <math.h>
#include <lal/LALInference.h>
#include <lal/TimeDelay.h>
#include <lal/LALDetectors.h>
#include <lal/LALStatusMacros.h>
#include <lal/SkyCoordinates.h>
#include <lal/Date.h>

void LALInferenceDetFrameToEquatorial(LALDetector *det0, LALDetector *det1,
                                      REAL8 t0, REAL8 alpha, REAL8 theta,
                                      REAL8 *tg, REAL8 *ra, REAL8 *dec)
{
  /* Calculate vector between detectors in earth frame */
  REAL8 detvec[3];
  UINT4 i,j;
  for(i=0;i<3;i++) detvec[i]=det1->location[i]-det0->location[i];
  REAL8 detnorm=0;
  for(i=0;i<3;i++) detnorm+=detvec[i]*detvec[i];
  for(i=0;i<3;i++) detvec[i]/=sqrt(detnorm);
  
  REAL8 NORTH[3]={0,0,1};
  REAL8 a[3];
  /* Cross product of North vector with detector vector
   will be the rotation axis for our matrix */
  a[0]=detvec[1]*NORTH[2];
  a[1]=-detvec[0]*NORTH[2];
  a[2]=0.0;
  REAL8 norm=sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
  for (i=0;i<3;i++) a[i]/=norm;
  
  /* Cartesian coordinates in detector-aligned frame */
  REAL8 DETPOS[3];
  DETPOS[0]=sin(alpha)*cos(theta);
  DETPOS[1]=sin(alpha)*sin(theta);
  DETPOS[2]=cos(alpha);
  
  /* Rotation angle about a */
  REAL8 rotang=-acos(detvec[2]*NORTH[2]);
  REAL8 c=cos(rotang),s=sin(rotang);
  /* Find rotation matrix to transform to Geographic coordinates */
  REAL8 det2horiz[3][3]=
  {
    {c+a[0]*a[0]*(1.-c), a[1]*a[0]*(1.-c)-a[2]*s, a[0]*a[2]*(1.-c)+a[1]*s},
    {a[1]*a[0]*(1.-c)+a[2]*s, c+a[1]*a[1]*(1.-c), a[1]*a[2]*(1.-c)-a[0]*s},
    {a[0]*a[2]*(1.-c)-a[1]*s, a[1]*a[2]*(1.-c)+a[0]*s, c+a[2]*a[2]*(1.-c)}
  };
  
  /* Calculate cartesian coordinates in Geographic frame */
  REAL8 GEO[3]={0,0,0};
  for(i=0;i<3;i++) for(j=0;j<3;j++) GEO[i]+=det2horiz[i][j]*DETPOS[j];
  
  REAL8 r=0;
  for(i=0;i<3;i++) r+=GEO[i]*GEO[i];
  r=sqrt(r);
  
  /* Calculate Geographic coordinates */
  REAL8 longitude=atan2(GEO[1],GEO[0]);
  REAL8 latitude=asin(GEO[2]/r);
  /* Calculate Equatorial coordinates */
  SkyPosition horiz,equat;
  horiz.longitude=longitude;
  horiz.latitude=latitude;
  horiz.system=COORDINATESYSTEM_GEOGRAPHIC;
  
  LALStatus status;
  INITSTATUS(&status);
  LIGOTimeGPS gpstime;
  
  XLALGPSSetREAL8(&gpstime,t0);
  LALGeographicToEquatorial(&status,&equat,&horiz,&gpstime);
  if(status.statusCode)
  {
    REPORTSTATUS(&status);
    fprintf(stderr,"Error in coordinate conversion.\n");
    exit(1);
  }

  *ra=equat.longitude;
  *dec=equat.latitude;
  
  /* Compute time at geocentre (strictly speaking we use the wrong gpstime for the earth orientation by a few ms) */
  *tg=t0-XLALTimeDelayFromEarthCenter(det0->location, equat.longitude,equat.latitude,&gpstime);
  
  
}
