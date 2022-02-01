//
// shapeset.cc:
//
#include <stdio.h>
#include <X11/Xlib.h>
#include <local_types.h>
#include <list.h>
#include <xfontpair.h>
#include <widget.h>
#include <eedefs.h>
#include <shape.h>
#include <shapeset.h>

//
// コンストラクタ
//
ShapeSet::ShapeSet()
{
  shape_slot.set_virtual_link( LIST_REAL );
}

//
// load_file():
//
Boolean ShapeSet::load_file( FILE* fp )
{
  return True;
}

//
// save_shape():
//
Boolean ShapeSet::save_file( FILE* fp )
{
  for( int i = 0 ; i < shape_slot.count() ; i++ )
    shape_slot.get(i)->save( fp );
  return True;
}

//
// tex_file
//
Boolean ShapeSet::tex_file( FILE* fp )
{
  double max_x = 0, max_y = 0;
  for( int i = 0 ; i < shape_slot.count() ; i++ ){
    double x, y, w, h;
    shape_slot.get( i )->bound( x, y, w, h );
    if( x+w > max_x ) max_x = x+w;
    if( y+h > max_y ) max_y = y+h;
  }
  fprintf( fp, "\\begin{picture}(%d,%d)\n", max_x, max_y );
  for( i = 0 ; i < shape_slot.count() ; i++ )
    shape_slot.get( i )->tex( fp, max_y, 0, 0 );
  fprintf( fp, "\\end{picture}\n" );
  return True;
}

//
// fig_file():
//
Boolean ShapeSet::fig_file( FILE* fp )
{
  for( int i = 0 ; i < shape_slot.count() ; i++ )
    shape_slot.get(i)->xfig( fp, 0, 0 );
  return True;
}

//
// find_hit_shape():
//
Shape* ShapeSet::find_hit_shape( int x, int y )
{
  for( int hw = 1 ; hw < 16 ; hw++ )
    for( int i = 0 ; i < shape_slot.count() ; i++ )
      if( shape_slot.get( i )->hit( x, y, hw ) )
	return shape_slot.get( i );
  return NULL;
}

//
// draw_shape():
//
void ShapeSet::draw_shape( Window window, int x, int y )
{
  for( int i = 0 ; i < shape_slot.count() ; i++ )
    shape_slot.get( i )->draw( window, x, y );
}

//
// draw_shape_gp():
//
void ShapeSet::draw_shape_gp( Window window, int x, int y )
{
  for( int i = 0 ; i < shape_slot.count() ; i++ )
    shape_slot.get( i )->draw_gp( window, x, y );
}
