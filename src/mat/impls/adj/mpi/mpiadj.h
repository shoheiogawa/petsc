/* $Id: mpiadj.h,v 1.6 2000/04/09 03:10:00 bsmith Exp bsmith $ */

#include "src/mat/matimpl.h"

#if !defined(__ADJ_H)
#define __ADJ_H

/*  
  MATMPIAdj format - Compressed row storage for storing adjacency lists, and possibly weights
                     This is for grid reorderings (to reduce bandwidth)
                     grid partitionings, etc. This is NOT currently a dynamic data-structure.
                     
*/

typedef struct {
  int              *rowners;         /* ranges owned by each processor */
  int              rstart,rend;     /* start and end of local rows */
  int              m;                /* local rows */
  int              nz;
  int              *diag;            /* pointers to diagonal elements, if they exist */
  int              *i;               /* pointer to beginning of each row */
  int              *j;               /* column values: j + i[k] - 1 is start of row k */
  int              *values;          /* numerical values */
  PetscTruth       symmetric;        /* user indicates the nonzero structure is symmetric */
  PetscTruth       freeaij;          /* call PetscFree() on a, i,j at destroy */
} Mat_MPIAdj;

#endif
