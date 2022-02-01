//
// geometry.cc:
//
#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>

#include <local_types.h>
#include <list.h>
#include <eedefs.h>
#include <geometry.h>

#define PAI 3.141592
#define RADIAN(x) (((x)/360.0)*PAI*2.0)

//
// normal_rect():
//
void normal_rect( int& x1, int& y1, int& x2, int& y2 )
{
  int save_x, save_y;
  if( x1 > x2 )
    save_x = x1, x1 = x2, x2 = save_x;
  if( y1 > y2 )
    save_y = y1, y1 = y2, y2 = save_y;
}

//
// normal_rect():
//
void normal_rect( double& x1, double& y1, double& x2, double& y2 )
{
  double save_x, save_y;
  if( x1 > x2 )
    save_x = x1, x1 = x2, x2 = save_x;
  if( y1 > y2 )
    save_y = y1, y1 = y2, y2 = save_y;
}

//
// get_angle():
//
double get_angle( double dx, double dy )
{
  double angle;
  if( dx != 0 ){
    angle = atan( dy/dx );
    if( dx < 0 ) angle += PAI;
  }else{
    angle = PAI/2;
    if( dy < 0 ) angle = -angle;
//    if( dy < 0 ) angle += 2*PAI;
  }
  return angle;
}

//
// iswap():
//
void iswap( int& a, int& b )
{
  int t = a;
  a = b;
  b = t;
}

//
// dswap():
//
void dswap( double& x, double& y )
{
  double tmp = x;
  x = y;
  y = tmp;
}

//
// check_on_the_line():
//
Boolean
check_on_the_line( int px, int py, int x1, int y1, int x2, int y2, int hw )
{
  int xx, yy, cp, sx, sy, ex, ey;

  sx = x1, sy = y1, ex = x2, ey = y2;
  if( x1 > x2 ) iswap( sx, ex );
  if( y1 > y2 ) iswap( sy, ey );

  xx = x2 - x1, yy = y2 - y1;

  if( x1 == x2 || y1 == y2 ){
    if( px < sx-hw || px > ex+hw ) return False;
    if( py < sy-hw || py > ey+hw ) return False;
    return True;
  }

  if( py < sy-hw || py > ey+hw ) return False;
  if( px < sx-hw || px > ex+hw ) return False;

  xx = x2 - x1, yy = y2 - y1;
  px -= x2, py -= y2;

  cp = ROUND(((double)yy * (double)px)/(double)xx);
  if( py < cp-hw || py > cp+hw ) return False;

  cp = ROUND(((double)xx * (double)py)/(double)yy);
  if( px < cp-hw || px > cp+hw ) return False;

  return True;
}

//
// check_intersect_rect():
//
Boolean check_intersect_rect( int x1, int y1, int w1, int h1,
			     int x2, int y2, int w2, int h2 )
{
  Boolean x_intersect = False;
  Boolean y_intersect = False;

  if( x1 >= x2 && x1 < x2+w2 )
    x_intersect = True;
  if( x2 >= x1 && x2 < x1+w1 )
    x_intersect = True;

  if( y1 >= y2 && y1 < y2+h2 )
    y_intersect = True;
  if( y2 >= y1 && y2 < y1+h1 )
    y_intersect = True;

  return x_intersect && y_intersect;
}

//
// contain_chk():
//
Boolean contain_chk( int mx, int my, int cx, int cy, int bw )
{
  if( cx >= mx-bw && cx < mx+bw && cy >= my-bw && cy < my+bw )
    return True;
  return False;
}
