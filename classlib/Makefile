#
SUNX11_INC=/usr/X11R5/include
LINUXX11_INC=/usr/X386/include
LINUX_INC=/usr/local/lib/gcc-lib/i486-linux/2.6.0/include
SUN_INC=/usr/local/lib/gcc-lib/sparc-sun-sunos4.1/2.6.0/include
SRCS = widget.cc button.cc xfontpair.cc check.cc tile.cc \
	message.cc icon.cc scrollbar.cc  menubar.cc text.cc \
	panel.cc frame.cc skkserv.cc skkfep.cc canvas.cc xcontext.cc

# text.cc
.cc.o:
	g++ $(C++FLAGS) -c $<

OBJS = $(SRCS:%.cc=%.o)
CLASSLIB=libPclass.a

#C++FLAGS = -I. -g -I$(SUNX11_INC)
C++FLAGS = -I. -g -I$(LINUX_INC)

all: xtest $(CLASSLIB)

$(CLASSLIB): $(OBJS)
	ar ruv $(CLASSLIB) $(OBJS)
	ranlib $(CLASSLIB)

xtest: test.o $(CLASSLIB)
	g++ -o xtest test.o $(CLASSLIB) -lXpm -lX11

test.o: test.cc

depend:
#	makedepend -I$(SUN_INC) $(C++FLAGS) $(SRCS)
	makedepend -I$(LINUX_INC) $(C++FLAGS) $(SRCS)

clean:
	rm -f libPclass.a *.o xtest *~

# DO NOT DELETE THIS LINE -- make depend depends on it.

