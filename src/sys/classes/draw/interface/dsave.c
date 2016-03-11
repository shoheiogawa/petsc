#include <petsc/private/drawimpl.h>  /*I "petscdraw.h" I*/

PETSC_EXTERN PetscErrorCode PetscDrawImageSave(const char[],const char[],unsigned char[][3],unsigned int,unsigned int,const unsigned char[]);
PETSC_EXTERN PetscErrorCode PetscDrawImageCheckFormat(const char *[]);
PETSC_EXTERN PetscErrorCode PetscDrawMovieCheckFormat(const char *[]);

#if defined(PETSC_HAVE_SAWS)
static PetscErrorCode PetscDrawSave_SAWs(PetscDraw);
#endif

#undef __FUNCT__
#define __FUNCT__ "PetscDrawSetSave"
/*@C
   PetscDrawSetSave - Saves images produced in a PetscDraw into a file

   Collective on PetscDraw

   Input Parameter:
+  draw      - the graphics context
.  filename  - name of the file, if .ext then uses name of draw object plus .ext using .ext to determine the image type
-  movieext  - if not NULL, produces a movie of all the images

   Options Database Command:
+  -draw_save  <filename>  - filename could be name.ext or .ext (where .ext determines the type of graphics file to save, for example .png)
.  -draw_save_movie <.ext> - saves a movie to filename.ext
.  -draw_save_final_image [optional filename] - saves the final image displayed in a window
-  -draw_save_single_file - saves each new image in the same file, normally each new image is saved in a new file with filename/filename_%d.ext

   Level: intermediate

   Concepts: X windows^graphics

   Notes: You should call this BEFORE creating your image and calling PetscDrawSave().
   The supported image types are .png, .gif, .jpg, and .ppm (PETSc chooses the default in that order).
   Support for .png images requires configure --with-libpng.
   Support for .gif images requires configure --with-giflib.
   Support for .jpg images requires configure --with-libjpeg.
   Support for .ppm images is built-in. The PPM format has no compression (640x480 pixels ~ 900 KiB).
   The ffmpeg utility must be in your path to make the movie.

.seealso: PetscDrawSetFromOptions(), PetscDrawCreate(), PetscDrawDestroy(), PetscDrawSetSaveFinalImage()
@*/
PetscErrorCode  PetscDrawSetSave(PetscDraw draw,const char filename[],const char movieext[])
{
  const char     *savename = NULL;
  const char     *imageext = NULL;
  char           buf[PETSC_MAX_PATH_LEN];
  PetscErrorCode ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(draw,PETSC_DRAW_CLASSID,1);
  if (filename) PetscValidCharPointer(filename,2);
  if (movieext) PetscValidCharPointer(movieext,2);

  /* determine save filename and image extension */
  if (filename && filename[0]) {
    ierr = PetscStrchr(filename,'.',(char **)&imageext);CHKERRQ(ierr);
    if (!imageext) savename = filename;
    else if (imageext != filename) {
      size_t l1 = 0,l2 = 0;
      ierr = PetscStrlen(filename,&l1);CHKERRQ(ierr);
      ierr = PetscStrlen(imageext,&l2);CHKERRQ(ierr);
      ierr = PetscStrncpy(buf,filename,l1-l2+1);CHKERRQ(ierr);
      savename = buf;
    }
  }

  if (!savename) {ierr = PetscObjectGetName((PetscObject)draw,&savename);CHKERRQ(ierr);}
  ierr = PetscDrawImageCheckFormat(&imageext);CHKERRQ(ierr);
  if (movieext) {ierr = PetscDrawMovieCheckFormat(&movieext);CHKERRQ(ierr);}
  if (movieext) draw->savesinglefile = PETSC_FALSE; /* otherwise we cannot generage movies */

  if (draw->savesinglefile) {
    ierr = PetscInfo2(NULL,"Will save image to file %s%s\n",savename,imageext);CHKERRQ(ierr);
  } else {
    ierr = PetscInfo3(NULL,"Will save images to file %s/%s_%%d%s\n",savename,savename,imageext);CHKERRQ(ierr);
  }
  if (movieext) {
    ierr = PetscInfo2(NULL,"Will save movie to file %s%s\n",savename,movieext);CHKERRQ(ierr);
  }

  draw->savefilecount = 0;
  ierr = PetscFree(draw->savefilename);CHKERRQ(ierr);
  ierr = PetscFree(draw->saveimageext);CHKERRQ(ierr);
  ierr = PetscFree(draw->savemovieext);CHKERRQ(ierr);
  ierr = PetscStrallocpy(savename,&draw->savefilename);CHKERRQ(ierr);
  ierr = PetscStrallocpy(imageext,&draw->saveimageext);CHKERRQ(ierr);
  ierr = PetscStrallocpy(movieext,&draw->savemovieext);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PetscDrawSetSaveFinalImage"
/*@C
   PetscDrawSetSaveFinalImage - Saves the final image produced in a PetscDraw into a file

   Collective on PetscDraw

   Input Parameter:
+  draw      - the graphics context
-  filename  - name of the file, if NULL or empty uses name set with PetscDrawSetSave() or name of draw object

   Options Database Command:
.  -draw_save_final_image  <filename> - filename could be name.ext or .ext (where .ext determines the type of graphics file to save, for example .png)

   Level: intermediate

   Concepts: X windows^graphics

   Notes: You should call this BEFORE creating your image and calling PetscDrawSave().
   The supported image types are .png, .gif, and .ppm (PETSc chooses the default in that order).
   Support for .png images requires configure --with-libpng.
   Support for .gif images requires configure --with-giflib.
   Support for .jpg images requires configure --with-libjpeg.
   Support for .ppm images is built-in. The PPM format has no compression (640x480 pixels ~ 900 KiB).

.seealso: PetscDrawSetFromOptions(), PetscDrawCreate(), PetscDrawDestroy(), PetscDrawSetSave()
@*/
PetscErrorCode  PetscDrawSetSaveFinalImage(PetscDraw draw,const char filename[])
{
  char           buf[PETSC_MAX_PATH_LEN];
  PetscErrorCode ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(draw,PETSC_DRAW_CLASSID,1);
  if (!filename || !filename[0]) {
    if (!draw->savefilename) {
      ierr = PetscObjectGetName((PetscObject)draw,&filename);CHKERRQ(ierr);
    } else {
      ierr = PetscSNPrintf(buf,sizeof(buf),"%s%s",draw->savefilename,draw->saveimageext);CHKERRQ(ierr);
      filename = buf;
    }
  }
  ierr = PetscFree(draw->savefinalfilename);CHKERRQ(ierr);
  ierr = PetscStrallocpy(filename,&draw->savefinalfilename);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PetscDrawSave"
/*@
   PetscDrawSave - Saves a drawn image

   Collective on PetscDraw

   Input Parameters:
.  draw - the drawing context

   Level: advanced

   Notes: this is not normally called by the user, it is called by PetscDrawFlush() to save a sequence of images.

.seealso: PetscDrawSetSave()

@*/
PetscErrorCode  PetscDrawSave(PetscDraw draw)
{
  PetscInt       savecount;
  char           basename[PETSC_MAX_PATH_LEN];
  unsigned char  palette[256][3];
  unsigned int   w,h;
  unsigned char  *pixels = NULL;
  PetscMPIInt    rank;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(draw,PETSC_DRAW_CLASSID,1);
  if (draw->ops->save) {ierr = (*draw->ops->save)(draw);CHKERRQ(ierr); goto finally;}
  if (!draw->savefilename) PetscFunctionReturn(0);
  if (!draw->ops->getimage) PetscFunctionReturn(0);
  ierr = MPI_Comm_rank(PetscObjectComm((PetscObject)draw),&rank);CHKERRQ(ierr);

  savecount = draw->savefilecount++;

  if (!rank && !savecount) {
    char path[PETSC_MAX_PATH_LEN];
    if (draw->savesinglefile) {
      ierr = PetscSNPrintf(path,sizeof(path),"%s%s",draw->savefilename,draw->saveimageext);CHKERRQ(ierr);
      (void)remove(path);
    } else {
      ierr = PetscSNPrintf(path,sizeof(path),"%s",draw->savefilename);CHKERRQ(ierr);
      ierr = PetscRMTree(path);CHKERRQ(ierr);
      ierr = PetscMkdir(path);CHKERRQ(ierr);
    }
    if (draw->savemovieext) {
      ierr = PetscSNPrintf(path,sizeof(path),"%s%s",draw->savefilename,draw->savemovieext);CHKERRQ(ierr);
      (void)remove(path);
    }
  }
  if (draw->savesinglefile) {
    ierr = PetscSNPrintf(basename,sizeof(basename),"%s",draw->savefilename);CHKERRQ(ierr);
  } else {
    ierr = PetscSNPrintf(basename,sizeof(basename),"%s/%s_%d",draw->savefilename,draw->savefilename,(int)savecount);CHKERRQ(ierr);
  }

  /* this call is collective, only the first process gets the image data */
  ierr = (*draw->ops->getimage)(draw,palette,&w,&h,&pixels);CHKERRQ(ierr);
  /* only the first process handles the saving business */
  if (!rank) {ierr = PetscDrawImageSave(basename,draw->saveimageext,palette,w,h,pixels);CHKERRQ(ierr);}
  ierr = PetscFree(pixels);CHKERRQ(ierr);
  ierr = MPI_Barrier(PetscObjectComm((PetscObject)draw));CHKERRQ(ierr);

finally:
#if defined(PETSC_HAVE_SAWS)
  ierr = PetscDrawSave_SAWs(draw);CHKERRQ(ierr);
#endif
  PetscFunctionReturn(0);
}

#if defined(PETSC_HAVE_SAWS)
#include <petscviewersaws.h>
/*
  The PetscImageList object and functions are used to maintain a list of file images
  that can be displayed by the SAWs webserver.
*/
typedef struct _P_PetscImageList *PetscImageList;
struct _P_PetscImageList {
  PetscImageList next;
  char           *filename;
  char           *ext;
  PetscInt       count;
} ;

static PetscImageList SAWs_images = NULL;

#undef __FUNCT__
#define __FUNCT__ "PetscImageListDestroy"
static PetscErrorCode PetscImageListDestroy(void)
{
  PetscErrorCode ierr;
  PetscImageList image = SAWs_images;

  PetscFunctionBegin;
  while (image) {
    PetscImageList next = image->next;
    ierr = PetscFree(image->filename);CHKERRQ(ierr);
    ierr = PetscFree(image->ext);CHKERRQ(ierr);
    ierr = PetscFree(image);CHKERRQ(ierr);
    image = next;
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PetscImageListAdd"
static PetscErrorCode PetscImageListAdd(const char filename[],const char ext[],PetscInt count)
{
  PetscErrorCode  ierr;
  PetscImageList  image,oimage = SAWs_images;
  PetscBool       flg;

  PetscFunctionBegin;
  if (oimage) {
    ierr = PetscStrcmp(filename,oimage->filename,&flg);CHKERRQ(ierr);
    if (flg) {
      oimage->count = count;
      PetscFunctionReturn(0);
    }
    while (oimage->next) {
      oimage = oimage->next;
      ierr = PetscStrcmp(filename,oimage->filename,&flg);CHKERRQ(ierr);
      if (flg) {
        oimage->count = count;
        PetscFunctionReturn(0);
      }
    }
    ierr = PetscNew(&image);CHKERRQ(ierr);
    oimage->next = image;
  } else {
    ierr = PetscRegisterFinalize(PetscImageListDestroy);CHKERRQ(ierr);
    ierr = PetscNew(&image);CHKERRQ(ierr);
    SAWs_images = image;
  }
  ierr = PetscStrallocpy(filename,&image->filename);CHKERRQ(ierr);
  ierr = PetscStrallocpy(ext,&image->ext);CHKERRQ(ierr);
  image->count = count;
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PetscDrawSave_SAWs"
static PetscErrorCode PetscDrawSave_SAWs(PetscDraw draw)
{
  PetscImageList image;
  char           body[4096];
  size_t         len = 0;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  if (!draw->savefilename) PetscFunctionReturn(0);
  ierr = PetscImageListAdd(draw->savefilename,draw->saveimageext,draw->savefilecount-1);CHKERRQ(ierr);
  image = SAWs_images;
  while (image) {
    const char *name = image->filename;
    const char *ext  = image->ext;
    if (draw->savesinglefile) {
      ierr = PetscSNPrintf(body+len,4086-len,"<img src=\"%s%s\" alt=\"None\">",name,ext);CHKERRQ(ierr);
    } else {
      ierr = PetscSNPrintf(body+len,4086-len,"<img src=\"%s/%s_%d%s\" alt=\"None\">",name,name,image->count,ext);CHKERRQ(ierr);
    }
    ierr = PetscStrlen(body,&len);CHKERRQ(ierr);
    image = image->next;
  }
  ierr = PetscStrcat(body,"<br>\n");CHKERRQ(ierr);
  if (draw->savefilecount > 0) PetscStackCallSAWs(SAWs_Pop_Body,("index.html",1));
  PetscStackCallSAWs(SAWs_Push_Body,("index.html",1,body));
  PetscFunctionReturn(0);
}

#endif