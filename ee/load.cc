/*****************************************************************************

 load.c:

*/
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include <local_types.h>
#include <list.h>

#include <geometry.h>

#include <eedefs.h>
#include <xfontpair.h>
#include <widget.h>

#include <panel.h>
#include <frame.h>
#include <button.h>
#include <menubar.h>
#include <tile.h>
#include <scrollbar.h>
#include <canvas.h>

#include <skkserv.h>
#include <skkfep.h>

#include <shape.h>
#include <shapeset.h>
#include <board.h>

#include <misc.h>

#include <point.h>
#include <line.h>
#include <spline.h>
#include <rect.h>
#include <oval.h>
#include <str.h>
#include <arc.h>
#include <group.h>

#include <xcontext.h>

#define PAI 3.141592
#define PAI_2 (PAI*2)
#define RADIAN(x) (((double)(x)/360.0)*PAI_2)

#define TKNCK(x) if( (x) != gettoken(fp) ){parse_error_msg();return False;}

static Frame* parent_frame;
static char* lvalue = 0;
static int line_count = 0;

static double scale_x = 1.0;
static double scale_y = 1.0;

static LineMode lm_conv( char* s );
static LineStyle ls_conv( char* s );
static RoundStyle rs_conv( char* s );
static Boolean bool_conv( char* s );
static FontSize font_size_conv( char* str );
static void parse_error_msg();

typedef enum{
  TKN_STR, TKN_BRS, TKN_BRE, TKN_PAS, TKN_PAE,
  TKN_COM, TKN_SEM, TKN_EQU, TKN_NOSUCH,
} TknType;

typedef enum{
  KWD_LINE, KWD_RECT, KWD_OVAL, KWD_GEOM, KWD_LINE_STYLE, KWD_RROT,
  KWD_LINE_MODE, KWD_FILL, KWD_ROUND_STYLE, KWD_NODE_COUNT, KWD_NODE,
  KWD_STRING, KWD_BORDER_WIDTH, KWD_FARROW, KWD_RARROW, KWD_BARROW,
  KWD_MMASSOC, KWD_OMASSOC, KWD_SOLID, KWD_DASH, KWD_DOT, KWD_ARC,
  KWD_CAPSULE, KWD_TRUE, KWD_FALSE, KWD_NONE, KWD_NOSUCH, KWD_GEOM2,
  KWD_GROUP, KWD_SPLINE, KWD_RESOLUTION, KWD_HIGH, KWD_LOW, KWD_ARROW_SIZE,
  KWD_BALL_SIZE, KWD_FONT_SIZE, KWD_tiny, KWD_scriptsize, KWD_footnotesize,
  KWD_small, KWD_normalsize, KWD_large, KWD_Large, KWD_LARGE, KWD_huge,
  KWD_Huge, KWD_LINE_WIDTH,
} KwdType;

static struct{
  unsigned char c;
  TknType token;
} tkn_table[] = {
  { '{', TKN_BRS },
  { '}', TKN_BRE },
  { '(', TKN_PAS },
  { ')', TKN_PAE },
  { ',', TKN_COM },
  { ';', TKN_SEM },
  { '=', TKN_EQU }
};

