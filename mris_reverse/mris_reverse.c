
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "macros.h"
#include "error.h"
#include "diag.h"
#include "proto.h"
#include "mrisurf.h"
#include "mri.h"
#include "macros.h"

static char vcid[] = "$Id: mris_reverse.c,v 1.3 1999/08/13 23:13:10 fischl Exp $";

int main(int argc, char *argv[]) ;

static int  get_option(int argc, char *argv[]) ;
static void usage_exit(void) ;
static void print_usage(void) ;
static void print_help(void) ;
static void print_version(void) ;

char *Progname ;

static int patch_flag = 0 ;
static int which = REVERSE_X ;

int
main(int argc, char *argv[])
{
  char               **av, *in_fname, *out_fname, path[STRLEN], fname[STRLEN],
                     hemi[10], *cp ;
  int                ac, nargs ;
  MRI_SURFACE        *mris ;

  Progname = argv[0] ;
  ErrorInit(NULL, NULL, NULL) ;
  DiagInit(NULL, NULL, NULL) ;

  ac = argc ;
  av = argv ;
  for ( ; argc > 1 && ISOPTION(*argv[1]) ; argc--, argv++)
  {
    nargs = get_option(argc, argv) ;
    argc -= nargs ;
    argv += nargs ;
  }

  if (argc < 3)
    usage_exit() ;

  in_fname = argv[1] ;
  out_fname = argv[2] ;
  
  if (patch_flag)
  {
    FileNamePath(in_fname, path) ;
    FileNameOnly(in_fname, hemi) ;
    cp = strchr(hemi, '.') ;
    if (cp)
      *cp = 0 ;
    else
      ErrorExit(ERROR_BADPARM, "%s: could not scan hemisphere from %s\n",
                in_fname) ;
    sprintf(fname, "%s/%s.%s", path, hemi, ORIG_NAME) ;
    mris = MRISread(fname) ;
    if (!mris)
      ErrorExit(ERROR_NOFILE, "%s: could not read surface file %s",
                Progname, fname) ;
    if (MRISreadPatch(mris, in_fname) != NO_ERROR)
      ErrorExit(Gerror, "%s: could not read patch\n", Progname) ;
  }
  else
  {
    mris = MRISread(in_fname) ;
    if (!mris)
      ErrorExit(ERROR_NOFILE, "%s: could not read surface file %s",
                Progname, in_fname) ;
  }

  FileNamePath(out_fname, path) ;
  MRISreverse(mris, which) ;
  if (Gdiag & DIAG_SHOW)
    fprintf(stderr, "writing reversed surface to %s\n", out_fname) ;
  mris->type = MRIS_TRIANGULAR_SURFACE ;
  if (patch_flag)
    MRISwritePatch(mris, out_fname) ;
  else
    MRISwrite(mris, out_fname) ;
  exit(0) ;
  return(0) ;  /* for ansi */
}

/*----------------------------------------------------------------------
            Parameters:

           Description:
----------------------------------------------------------------------*/
static int
get_option(int argc, char *argv[])
{
  int  nargs = 0 ;
  char *option ;
  
  option = argv[1] + 1 ;            /* past '-' */
  if (!stricmp(option, "-help"))
    print_help() ;
  else if (!stricmp(option, "-version"))
    print_version() ;
  else switch (toupper(*option))
  {
  case '?':
  case 'U':
    print_usage() ;
    exit(1) ;
    break ;
  case 'P':
    patch_flag = 1 ;
    break ;
  case 'Y':
    which = REVERSE_Y ;
    break ;
  case 'Z':
    which = REVERSE_Y ;
    break ;
  default:
    fprintf(stderr, "unknown option %s\n", argv[1]) ;
    exit(1) ;
    break ;
  }

  return(nargs) ;
}

static void
usage_exit(void)
{
  print_usage() ;
  exit(1) ;
}

static void
print_usage(void)
{
  fprintf(stderr, 
          "usage: %s [options] <input surface> <output surface>\n", 
          Progname) ;
}

static void
print_help(void)
{
  print_usage() ;
  fprintf(stderr, "\nThis reverses a cortical surface\n") ;
  fprintf(stderr, "\nvalid options are:\n\n") ;
  exit(1) ;
}

static void
print_version(void)
{
  fprintf(stderr, "%s\n", vcid) ;
  exit(1) ;
}

