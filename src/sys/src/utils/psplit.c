/*$Id: psplit.c,v 1.10 2000/04/12 04:21:38 bsmith Exp bsmith $*/

#include "petsc.h"           /*I    "petsc.h" I*/

#undef __FUNC__  
#define __FUNC__ /*<a name=""></a>*/"PetscSplitOwnership"
/*@C
    PetscSplitOwnership - Given a global (or local) length determines a local 
        (or global) length via a simple formula

   Collective on MPI_Comm (if N is PETSC_DECIDE)

   Input Parameters:
+    comm - MPI communicator that shares the object being divided
.    n - local length (or PETSC_DECIDE to have it set)
-    N - global length (or PETSC_DECIDE)

  Level: developer

   Notes:
     n and N cannot be both PETSC_DECIDE

     If one processor calls this with N of PETSC_DECIDE then all processors
     must, otherwise the program will hang.

@*/
int PetscSplitOwnership(MPI_Comm comm,int *n,int *N)
{
  int ierr,size,rank;

  PetscFunctionBegin;
  if (*N == PETSC_DECIDE && *n == PETSC_DECIDE) SETERRQ(1,1,"Both n and N cannot be PETSC_DECIDE");

  if (*N == PETSC_DECIDE) { 
    ierr = MPI_Allreduce(n,N,1,MPI_INT,MPI_SUM,comm);CHKERRQ(ierr);
  } else if (*n == PETSC_DECIDE) { 
    ierr = MPI_Comm_size(comm,&size);CHKERRQ(ierr);
    ierr = MPI_Comm_rank(comm,&rank);CHKERRQ(ierr); 
    *n = *N/size + ((*N % size) > rank);
  }
#if defined(old_test)
  else {
    int tmp;
    ierr = MPI_Allreduce(n,&tmp,1,MPI_INT,MPI_SUM,comm);CHKERRQ(ierr);
    if (tmp != *N) {
      SETERRQ2(1,1,"Global length %d not equal sum of local lengths %d",*N,tmp);
    }
  }
#endif

  PetscFunctionReturn(0);
}

