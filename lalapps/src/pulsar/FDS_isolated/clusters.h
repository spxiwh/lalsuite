#ifndef _CLUSTERS_H  /* Double-include protection. */
#define _CLUSTERS_H

#include <lal/LALDatatypes.h>

/* C++ protection. */
#ifdef  __cplusplus
extern "C" {
#endif

typedef struct Clusterstag {
  INT2  Nclusters;     /* how many clusters */
  UINT4  *NclustPoints; /* for each cluster: how many points it has */
  UINT4  *Iclust;       /* index of the first datum of each cluster */
  REAL8 *clusters;     /* value of ratio for each cluster point, for all clusters */
} Clusters;

typedef struct ClustersParamstag {
  INT4  wings;
  INT2  smallBlock;
} ClustersParams;

typedef struct Outlierstag {
  UINT4  Noutliers;
  INT4  rightwing;
  INT4  leftwing;
  UINT4  *outlierIndexes; /*  indexes in OutliersInput->data vector */
  REAL8 *ratio;
} Outliers;

typedef struct OutliersInputtag {
  REAL8Vector *data;
  INT4        ifmin;
} OutliersInput;

typedef struct OutliersParamstag {
  REAL8Vector *Floor;
  REAL4       Thr; 
  INT4        wings;
  INT4        ifmin;
} OutliersParams;

typedef struct ClustersInputtag {
  OutliersInput  *outliersInput;
  OutliersParams *outliersParams;
  Outliers       *outliers;
} ClustersInput;



/* Function Prototypes */

void DetectClusters(LALStatus *, ClustersInput *input, ClustersParams *clParams, Clusters *output);
int ComputeOutliers(OutliersInput *outliersInput, OutliersParams *outlierParams, Outliers *outliers);
void EstimateFloor(LALStatus *, REAL8Vector *input, INT2 windowSize, REAL8Vector *output);

#ifdef  __cplusplus
}
#endif  
/* C++ protection. */

#endif  /* Double-include protection. */