static struct{
  char* label;
  KwdType keyword;
} kwd_table[] = {
  { "line", KWD_LINE }, { "spline", KWD_SPLINE }, { "rect", KWD_RECT },
  { "oval", KWD_OVAL }, { "arc", KWD_ARC }, { "string", KWD_STRING },
  { "geom", KWD_GEOM }, { "geom2", KWD_GEOM2 },
  { "line_style", KWD_LINE_STYLE }, { "line_mode", KWD_LINE_MODE },
  { "fill", KWD_FILL }, { "round_style", KWD_ROUND_STYLE },
  { "node_count", KWD_NODE_COUNT }, { "node", KWD_NODE },
  { "string", KWD_STRING }, { "border_width", KWD_BORDER_WIDTH },
  { "farrow", KWD_FARROW }, { "rarrow", KWD_RARROW }, { "barrow", KWD_BARROW },
  { "mmassoc", KWD_MMASSOC }, { "omassoc", KWD_OMASSOC },
  { "solid", KWD_SOLID }, { "dash", KWD_DASH }, { "dot", KWD_DOT },
  { "capsule", KWD_CAPSULE }, { "true", KWD_TRUE }, { "false", KWD_FALSE },
  { "rrot", KWD_RROT }, { "none", KWD_NONE }, { "group", KWD_GROUP },
  { "resolution", KWD_RESOLUTION }, { "high", KWD_HIGH }, { "low", KWD_LOW },
  { "arrow_size", KWD_ARROW_SIZE }, { "ball_size", KWD_BALL_SIZE },
  { "font_size", KWD_FONT_SIZE }, { "tiny", KWD_tiny },
  { "scriptsize", KWD_scriptsize }, { "footnotesize", KWD_footnotesize },
  { "small", KWD_small }, { "normalsize", KWD_normalsize },
  { "large", KWD_large }, { "Large", KWD_Large }, { "LARGE", KWD_LARGE },
  { "huge", KWD_huge }, { "Huge", KWD_Huge }, { "line_width", KWD_LINE_WIDTH }
};

/*****************************************************************************

 tkn_search():

*/
static TknType tkn_search( char c )
{
  int i;
  for( i = 0 ; i < NUMBER(tkn_table) ; i++ ){
    if( tkn_table[i].c == c )
      return tkn_table[i].token;
  }
  return TKN_NOSUCH;
}

/****************************************************************************

 kwd_search():

*/
static KwdType kwd_search( char* label )
{
  int i;
  for( i = 0 ; i < NUMBER(kwd_table) ; i++ ){
    if( strcmp( kwd_table[i].label, label ) == 0 )
      return kwd_table[i].keyword;
  }
  return KWD_NOSUCH;
}

/*****************************************************************************

  getonechar(): ファイルからキャラクターを１バイト読み込む。

*/
static unsigned char getonechar( FILE* fp )
{
  static unsigned char buf[ 1024 ];
  static int rpos = -1;

  if( rpos < 0 ){
    while(1){
      line_count++;
      if( fgets( (char*)buf, sizeof(buf), fp ) == NULL ) return 0;
      if( buf[ 0 ] != '#' ) break;
    }
    if( strlen( (char*)buf ) <= 1 ) rpos = -1;
    else rpos = 1;
    return buf[ 0 ];
  }else{
    char c = buf[ rpos++ ];
    if( !buf[ rpos ] ) rpos = -1;
    return c;
  }
}

static TknType gettoken( FILE* fp )
{
  static unsigned char last_char = 0;
  TknType token;
  char buf[ 128 ];
  char c;
  int  i;

  while(1){
    if( last_char ){
      c = last_char;
      last_char = 0;
    }else c = getonechar( fp );
    if( !c )
      return TKN_NOSUCH;
    if( c != ' ' && c != 9 && c != 0x0a && c != 0x0c )
      break;
  }

  if( (token = tkn_search( c )) != TKN_NOSUCH )
    return token;

  if( c == '"' ){
    for( i = 0 ; i < sizeof(buf) ; i++ ){
      buf[ i ] = getonechar( fp );
      if( buf[ i ] == '"' )
        break;
      if( (unsigned char)buf[ i ] & (unsigned char)0x80 )
	buf[ ++i ] = getonechar( fp );
    }
    buf[ i ] = 0;
    lvalue = buf;
    return TKN_STR;
  }else{
    for( i = 1, buf[ 0 ] = c ; i < sizeof(buf) ; i++ ){
      buf[ i ] = getonechar( fp );
      if( tkn_search( buf[i] ) != TKN_NOSUCH )
	break;
      if( !isalnum( buf[ i ] )&&( buf[ i ] != '_' )&&( buf[ i ] != '.' ) )
	break;
    }
    last_char = buf[ i ];
    buf[ i ] = 0;
    lvalue = buf;
    return TKN_STR;
  }
}

