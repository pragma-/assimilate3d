/*==========================================================
  File:  time.c
  Author:  _pragma

  Description:  High-resolution time routines.
  ==========================================================*/

#include <sys/time.h>

#include "util.h"

int gt_curtime = 0, gt_realtime = 0, gt_fps = 0;

/*==========================================================
  Function:  T_getTime(void)
  Parameters:  none
  Returns: Milliseconds since engine started up.

  Description:  High-resolution time.
  ==========================================================*/
int T_getTime(void)
{
  struct timeval tp;
  struct timezone tz;
  static int base;

  gettimeofday(&tp, &tz);

  if(!base)
  {
    base = tp.tv_sec;
    return tp.tv_usec/1000;
  }

  gt_curtime = (tp.tv_sec - base) * 1000 + tp.tv_usec/1000;
  return gt_curtime;
}
