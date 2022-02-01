//
// point.h:
//
#ifndef __point_h
#define __point_h

class Point {
public:
  double x, y;
  Point( double _x, double _y ){ x = _x, y = _y; }
  Point( Point* point ){ x = point->x, y = point->y; }
};

#endif /* __point_h */
