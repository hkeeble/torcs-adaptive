##############################################################################
#
#    file                 : Makefile
#    created              : Sat Mar 18 23:28:27 CET 2000
#    copyright            : (C) 2000 by Eric Espie                         
#    email                : torcs@free.fr   
#    version              : $Id: Makefile,v 1.8 2002/11/30 19:00:50 torcs Exp $                                  
#
##############################################################################
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
##############################################################################

ROBOT = K1999

MODULE    = ${ROBOT}.so

MODULEDIR = drivers/${ROBOT}

SOURCES   = ${ROBOT}.cpp

SHIPDIR   = drivers/${ROBOT}

SHIP      = ${ROBOT}.xml logo.rgb

SHIPSUBDIRS = 1 2

PKGSUBDIRS  = $(SHIPSUBDIRS)

COMPILFLAGS  =	-Wno-deprecated

src-robots-K1999_PKGFILES	= $(shell find * -maxdepth 0 -type f -print)

src-robots-K1999_PKGDIR	= ${PACKAGE}-${VERSION}/$(subst ${TORCS_BASE},,$(shell pwd))

include ${MAKE_DEFAULT}
