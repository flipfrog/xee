#
#
all:
	(cd classlib;make)
	(cd ee;make)

clean:
	(cd classlib;make clean)
	(cd ee;make clean)

depend:
	(cd classlib;make depend)
	(cd ee;make depend)
