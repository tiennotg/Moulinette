#!/bin/bash

#
#  makefile.sh
#
#  This file is part of Moulinette.
#
#  Moulinette is free software: you can redistribute it and/or modify it under the terms of
#  the GNU General Public License as published by the Free Software Foundation, either
#  version 3 of the License, or (at your option) any later version.
# 
#  Moulinette is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
#  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE. See the GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License along with Moulinette.
#  If not, see <https://www.gnu.org/licenses/>.
#

gcc moulinette3.c filelist.c buttonbar.c fileitem.c fileutils.c digas.c process.c stringutils.c usagewindow.c logo.res -o moulinette31.exe -O3 -Wall -s -mwindows $(pkg-config gtk+-3.0 --cflags --libs)