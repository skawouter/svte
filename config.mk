# svte: simple virtual terminal emulator: minimal, tabbed, VTE-based terminal
# Copyright 2010 svte contributors.
#
# This file is part of svte.
#
# svte is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


# mocicon version:
VERSION = 0.1.4

# Customize below to fit your system

# paths
PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man


# includes and libs

GTKINC=$(shell pkg-config --cflags gtk+-2.0 vte )
GTKLIB=$(shell pkg-config --libs gtk+-2.0 vte )

INCS = -I. -I/usr/include ${GTKINC}
LIBS = -L/usr/lib -lc ${GTKLIB}
# flags
CPPFLAGS = -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS}
CFLAGS = -mtune=native -std=gnu99 -O3 ${INCS} ${CPPFLAGS}
LDFLAGS = -s ${LIBS}

# Solaris
#CFLAGS = -fast ${INCS} -DVERSION=\"${VERSION}\"
#LDFLAGS = ${LIBS}

# compiler and linker
CC = cc
