#
# Ｘ−Ｗｉｎｄｏｗ版 ｅｅ → ｘｅｅ
#

#C++=/usr/local/bin/g++
#CC=/usr/local/bin/gcc

SUNX11_INC=/usr/X11R5/include
LINUXX11_INC=/usr/X386/include
SUN_INC=/usr/local/lib/gcc-lib/sparc-sun-sunos4.1/2.6.0/include
LINUX_INC=/usr/local/lib/gcc-lib/i486-linux/2.6.0/include

#C++FLAGS = -g -I. -I../classlib -I$(SUNX11_INC)
C++FLAGS = -g -I. -I../classlib -I$(LINUXX11_INC)

SHAPE_SRC = shape.cc arc.cc line.cc rect.cc oval.cc str.cc spline.cc group.cc
MISC_SRC = misc.cc geometry.cc load.cc make_panels.cc
SRCS = shapeset.cc board.cc main.cc version.cc $(SHAPE_SRC) $(MISC_SRC)
# load2.cc
.cc.o:
	g++ $(C++FLAGS) -c $<

OBJS = $(SRCS:%.cc=%.o)
CLASSLIB = ../classlib/libPclass.a

all: xee

xee: $(OBJS) $(CLASSLIB)
	g++ -o xee $(OBJS) $(CLASSLIB) -lXpm -lX11 -lm

depend:
#	makedepend -I $(SUN_INC) $(C++FLAGS) $(SRCS)
	makedepend -I $(LINUX_INC) $(C++FLAGS) $(SRCS)

clean:
	rm -f xee *.o

# DO NOT DELETE THIS LINE -- make depend depends on it.