//
// load():
//
Boolean Board::load( char* fname )
{
  int major, minor;
  TknType token;
  FILE* fp;

  parent_frame = frame;

  if( (fp = fopen( fname, "r" )) == NULL ){
    if( batch_mode ){
      fprintf( stderr, "'%s' が読み込みオープンできません。\n", fname );
    }else{
      char buf[128];
      sprintf( buf, "'%s' が読み込みオープンできません。", fname );
      message_box( parent_frame, buf, " 確認 " );
    }
    return False;
  }

  if( shapeset.count_shape() > 0 ){
    char *msg = "現在のデータを削除してロードしますか？";
    char *s1 = "削除する";
    char *s2 = "上書きする";
    switch( selectABC_box( parent_frame, msg, s1, s2, "キャンセル" ) ){
    case 0:
      return False;
    case 1:
      shapeset.unlink_shape( LIST_TOP, 0, shapeset.count_shape() );
      break;
    case 2:
      break;
    }
  }

  if( fscanf( fp, "EE %d %d\n", &major, &minor ) == 2 ){
    Boolean ret;
    rewind(fp);
//    ret = load2( this, fp );
    fclose(fp);
    return ret;
  }

  Shape* shape;
  while( (token = gettoken(fp)) == TKN_STR ){
    switch( kwd_search(lvalue) ){
    case KWD_LINE:   shape = new Line( this );  break;
    case KWD_SPLINE: shape = new Spline( this ); break;
    case KWD_RECT:   shape = new Rect( this );   break;
    case KWD_OVAL:   shape = new Oval( this );   break;
    case KWD_ARC:    shape = new Arc( this );    break;
    case KWD_STRING: shape = new Str( this );    break;
    case KWD_GROUP:  shape = new Group( this );  break;
    case KWD_RESOLUTION:
      TKNCK(TKN_EQU);
      TKNCK(TKN_STR);
      switch(kwd_search(lvalue)){
      case KWD_HIGH:
/*
	if( BmCheckHighreso() ){
	  scale_x = scale_y = 1.0;
	}else{
	  scale_x = scale_y = 16.0/24.0;
	}
*/
	break;
      default:
/*
	if( BmCheckHighreso() ){
	  scale_x = scale_y = 24.0/16.0;
	}else{
	  scale_x = scale_y = 1.0;
	}
*/
	break;
      }
      TKNCK(TKN_SEM);
      shape = NULL;
      break;
    case KWD_NOSUCH:
      parse_error_msg();
      fclose(fp);
      return False;
    }
    if( shape != NULL ){
//      shape->scale( shape, scale_x, scale_y );
      if( !shape->load_file( fp ) ){
	parse_error_msg();
	fclose(fp);
	return False;
      }
      shapeset.append_shape( shape );
    }
  }
  fclose( fp );
  XClearArea( xcontext->get_dpy(), canvas->get_canvas_window(),
	     0, 0, 0, 0, True );
  return True;
}

//
//
Boolean Group::load_file( FILE* fp )
{
  Shape* shape;
  TknType token;
  TKNCK(TKN_BRS);
  while( (token = gettoken(fp)) == TKN_STR ){
    switch( kwd_search(lvalue) ){
    case KWD_LINE:   shape = new Line( board );   break;
    case KWD_SPLINE: shape = new Spline( board ); break;
    case KWD_RECT:   shape = new Rect( board );   break;
    case KWD_OVAL:   shape = new Oval( board );   break;
    case KWD_ARC:    shape = new Arc( board );    break;
    case KWD_STRING: shape = new Str( board );    break;
    case KWD_GROUP:  shape = new Group( board );  break;
    case KWD_GEOM:
      TKNCK(TKN_EQU);
      TKNCK(TKN_PAS);
      TKNCK(TKN_STR);
      _x = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      _y = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      _w = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      _h = atof(lvalue);
      TKNCK(TKN_PAE);
      TKNCK(TKN_SEM);
      shape = NULL;
      break;
    default:
      shape = NULL;
      break;
    }
    if( shape != NULL ){
      if( !shape->load_file( fp ) ){
	parse_error_msg();
	fclose(fp);
	return False;
      }
      shape_slot.append( shape );
    }
  }
  return True;
}

