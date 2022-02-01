//
// xfig.h:
//
#ifndef __xfig_h
#define __xfig_h

/* object type */

#define O_NONE         0
#define O_ELLIPSE      1
#define O_POLYLINE     2
#define O_SPLINE       3
#define O_TEXT         4
#define O_ARC          5
#define O_COMPOUND     6
#define O_END_COMPOUND (-O_COMPOUND)
#define O_ALL_OBJECT   99

/* line type */

#define SOLID_LINE  0
#define DASHED_LINE   1
#define DOTTED_LINE 2
#define RUBBER_LINE 3
#define PANEL_LINE  4

/* ellips, circle type */

#define T_ELLIPS_BY_RAD 1
#define T_ELLIPS_BY_DIA 2
#define T_CIRCLE_BY_RAD 3
#define T_CIRCLE_BY_DIA 4

/* fill type */

#define UNFILLED   0
#define WHITE_FILL 1
#define BLACK_FILL 2

/* arc type */

#define T_3_POINTS_ARC 1

/* path type? */

#define CLOSED_PATH    0
#define OPEN_PATH      1
#define DEF_BOXRADIUS  7
#define DEF_DASHLENGTH 4
#define DEF_DOTGAP     3

/* line type */

#define T_POLYLINE 1
#define T_BOX      2
#define T_POLYGON  3
#define T_ARC_BOX  4
#define T_EPS_BOX  5

/* text layout */

#define T_LEFT_JUSTIFIED   0
#define T_CENTER_JUSTIFIED 1
#define T_RIGHT_JUSTIFIED  2

/* text type */

#define RIGID_TEXT   1
#define SPECIAL_TEXT 2
#define PSFONT_TEXT  3
#define HIDDEN_TEXT  4

#endif /* __xfig_h */
