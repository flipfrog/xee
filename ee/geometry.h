//
// geometry.h:
//
#ifndef __geometry_h
#define __geometry_h

void normal_rect( int& x1, int& y1, int& x2, int& y2 );
void normal_rect( double& x1, double& y1, double& x2, double& y2 );
double get_angle( double dx, double dy );
void iswap( int& a, int& b );
void dswap( double& x, double& y );

Boolean check_on_the_line(int px, int py,
			  int x1, int y1,
			  int x2, int y2,
			  int hw );
Boolean check_intersect_rect(int x1, int y1,
			     int w1, int h1,
			     int x2, int y2,
			     int w2, int h2 );

Boolean contain_chk( int mx, int my, int cx, int cy, int bw );

#endif // __geometry_h