//
// load_file():
//
Boolean Line::load_file( FILE* fp )
{
  line_width = 1;
  TknType token;
  TKNCK(TKN_BRS);
  while( (token = gettoken(fp)) == TKN_STR ){
    switch( kwd_search(lvalue) ){
    case KWD_LINE_STYLE:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      line_style = ls_conv(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_LINE_MODE:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      line_mode = lm_conv(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_LINE_WIDTH:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      line_width = atoi(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_NODE_COUNT:
      TKNCK(TKN_EQU); TKNCK(TKN_STR); TKNCK(TKN_SEM);
      break;
    case KWD_ARROW_SIZE:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      arrow_size = atof(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_BALL_SIZE:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      ball_size = atof(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_NODE:
      TKNCK(TKN_EQU);
      while( (token = gettoken(fp)) != TKN_SEM ){
	Point* point;
	double x, y;
	TKNCK(TKN_STR);
	x = atof(lvalue);
	TKNCK(TKN_COM);
	TKNCK(TKN_STR);
	y = atof(lvalue);
	point_slot.append( new Point( x, y ) );
	TKNCK(TKN_PAE);
      }
      break;
    }
  }
  return True;
}

//
// load_file():
//
Boolean Spline::load_file( FILE* fp )
{
  line_width = 1;
  TknType token;
  TKNCK(TKN_BRS);
  while( (token = gettoken(fp)) == TKN_STR ){
    switch( kwd_search(lvalue) ){
    case KWD_LINE_STYLE:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      line_style = ls_conv(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_LINE_MODE:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      line_mode = lm_conv(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_LINE_WIDTH:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      line_width = atoi(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_NODE_COUNT:
      TKNCK(TKN_EQU); TKNCK(TKN_STR); TKNCK(TKN_SEM);
      break;
    case KWD_ARROW_SIZE:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      arrow_size = atof(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_BALL_SIZE:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      ball_size = atof(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_NODE:
      TKNCK(TKN_EQU);
      while( (token = gettoken(fp)) != TKN_SEM ){
	double x, y;
	TKNCK(TKN_STR);
	x = atof(lvalue);
	TKNCK(TKN_COM);
	TKNCK(TKN_STR);
	y = atof(lvalue);
	point_slot.append( new Point( x, y ) );
	TKNCK(TKN_PAE);
      }
      break;
    }
  }
  if( point_slot.count() > 0 )
    make_spline( &point_slot, &wpoint_slot);
  return True;
}

//
// load_file():
//
Boolean Rect::load_file( FILE* fp )
{
  line_width = 1;
  fill_mode = False;
  TknType token;
  TKNCK(TKN_BRS);
  while( (token = gettoken(fp)) == TKN_STR ){
    switch( kwd_search(lvalue) ){
    case KWD_LINE_STYLE:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      line_style = ls_conv(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_LINE_WIDTH:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      line_width = atoi(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_FILL:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      fill_mode = bool_conv(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_GEOM:
      TKNCK(TKN_EQU);
      TKNCK(TKN_PAS);
      TKNCK(TKN_STR);
      _x = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      _y = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      _w = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      _h = atof(lvalue);
      TKNCK(TKN_PAE);
      TKNCK(TKN_SEM);
      break;
    }
  }
  return True;
}

//
// load_file():
//
Boolean Oval::load_file( FILE* fp )
{
  line_style = LS_SOLID;
  line_width = 1;
  TknType token;
  TKNCK(TKN_BRS);
  while( (token = gettoken(fp)) == TKN_STR ){
    switch( kwd_search(lvalue) ){
    case KWD_FILL:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      fill_mode = bool_conv(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_LINE_STYLE:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      line_style = ls_conv(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_LINE_WIDTH:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      line_width = atoi(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_GEOM:
      TKNCK(TKN_EQU); TKNCK(TKN_PAS); TKNCK(TKN_STR);
      _x = atof(lvalue);
      TKNCK(TKN_COM); TKNCK(TKN_STR);
      _y = atof(lvalue);
      TKNCK(TKN_COM); TKNCK(TKN_STR);
      _w = atof(lvalue);
      TKNCK(TKN_COM); TKNCK(TKN_STR);
      _h = atof(lvalue);
      TKNCK(TKN_PAE); TKNCK(TKN_SEM);
      break;
    }
  }
  _x -= _w, _y -= _h;
  _w *= 2.0, _h *= 2.0;
  return True;
}

//
// load_file():
//
Boolean Arc::load_file( FILE* fp )
{
  int i;
  line_style = LS_SOLID;
  line_width = 1;
  Boolean geom2_f = False;
  double sdeg, cdeg, edeg;
  TknType token;
  TKNCK(TKN_BRS);
  while( (token = gettoken(fp)) == TKN_STR ){
    switch( kwd_search(lvalue) ){
    case KWD_LINE_MODE:
      TKNCK(TKN_EQU);
      TKNCK(TKN_STR);
      line_mode = lm_conv(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_LINE_STYLE:
      TKNCK(TKN_EQU);
      TKNCK(TKN_STR);
      line_style = ls_conv(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_LINE_WIDTH:
      TKNCK(TKN_EQU);
      TKNCK(TKN_STR);
      line_width = atoi(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_RROT:
      TKNCK(TKN_EQU);
      TKNCK(TKN_STR);
      rrot = bool_conv(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_GEOM:
      TKNCK(TKN_EQU);
      TKNCK(TKN_PAS);
      TKNCK(TKN_STR);
      _x = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      _y = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      r = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      sdeg = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      edeg = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      sdx = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      sdy = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      edx = atof(lvalue);
      TKNCK(TKN_COM);
      TKNCK(TKN_STR);
      edy = atof(lvalue);
      TKNCK(TKN_PAE);
      TKNCK(TKN_SEM);
      break;
    case KWD_ARROW_SIZE:
      TKNCK(TKN_EQU);
      TKNCK(TKN_STR);
      arrow_size = atof(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_BALL_SIZE:
      TKNCK(TKN_EQU);
      TKNCK(TKN_STR);
      ball_size = atof(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_GEOM2:
      geom2_f = True;
      TKNCK(TKN_EQU);
      TKNCK(TKN_PAS);
      for( i = 0 ; i < 3 ; i++ ){
	TKNCK(TKN_STR);
	xx[i] = atof( lvalue );
	TKNCK(TKN_COM);
	TKNCK(TKN_STR);
	yy[i] = atof( lvalue );
	if( i < 2 ){
	  TKNCK(TKN_COM);
	}
      }
      TKNCK(TKN_PAE);
      TKNCK(TKN_SEM);
      compute_geom();
      break;
    }
  }

  if( !geom2_f ){
    if( !rrot ){
      cdeg = (sdeg+edeg)/2.0;
    }else{
      cdeg = (sdeg+edeg)/2.0+180.0;
    }
    xx[0] = _x + r*cos(RADIAN(sdeg));
    yy[0] = _y + r*sin(RADIAN(sdeg));
    xx[1] = _x + r*cos(RADIAN(cdeg));
    yy[1] = _y + r*sin(RADIAN(cdeg));
    xx[2] = _x + r*cos(RADIAN(edeg));
    yy[2] = _y + r*sin(RADIAN(edeg));
  }
  return True;
}

//
// load_file():
//
Boolean Str::load_file( FILE* fp )
{
  line_style = LS_SOLID;
  line_width = 1;
  TknType token;
  TKNCK(TKN_BRS);
  while( (token = gettoken(fp)) == TKN_STR ){
    switch( kwd_search(lvalue) ){
    case KWD_STRING:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      strncpy( buf, lvalue, sizeof(buf)-1 );
      buf[ sizeof(buf)-1 ] = 0;
      TKNCK(TKN_SEM);
      break;
    case KWD_BORDER_WIDTH:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      border_width = atoi(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_FONT_SIZE:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      font_size = font_size_conv( lvalue );
      TKNCK(TKN_SEM);
      break;
    case KWD_ROUND_STYLE:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      round_style = rs_conv(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_LINE_STYLE:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      line_style = ls_conv(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_LINE_WIDTH:
      TKNCK(TKN_EQU); TKNCK(TKN_STR);
      line_width = atoi(lvalue);
      TKNCK(TKN_SEM);
      break;
    case KWD_GEOM:
      TKNCK(TKN_EQU); TKNCK(TKN_PAS); TKNCK(TKN_STR);
      _x = atof(lvalue);
      TKNCK(TKN_COM); TKNCK(TKN_STR);
      _y = atof(lvalue);
      TKNCK(TKN_COM); TKNCK(TKN_STR);
      _w = atof(lvalue);
      TKNCK(TKN_COM); TKNCK(TKN_STR);
      _h = atof(lvalue);
      TKNCK(TKN_PAE); TKNCK(TKN_SEM);
      break;
    }
  }
  return True;
}

/*****************************************************************************

 lm_conv():

*/
static LineMode lm_conv( char* s )
{
  switch( kwd_search(s) ){
  case KWD_FARROW: return LM_FARROW; break;
  case KWD_RARROW: return LM_RARROW; break;
  case KWD_BARROW: return LM_BARROW; break;
  case KWD_MMASSOC: return LM_MMASSOC; break;
  case KWD_OMASSOC: return LM_OMASSOC; break;
  case KWD_SOLID: return LM_SOLID; break;
  }
  return LM_SOLID;
}

/*****************************************************************************

 ls_conv():

*/
static LineStyle ls_conv( char* s )
{
  switch( kwd_search(s) ){
  case KWD_DASH: return LS_DASH; break;
  case KWD_DOT: return LS_DOT; break;
  case KWD_SOLID: return LS_SOLID; break;
  }
  return LS_SOLID;
}

/*****************************************************************************

 rs_conv():

*/
static RoundStyle rs_conv( char* s )
{
  switch( kwd_search(s) ){
  case KWD_RECT: return RS_RECT; break;
  case KWD_CAPSULE: return RS_CAPSULE; break;
  case KWD_NONE: return RS_NONE; break;
  }
  return RS_NONE;
}

/*****************************************************************************

 bool_conv():

*/
static Boolean bool_conv( char* s )
{
  switch( kwd_search(s) ){
  case KWD_TRUE: return True; break;
  case KWD_FALSE: return False; break;
  }
  return False;
}
 
/*****************************************************************************

 font_size_conv():
 
*/
static FontSize font_size_conv( char* str )
{
  switch( kwd_search(str) ){
  case KWD_tiny: return FONT_tiny; break;
  case KWD_normalsize: return FONT_normalsize; break;
  case KWD_scriptsize: return FONT_scriptsize; break;
  case KWD_footnotesize: return FONT_footnotesize; break;
  case KWD_small: return FONT_small; break;
  case KWD_large: return FONT_large; break;
  case KWD_Large: return FONT_Large; break;
  case KWD_LARGE: return FONT_LARGE; break;
  case KWD_huge: return FONT_huge; break;
  case KWD_Huge: return FONT_Huge; break;
  }
  return FONT_normalsize;
}

/*****************************************************************************

 parse_error_msg():

*/
static void parse_error_msg()
{
  char buf[128];
  sprintf(buf, "行番号 %d において、文法エラーが検出されました。", line_count);
  message_box( parent_frame, buf, " 確認 " );
}
