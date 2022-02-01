/*****************************************************************************

 load2.c:

*/
#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <board.h>
#include <line.h>
#include <rect.h>
#include <oval.h>
#include <str.h>
#include <arc.h>
#include <group.h>

/*****************************************************************************

 next_str():

*/
static char* next_str( char* p )
{
  /* skip string */
  if( *p != ' ' ){
    for( ; *p != ' ' ; p++ )
      if( *p == 0 ) return NULL;
  }
  /* skip space */
  for( ; *p == ' ' ; p++ )
    ;
  return (*p==0)?NULL:p;
}

/*****************************************************************************

 load():

*/
Boolean load2( BoardClass* this, FILE* fp )
{
  char *p, buf[1024];
  int line_count = 0;
  int major_ver = 1, minor_ver = 0;
  ShapeClass* shape;

  /* EEディレクティブの有無をしらべる。*/

  if( fgets( buf, sizeof(buf), fp ) == NULL )
    return False;

  if( strncmp( buf, "EE", 2 ) == 0 ){
    p = next_str(buf+2);
    if( p == NULL ){
      fprintf( stderr, "parse error of EE derective.\n");
      return False;
    }
    major_ver = atoi(p);
    p = next_str(p);
    if( p == NULL ){
      fprintf( stderr, "parse error of EE derective.\n");
      return False;
    }
    minor_ver = atoi(p);
  }else{
    rewind( fp );
  }

  while( fgets( buf, sizeof(buf), fp ) != NULL ){
    int size;
    char* p;
    line_count++;
    if( strncmp( buf, "line", 4 ) == 0 ){
      /* 1.0 line <arrow> <assoc> points... */
      /* 1.1 line <arrow> <assoc> <mode> points... */
      PointClass* point;
      LineClassP* line;
      shape = Line__create();
      line = shape->private;
      this->shape_slot->append( this->shape_slot, shape );

      p = next_str(buf+4);
      switch( atoi(p) ){
      case 0: /* farrow */
	line->lm = LM_FARROW;
	break;
      case 1: /* rarrow */
	line->lm = LM_RARROW;
	break;
      case 2: /* barrow */
	line->lm = LM_BARROW;
	break;
      }

      p = next_str(p);
      switch( atoi(p) ){
      case 0: /* omassoc */
	line->lm = LM_OMASSOC;
	break;
      case 2: /* imassoc is skip... mmassoc */
	line->lm = LM_MMASSOC;
	break;
      }

      p = next_str(p);
      if( major_ver == 1 & minor_ver == 1 ){
	switch( atoi(p) ){
	case 1:
	  line->ls = LS_DOT;
	  break;
	case 2:
	  line->ls = LS_DASH;
	  break;
	}
	p = next_str(p);
      }

      while( p != NULL ){
	int x, y;
	x = atoi(p);
	if( (p = next_str(p)) == NULL ){
	  printf("line %d ignored...\n", line_count );
	  continue;
	}
	y = atoi(p);
	line->append_point( shape, Point__create( x, y ) );
	p = next_str(p);
      }

    }else if( strncmp( buf, "rect", 4 ) == 0 ){
      /* 1.0 rect x y w h */
      /* 1.1 rect <line mode> x y w h */
      RectClassP* rect;
      shape = Rect__create();
      rect = shape->private;
      this->shape_slot->append( this->shape_slot, shape );
      p = buf+4;
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored...\n", line_count );
	continue;
      }
      if( major_ver == 1 & minor_ver == 1 ){
	switch( atoi(p) ){
	case 1:
	  rect->ls = LS_DOT;
	  break;
	case 2:
	  rect->ls = LS_DASH;
	  break;
	}
	p = next_str(p);
      }
      rect->x = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      rect->y = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      rect->w = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      rect->h = atoi(p);
    }else if( strncmp( buf, "oval", 4 ) == 0 ){
      OvalClassP* oval;
      shape = Oval__create();
      oval = shape->private;
      this->shape_slot->append( this->shape_slot, shape );
      p = buf+4;
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      oval->fill = !atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      oval->x = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      oval->y = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      oval->w = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      oval->h = atoi(p);
    }else if( strncmp( buf, "string", 6 ) == 0 ){
      int x, y;
      StringClassP* string;
      p = buf+6;
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      x = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      y = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      p[ strlen(p)-1 ] = 0;
      shape = String__create( p, RS_NONE, FONT_normalsize, 0 );
      string = shape->private;
      string->x = x, string->y = y;
      this->shape_slot->append( this->shape_slot, shape );
    }else if( strncmp( buf, "rstring", 7 ) == 0 ){
      int x, y, bw;
      StringClassP* string;
      p = buf+7;
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      bw = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      x = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      y = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      p[ strlen(p)-1 ] = 0;
      shape = String__create( p, RS_RECT, FONT_normalsize, bw );
      string = shape->private;
      string->x = x, string->y = y;
      this->shape_slot->append( this->shape_slot, shape );
    }else if( strncmp( buf, "btnstring", 9 ) == 0 ){
      int x, y, bw;
      StringClassP* string;
      p = buf+9;
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      bw = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      x = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      y = atoi(p);
      if( (p = next_str(p)) == NULL ){
	printf("line %d ignored ...\n", line_count );
	continue;
      }
      p[ strlen(p)-1 ] = 0;
      shape = String__create( p, RS_CAPSULE, FONT_normalsize, bw );
      string = shape->private;
      string->x = x, string->y = y;
      this->shape_slot->append( this->shape_slot, shape );
    }else{
      printf("line %d ignored ...\n", line_count );
      continue;
    }
  }
  return False;
}