shapeset.o: /usr/include/stdio.h /usr/include/features.h
shapeset.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
shapeset.o: /usr/X386/include/X11/Xlib.h /usr/include/sys/types.h
shapeset.o: /usr/include/linux/types.h /usr/X386/include/X11/X.h
shapeset.o: /usr/X386/include/X11/Xfuncproto.h
shapeset.o: /usr/X386/include/X11/Xosdefs.h
shapeset.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
shapeset.o: ../classlib/local_types.h ../classlib/list.h
shapeset.o: /usr/include/stdlib.h /usr/include/errno.h
shapeset.o: /usr/include/linux/errno.h
shapeset.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
shapeset.o: ../classlib/xfontpair.h ../classlib/widget.h eedefs.h shape.h
shapeset.o: shapeset.h
board.o: /usr/include/stdio.h /usr/include/features.h
board.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
board.o: /usr/include/errno.h /usr/include/linux/errno.h
board.o: /usr/include/sys/types.h /usr/include/linux/types.h
board.o: /usr/include/sys/stat.h /usr/include/linux/stat.h
board.o: /usr/include/stdlib.h
board.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
board.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
board.o: /usr/include/string.h /usr/X386/include/X11/Xlib.h
board.o: /usr/X386/include/X11/X.h /usr/X386/include/X11/Xfuncproto.h
board.o: /usr/X386/include/X11/Xosdefs.h /usr/X386/include/X11/Xutil.h
board.o: /usr/X386/include/X11/cursorfont.h ../classlib/local_types.h
board.o: ../classlib/list.h geometry.h eedefs.h ../classlib/xfontpair.h
board.o: ../classlib/skkserv.h ../classlib/skkfep.h ../classlib/widget.h
board.o: ../classlib/panel.h ../classlib/frame.h ../classlib/button.h
board.o: ../classlib/menubar.h ../classlib/tile.h ../classlib/text.h
board.o: ../classlib/scrollbar.h ../classlib/canvas.h ../classlib/check.h
board.o: shape.h shapeset.h board.h misc.h point.h line.h spline.h rect.h
board.o: oval.h str.h arc.h group.h ../classlib/xcontext.h bitmaps/cursor.xbm
main.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
main.o: /usr/include/_G_config.h /usr/include/signal.h
main.o: /usr/include/sys/types.h /usr/include/linux/types.h
main.o: /usr/include/linux/signal.h /usr/include/sys/stat.h
main.o: /usr/include/linux/stat.h /usr/include/string.h
main.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
main.o: /usr/X386/include/X11/Xlib.h /usr/X386/include/X11/X.h
main.o: /usr/X386/include/X11/Xfuncproto.h /usr/X386/include/X11/Xosdefs.h
main.o: /usr/X386/include/X11/Xutil.h ../classlib/local_types.h
main.o: ../classlib/list.h /usr/include/stdlib.h /usr/include/errno.h
main.o: /usr/include/linux/errno.h
main.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h geometry.h
main.o: eedefs.h ../classlib/xfontpair.h ../classlib/widget.h
main.o: ../classlib/scrollbar.h ../classlib/canvas.h ../classlib/panel.h
main.o: ../classlib/frame.h shape.h shapeset.h board.h ../classlib/xcontext.h
main.o: misc.h
shape.o: /usr/include/stdio.h /usr/include/features.h
shape.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
shape.o: /usr/include/math.h /usr/include/huge_val.h /usr/include/endian.h
shape.o: /usr/include/bytesex.h
shape.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
shape.o: /usr/include/values.h /usr/X386/include/X11/Xlib.h
shape.o: /usr/include/sys/types.h /usr/include/linux/types.h
shape.o: /usr/X386/include/X11/X.h /usr/X386/include/X11/Xfuncproto.h
shape.o: /usr/X386/include/X11/Xosdefs.h
shape.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
shape.o: ../classlib/local_types.h ../classlib/list.h /usr/include/stdlib.h
shape.o: /usr/include/errno.h /usr/include/linux/errno.h eedefs.h shape.h
shape.o: shapeset.h ../classlib/frame.h ../classlib/xcontext.h board.h
arc.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
arc.o: /usr/include/_G_config.h /usr/include/math.h /usr/include/huge_val.h
arc.o: /usr/include/endian.h /usr/include/bytesex.h
arc.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
arc.o: /usr/include/values.h
arc.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/limits.h
arc.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/syslimits.h
arc.o: /usr/X386/include/X11/Xlib.h /usr/include/sys/types.h
arc.o: /usr/include/linux/types.h /usr/X386/include/X11/X.h
arc.o: /usr/X386/include/X11/Xfuncproto.h /usr/X386/include/X11/Xosdefs.h
arc.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
arc.o: ../classlib/local_types.h ../classlib/list.h /usr/include/stdlib.h
arc.o: /usr/include/errno.h /usr/include/linux/errno.h eedefs.h shape.h
arc.o: shapeset.h ../classlib/frame.h ../classlib/xcontext.h board.h arc.h
arc.o: misc.h geometry.h xfig.h
line.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
line.o: /usr/include/_G_config.h /usr/include/stdlib.h
line.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
line.o: /usr/include/errno.h /usr/include/linux/errno.h
line.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
line.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/limits.h
line.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/syslimits.h
line.o: /usr/include/math.h /usr/include/huge_val.h /usr/include/endian.h
line.o: /usr/include/bytesex.h /usr/include/values.h
line.o: /usr/X386/include/X11/Xlib.h /usr/include/sys/types.h
line.o: /usr/include/linux/types.h /usr/X386/include/X11/X.h
line.o: /usr/X386/include/X11/Xfuncproto.h /usr/X386/include/X11/Xosdefs.h
line.o: ../classlib/local_types.h ../classlib/list.h eedefs.h shape.h
line.o: shapeset.h ../classlib/frame.h ../classlib/xcontext.h board.h point.h
line.o: line.h misc.h geometry.h xfig.h
rect.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
rect.o: /usr/include/_G_config.h /usr/include/stdlib.h
rect.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
rect.o: /usr/include/errno.h /usr/include/linux/errno.h
rect.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
rect.o: /usr/include/math.h /usr/include/huge_val.h /usr/include/endian.h
rect.o: /usr/include/bytesex.h /usr/include/values.h
rect.o: /usr/X386/include/X11/Xlib.h /usr/include/sys/types.h
rect.o: /usr/include/linux/types.h /usr/X386/include/X11/X.h
rect.o: /usr/X386/include/X11/Xfuncproto.h /usr/X386/include/X11/Xosdefs.h
rect.o: ../classlib/local_types.h ../classlib/list.h eedefs.h shape.h
rect.o: shapeset.h ../classlib/frame.h ../classlib/xcontext.h board.h point.h
rect.o: rect.h misc.h geometry.h xfig.h
oval.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
oval.o: /usr/include/_G_config.h /usr/include/stdlib.h
oval.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
oval.o: /usr/include/errno.h /usr/include/linux/errno.h
oval.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
oval.o: /usr/include/math.h /usr/include/huge_val.h /usr/include/endian.h
oval.o: /usr/include/bytesex.h /usr/include/values.h
oval.o: /usr/X386/include/X11/Xlib.h /usr/include/sys/types.h
oval.o: /usr/include/linux/types.h /usr/X386/include/X11/X.h
oval.o: /usr/X386/include/X11/Xfuncproto.h /usr/X386/include/X11/Xosdefs.h
oval.o: ../classlib/local_types.h ../classlib/list.h eedefs.h shape.h
oval.o: shapeset.h ../classlib/frame.h ../classlib/xcontext.h board.h point.h
oval.o: oval.h misc.h geometry.h xfig.h
str.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
str.o: /usr/include/_G_config.h /usr/include/stdlib.h
str.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
str.o: /usr/include/errno.h /usr/include/linux/errno.h
str.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
str.o: /usr/include/string.h /usr/include/math.h /usr/include/huge_val.h
str.o: /usr/include/endian.h /usr/include/bytesex.h /usr/include/values.h
str.o: /usr/X386/include/X11/Xlib.h /usr/include/sys/types.h
str.o: /usr/include/linux/types.h /usr/X386/include/X11/X.h
str.o: /usr/X386/include/X11/Xfuncproto.h /usr/X386/include/X11/Xosdefs.h
str.o: /usr/X386/include/X11/Xutil.h /usr/X386/include/X11/keysym.h
str.o: /usr/X386/include/X11/keysymdef.h ../classlib/local_types.h
str.o: ../classlib/list.h eedefs.h shape.h shapeset.h ../classlib/xfontpair.h
str.o: ../classlib/skkserv.h ../classlib/skkfep.h ../classlib/frame.h
str.o: ../classlib/xcontext.h board.h point.h str.h misc.h geometry.h xfig.h
spline.o: /usr/include/stdio.h /usr/include/features.h
spline.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
spline.o: /usr/include/stdlib.h
spline.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
spline.o: /usr/include/errno.h /usr/include/linux/errno.h
spline.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
spline.o: /usr/include/math.h /usr/include/huge_val.h /usr/include/endian.h
spline.o: /usr/include/bytesex.h /usr/include/values.h
spline.o: /usr/X386/include/X11/Xlib.h /usr/include/sys/types.h
spline.o: /usr/include/linux/types.h /usr/X386/include/X11/X.h
spline.o: /usr/X386/include/X11/Xfuncproto.h /usr/X386/include/X11/Xosdefs.h
spline.o: ../classlib/local_types.h ../classlib/list.h eedefs.h shape.h
spline.o: shapeset.h ../classlib/frame.h ../classlib/xcontext.h board.h
spline.o: point.h spline.h misc.h geometry.h xfig.h
group.o: /usr/include/stdio.h /usr/include/features.h
group.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
group.o: /usr/include/stdlib.h
group.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
group.o: /usr/include/errno.h /usr/include/linux/errno.h
group.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
group.o: /usr/include/math.h /usr/include/huge_val.h /usr/include/endian.h
group.o: /usr/include/bytesex.h /usr/include/values.h
group.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/limits.h
group.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/syslimits.h
group.o: /usr/X386/include/X11/Xlib.h /usr/include/sys/types.h
group.o: /usr/include/linux/types.h /usr/X386/include/X11/X.h
group.o: /usr/X386/include/X11/Xfuncproto.h /usr/X386/include/X11/Xosdefs.h
group.o: ../classlib/local_types.h ../classlib/list.h eedefs.h shape.h
group.o: shapeset.h ../classlib/frame.h ../classlib/xcontext.h board.h
group.o: group.h misc.h geometry.h xfig.h
misc.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
misc.o: /usr/include/_G_config.h /usr/include/math.h /usr/include/huge_val.h
misc.o: /usr/include/endian.h /usr/include/bytesex.h
misc.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
misc.o: /usr/include/values.h /usr/X386/include/X11/Xlib.h
misc.o: /usr/include/sys/types.h /usr/include/linux/types.h
misc.o: /usr/X386/include/X11/X.h /usr/X386/include/X11/Xfuncproto.h
misc.o: /usr/X386/include/X11/Xosdefs.h
misc.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
misc.o: /usr/X386/include/X11/Xutil.h ../classlib/local_types.h eedefs.h
misc.o: ../classlib/list.h /usr/include/stdlib.h /usr/include/errno.h
misc.o: /usr/include/linux/errno.h shape.h shapeset.h board.h
misc.o: ../classlib/xfontpair.h ../classlib/widget.h ../classlib/message.h
misc.o: ../classlib/button.h ../classlib/panel.h ../classlib/frame.h
misc.o: ../classlib/icon.h misc.h ../classlib/xcontext.h
misc.o: bitmaps/shadow_dont_know.xbm bitmaps/tabby.xbm
geometry.o: /usr/include/stdio.h /usr/include/features.h
geometry.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
geometry.o: /usr/include/math.h /usr/include/huge_val.h /usr/include/endian.h
geometry.o: /usr/include/bytesex.h
geometry.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
geometry.o: /usr/include/values.h /usr/X386/include/X11/Xlib.h
geometry.o: /usr/include/sys/types.h /usr/include/linux/types.h
geometry.o: /usr/X386/include/X11/X.h /usr/X386/include/X11/Xfuncproto.h
geometry.o: /usr/X386/include/X11/Xosdefs.h
geometry.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
geometry.o: ../classlib/local_types.h ../classlib/list.h
geometry.o: /usr/include/stdlib.h /usr/include/errno.h
geometry.o: /usr/include/linux/errno.h eedefs.h geometry.h
load.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
load.o: /usr/include/_G_config.h /usr/include/ctype.h
load.o: /usr/include/sys/types.h /usr/include/linux/types.h
load.o: /usr/include/sys/stat.h /usr/include/linux/stat.h
load.o: /usr/include/stdlib.h
load.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
load.o: /usr/include/errno.h /usr/include/linux/errno.h
load.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
load.o: /usr/include/string.h /usr/X386/include/X11/Xlib.h
load.o: /usr/X386/include/X11/X.h /usr/X386/include/X11/Xfuncproto.h
load.o: /usr/X386/include/X11/Xosdefs.h /usr/X386/include/X11/Xutil.h
load.o: /usr/X386/include/X11/cursorfont.h ../classlib/local_types.h
load.o: ../classlib/list.h geometry.h eedefs.h ../classlib/xfontpair.h
load.o: ../classlib/widget.h ../classlib/panel.h ../classlib/frame.h
load.o: ../classlib/button.h ../classlib/menubar.h ../classlib/tile.h
load.o: ../classlib/scrollbar.h ../classlib/canvas.h ../classlib/skkserv.h
load.o: ../classlib/skkfep.h shape.h shapeset.h board.h misc.h point.h line.h
load.o: spline.h rect.h oval.h str.h arc.h group.h ../classlib/xcontext.h
make_panels.o: /usr/include/stdio.h /usr/include/features.h
make_panels.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
make_panels.o: /usr/include/sys/types.h /usr/include/linux/types.h
make_panels.o: /usr/include/sys/stat.h /usr/include/linux/stat.h
make_panels.o: /usr/include/stdlib.h
make_panels.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
make_panels.o: /usr/include/errno.h /usr/include/linux/errno.h
make_panels.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
make_panels.o: /usr/include/string.h /usr/X386/include/X11/Xlib.h
make_panels.o: /usr/X386/include/X11/X.h /usr/X386/include/X11/Xfuncproto.h
make_panels.o: /usr/X386/include/X11/Xosdefs.h /usr/X386/include/X11/Xutil.h
make_panels.o: /usr/X386/include/X11/cursorfont.h ../classlib/local_types.h
make_panels.o: ../classlib/list.h geometry.h eedefs.h ../classlib/xfontpair.h
make_panels.o: ../classlib/skkserv.h ../classlib/skkfep.h
make_panels.o: ../classlib/widget.h ../classlib/panel.h ../classlib/frame.h
make_panels.o: ../classlib/button.h ../classlib/menubar.h ../classlib/tile.h
make_panels.o: ../classlib/text.h ../classlib/scrollbar.h
make_panels.o: ../classlib/canvas.h ../classlib/check.h ../classlib/icon.h
make_panels.o: ../classlib/message.h shape.h shapeset.h board.h misc.h
make_panels.o: point.h line.h spline.h rect.h oval.h str.h arc.h group.h
make_panels.o: ../classlib/xcontext.h panel_bitmaps.h bitmaps/move.xbm
make_panels.o: bitmaps/delete.xbm bitmaps/copy.xbm bitmaps/resize.xbm
make_panels.o: bitmaps/reversx.xbm bitmaps/reversy.xbm bitmaps/update.xbm
make_panels.o: bitmaps/arc.xbm bitmaps/group.xbm bitmaps/ungroup.xbm
make_panels.o: bitmaps/line.xbm bitmaps/oval.xbm bitmaps/rectangle.xbm
make_panels.o: bitmaps/spline.xbm bitmaps/string.xbm bitmaps/rotate.xbm
make_panels.o: bitmaps/rscapsule.xbm bitmaps/rsnone.xbm bitmaps/rsrect.xbm
make_panels.o: bitmaps/lssolid.xbm bitmaps/lsdot.xbm bitmaps/lsdash.xbm
make_panels.o: bitmaps/lmsolid.xbm bitmaps/lmfarrow.xbm bitmaps/lmrarrow.xbm
make_panels.o: bitmaps/lmbarrow.xbm bitmaps/lmmmassoc.xbm
make_panels.o: bitmaps/lmomassoc.xbm bitmaps/magnet_on.xbm
make_panels.o: bitmaps/magnet_off.xbm bitmaps/cross_cursor.xbm
make_panels.o: bitmaps/full_cursor.xbm bitmaps/width_one.xbm
make_panels.o: bitmaps/width_two.xbm bitmaps/width_three.xbm
make_panels.o: bitmaps/fill_on.xbm bitmaps/fill_off.xbm bitmaps/tex_lion.xbm
make_panels.o: pixmaps/fig.xpm pixmaps/file_server.xpm
