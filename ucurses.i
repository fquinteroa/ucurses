%module ucurses

%include "std_string.i"
%include "std_vector.i"
%include "typemaps.i"

%define array_in(T)
%typemap(perl5,in) T **{
   AV *tempav;
   SV **tv;
  if (!SvROK($input))
      croak("$input is not an array.");
   if (SvTYPE(SvRV($input)) != SVt_PVAV)
      croak("$input is not an array.");
   tempav = (AV*)SvRV($input);
   len = av_len(tempav);
   $1 = (T **) malloc(len*sizeof(char *));
   for (i = 0; i <= len; i++) {
      tv = av_fetch(tempav, i, 0);	
      T *ptr;
      if ((SWIG_ConvertPtr(av,(void **) &ptr,
           SWIGTYPE_p_ ## T,1)) == -1) return NULL;
      $1[i] = ptr;
   }
}

%typemap(perl5,freearg) T **{
    free($input);
}
%enddef

%{
#include "ucurses.h"
#include "ncurses.h"
int newMenu(array_in(T),int);
%}
%include "ucurses.h"