widget.o: /usr/include/stdio.h /usr/include/features.h
widget.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
widget.o: /usr/include/errno.h /usr/include/linux/errno.h
widget.o: /usr/include/string.h
widget.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
widget.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
widget.o: /usr/include/linux/types.h /usr/include/X11/X.h
widget.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
widget.o: local_types.h list.h /usr/include/stdlib.h
widget.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h xfontpair.h
widget.o: widget.h
button.o: /usr/include/stdio.h /usr/include/features.h
button.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
button.o: /usr/include/ctype.h /usr/include/malloc.h
button.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
button.o: /usr/include/string.h /usr/include/X11/Xlib.h
button.o: /usr/include/sys/types.h /usr/include/linux/types.h
button.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
button.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h local_types.h
button.o: list.h /usr/include/stdlib.h /usr/include/errno.h
button.o: /usr/include/linux/errno.h
button.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h xfontpair.h
button.o: widget.h button.h
xfontpair.o: /usr/include/stdio.h /usr/include/features.h
xfontpair.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
xfontpair.o: /usr/include/stdlib.h
xfontpair.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
xfontpair.o: /usr/include/errno.h /usr/include/linux/errno.h
xfontpair.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
xfontpair.o: /usr/include/string.h /usr/include/X11/Xlib.h
xfontpair.o: /usr/include/sys/types.h /usr/include/linux/types.h
xfontpair.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
xfontpair.o: /usr/include/X11/Xosdefs.h local_types.h xfontpair.h
check.o: /usr/include/stdio.h /usr/include/features.h
check.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
check.o: /usr/include/malloc.h
check.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
check.o: /usr/include/string.h local_types.h list.h /usr/include/stdlib.h
check.o: /usr/include/errno.h /usr/include/linux/errno.h
check.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
check.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
check.o: /usr/include/linux/types.h /usr/include/X11/X.h
check.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h xfontpair.h
check.o: widget.h check.h
tile.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
tile.o: /usr/include/_G_config.h /usr/include/malloc.h
tile.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
tile.o: /usr/include/string.h /usr/include/X11/Xlib.h
tile.o: /usr/include/sys/types.h /usr/include/linux/types.h
tile.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
tile.o: /usr/include/X11/Xosdefs.h /usr/include/X11/xpm.h
tile.o: /usr/include/X11/Xutil.h local_types.h list.h /usr/include/stdlib.h
tile.o: /usr/include/errno.h /usr/include/linux/errno.h
tile.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h xfontpair.h
tile.o: widget.h tile.h
message.o: /usr/include/stdio.h /usr/include/features.h
message.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
message.o: /usr/include/malloc.h
message.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
message.o: /usr/include/string.h /usr/include/X11/Xlib.h
message.o: /usr/include/sys/types.h /usr/include/linux/types.h
message.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
message.o: /usr/include/X11/Xosdefs.h local_types.h list.h
message.o: /usr/include/stdlib.h /usr/include/errno.h
message.o: /usr/include/linux/errno.h
message.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
message.o: xfontpair.h widget.h message.h
icon.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
icon.o: /usr/include/_G_config.h /usr/include/stdlib.h
icon.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
icon.o: /usr/include/errno.h /usr/include/linux/errno.h
icon.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
icon.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
icon.o: /usr/include/linux/types.h /usr/include/X11/X.h
icon.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
icon.o: /usr/include/X11/xpm.h /usr/include/X11/Xutil.h local_types.h
icon.o: xfontpair.h widget.h icon.h
scrollbar.o: /usr/include/stdio.h /usr/include/features.h
scrollbar.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
scrollbar.o: /usr/include/ctype.h /usr/include/malloc.h
scrollbar.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
scrollbar.o: /usr/include/string.h /usr/include/X11/Xlib.h
scrollbar.o: /usr/include/sys/types.h /usr/include/linux/types.h
scrollbar.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
scrollbar.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h
scrollbar.o: local_types.h list.h /usr/include/stdlib.h /usr/include/errno.h
scrollbar.o: /usr/include/linux/errno.h
scrollbar.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
scrollbar.o: xfontpair.h widget.h scrollbar.h
menubar.o: /usr/include/stdio.h /usr/include/features.h
menubar.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
menubar.o: /usr/include/ctype.h /usr/include/malloc.h
menubar.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
menubar.o: /usr/include/string.h /usr/include/unistd.h
menubar.o: /usr/include/posix_opt.h /usr/include/gnu/types.h
menubar.o: /usr/include/sys/types.h /usr/include/linux/types.h
menubar.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
menubar.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
menubar.o: /usr/include/X11/Xutil.h local_types.h list.h
menubar.o: /usr/include/stdlib.h /usr/include/errno.h
menubar.o: /usr/include/linux/errno.h
menubar.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
menubar.o: xfontpair.h widget.h menubar.h
text.o: /usr/include/stdio.h /usr/include/features.h /usr/include/sys/cdefs.h
text.o: /usr/include/_G_config.h /usr/include/ctype.h /usr/include/malloc.h
text.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
text.o: /usr/include/string.h /usr/include/errno.h /usr/include/linux/errno.h
text.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
text.o: /usr/include/linux/types.h /usr/include/X11/X.h
text.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
text.o: /usr/include/X11/Xutil.h local_types.h list.h /usr/include/stdlib.h
text.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h xfontpair.h
text.o: widget.h skkserv.h skkfep.h text.h
panel.o: /usr/include/stdio.h /usr/include/features.h
panel.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
panel.o: /usr/include/malloc.h
panel.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
panel.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
panel.o: /usr/include/linux/types.h /usr/include/X11/X.h
panel.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
panel.o: local_types.h list.h /usr/include/stdlib.h /usr/include/errno.h
panel.o: /usr/include/linux/errno.h
panel.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h xfontpair.h
panel.o: widget.h panel.h frame.h
frame.o: /usr/include/stdio.h /usr/include/features.h
frame.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
frame.o: /usr/include/malloc.h
frame.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
frame.o: /usr/include/string.h /usr/include/X11/Xlib.h
frame.o: /usr/include/sys/types.h /usr/include/linux/types.h
frame.o: /usr/include/X11/X.h /usr/include/X11/Xfuncproto.h
frame.o: /usr/include/X11/Xosdefs.h /usr/include/X11/Xutil.h local_types.h
frame.o: list.h /usr/include/stdlib.h /usr/include/errno.h
frame.o: /usr/include/linux/errno.h
frame.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h xfontpair.h
frame.o: widget.h panel.h frame.h xcontext.h
skkserv.o: /usr/include/stdio.h /usr/include/features.h
skkserv.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
skkserv.o: /usr/include/unistd.h /usr/include/posix_opt.h
skkserv.o: /usr/include/gnu/types.h
skkserv.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
skkserv.o: /usr/include/sys/types.h /usr/include/linux/types.h
skkserv.o: /usr/include/stdlib.h /usr/include/errno.h
skkserv.o: /usr/include/linux/errno.h
skkserv.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
skkserv.o: /usr/include/memory.h /usr/include/string.h /usr/include/ctype.h
skkserv.o: /usr/include/sys/socket.h /usr/include/linux/socket.h
skkserv.o: /usr/include/linux/sockios.h /usr/include/netinet/in.h
skkserv.o: /usr/include/linux/in.h /usr/include/netdb.h /usr/include/paths.h
skkserv.o: local_types.h list.h skkserv.h
skkfep.o: /usr/include/stdio.h /usr/include/features.h
skkfep.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
skkfep.o: /usr/include/unistd.h /usr/include/posix_opt.h
skkfep.o: /usr/include/gnu/types.h
skkfep.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
skkfep.o: /usr/include/sys/types.h /usr/include/linux/types.h
skkfep.o: /usr/include/stdlib.h /usr/include/errno.h
skkfep.o: /usr/include/linux/errno.h
skkfep.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
skkfep.o: /usr/include/memory.h /usr/include/string.h /usr/include/ctype.h
skkfep.o: /usr/include/sys/socket.h /usr/include/linux/socket.h
skkfep.o: /usr/include/linux/sockios.h /usr/include/netinet/in.h
skkfep.o: /usr/include/linux/in.h /usr/include/netdb.h /usr/include/paths.h
skkfep.o: /usr/include/X11/Xlib.h /usr/include/X11/X.h
skkfep.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
skkfep.o: /usr/include/X11/Xutil.h /usr/include/X11/keysym.h
skkfep.o: /usr/include/X11/keysymdef.h local_types.h list.h xfontpair.h
skkfep.o: skkserv.h skkfep.h
canvas.o: /usr/include/stdio.h /usr/include/features.h
canvas.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
canvas.o: /usr/include/ctype.h /usr/include/malloc.h
canvas.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
canvas.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
canvas.o: /usr/include/linux/types.h /usr/include/X11/X.h
canvas.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
canvas.o: /usr/include/X11/Xutil.h local_types.h list.h /usr/include/stdlib.h
canvas.o: /usr/include/errno.h /usr/include/linux/errno.h
canvas.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h xfontpair.h
canvas.o: widget.h scrollbar.h canvas.h
xcontext.o: /usr/include/stdio.h /usr/include/features.h
xcontext.o: /usr/include/sys/cdefs.h /usr/include/_G_config.h
xcontext.o: /usr/include/string.h
xcontext.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/stddef.h
xcontext.o: /usr/include/errno.h /usr/include/linux/errno.h
xcontext.o: /usr/include/X11/Xlib.h /usr/include/sys/types.h
xcontext.o: /usr/include/linux/types.h /usr/include/X11/X.h
xcontext.o: /usr/include/X11/Xfuncproto.h /usr/include/X11/Xosdefs.h
xcontext.o: /usr/include/X11/Xutil.h local_types.h list.h
xcontext.o: /usr/include/stdlib.h
xcontext.o: /usr/local/lib/gcc-lib/i486-linux/2.6.0/include/float.h
xcontext.o: xfontpair.h widget.h button.h check.h tile.h message.h icon.h
xcontext.o: scrollbar.h skkserv.h skkfep.h text.h menubar.h canvas.h panel.h
xcontext.o: frame.h xcontext.h
