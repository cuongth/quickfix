noinst_PROGRAMS = icmarketsclient

icmarketsclient_SOURCES = \
	icmarketsclient.cpp \
	FixApp.h \
	FixApp.cpp

icmarketsclient_LDADD = $(top_builddir)/src/C++/libquickfix.la

AM_CPPFLAGS = -I$(top_builddir)/include -I..

all-local:
	rm -f $(top_builddir)/bin/icmarketsclient
	ln -s ../examples/icmarketsclient/icmarketsclient $(top_builddir)/bin/icmarketsclient
