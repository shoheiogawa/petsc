#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: str.c,v 1.19 1998/04/24 02:14:50 bsmith Exp bsmith $";
#endif
/*
    We define the string operations here. The reason we just don't use 
  the standard string routines in the PETSc code is that on some machines 
  they are broken or have the wrong prototypes.

*/
#include "petsc.h"        /*I  "petsc.h"   I*/
#if defined(HAVE_STRING_H)
#include <string.h>
#endif
#if defined(HAVE_STRINGS_H)
#include <strings.h>
#endif
#include "pinclude/petscfix.h"

#undef __FUNC__  
#define __FUNC__ "PetscStrlen"
int PetscStrlen(char *s)
{
  int len;

  PetscFunctionBegin;
  if (!s) PetscFunctionReturn(0);
  len = strlen(s);
  PetscFunctionReturn(len);
}

#undef __FUNC__  
#define __FUNC__ "PetscStrcpy"
/*
    Handles copying null string correctly
*/
int PetscStrcpy(char *s,char *t)
{
  PetscFunctionBegin;
  if (t && !s) {
    SETERRQ(1,1,"Trying to copy string into null pointer");
  }
  if (t) {strcpy(s,t);}
  else {s[0] = 0;}
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "PetscStrncpy"
int PetscStrncpy(char *s,char *t,int n)
{
  PetscFunctionBegin;
  strncpy(s,t,n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "PetscStrcat"
int PetscStrcat(char *s,char *t)
{
  PetscFunctionBegin;
  strcat(s,t);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "PetscStrncat"
int PetscStrncat(char *s,char *t,int n)
{
  PetscFunctionBegin;
  strncat(s,t,n);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "PetscStrcmp"
int PetscStrcmp(char *a,char *b)
{
  int c;

  PetscFunctionBegin;
  if (!a && !b) PetscFunctionReturn(0);
  if (!a || !b) PetscFunctionReturn(1);
  c = strcmp(a,b);
  PetscFunctionReturn(c);
}

#undef __FUNC__  
#define __FUNC__ "PetscStrcasecmp"
int PetscStrcasecmp(char *a,char *b)
{
  int c;

  PetscFunctionBegin;
  if (!a && !b) c = 0;
  else if (!a || !b) c = 1;
#if defined (PARCH_nt)
  else c = stricmp(a,b);
#else
  else c = strcasecmp(a,b);
#endif
  PetscFunctionReturn(c);
}

#undef __FUNC__  
#define __FUNC__ "PetscStrncmp"
int PetscStrncmp(char *a,char *b,int n)
{
  int c;

  PetscFunctionBegin;
  c = strncmp(a,b,n);
  PetscFunctionReturn(c);
}

#undef __FUNC__  
#define __FUNC__ "PetscStrchr"
char *PetscStrchr(char *a,char b)
{
  char *c;

  PetscFunctionBegin;
  c = strchr(a,b);
  PetscFunctionReturn(c);
}

/*
      This is slightly different then the system version. 
   It returns the position after the position of b and 
   if it does not find it then it returns the entire string.
*/
#undef __FUNC__  
#define __FUNC__ "PetscStrrchr"
char *PetscStrrchr(char *a,char b)
{
  char *tmp;

  PetscFunctionBegin;
  tmp = strrchr(a,b);
  if (!tmp) tmp = a; else tmp = tmp + 1;
  PetscFunctionReturn(tmp);
}

/*
     This version is different from the system version in that
  it allows you to pass a read-only string into the function.
  A copy is made that is then passed into the system strtok() 
  routine.

    Limitation: 
  String must be less than or equal 1024 bytes in length.

*/
#undef __FUNC__  
#define __FUNC__ "PetscStrtok"
char *PetscStrtok(char *a,char *b)
{
  static char init[1024];
         char *tmp;

  PetscFunctionBegin;
  if (a) {
    PetscStrncpy(init,a,1024);
    a = init;
  }
  tmp = strtok(a,b);
  PetscFunctionReturn(tmp);
}

#undef __FUNC__  
#define __FUNC__ "PetscStrstr"
char *PetscStrstr(char*a,char *b)
{
  char *tmp;

  PetscFunctionBegin;
  tmp = strstr(a,b);
  PetscFunctionReturn(tmp);
}



