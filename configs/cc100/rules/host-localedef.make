# -*-makefile-*-
#
# Copyright (C) 2007 by Luotao Fu <lfu@pengutronix.de>
#               2008, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_LOCALEDEF) += host-localedef

#
# Paths and names
#
HOST_LOCALEDEF_VERSION	:= eglibc-2.14.1-r17443-ptx1
HOST_LOCALEDEF_MD5	:= bc9e33364767f361bd7b029a602e8cf4
HOST_LOCALEDEF		:= localedef-$(HOST_LOCALEDEF_VERSION)
HOST_LOCALEDEF_SUFFIX	:= tar.bz2
HOST_LOCALEDEF_URL	:= http://www.pengutronix.de/software/ptxdist/temporary-src/$(HOST_LOCALEDEF).$(HOST_LOCALEDEF_SUFFIX)
HOST_LOCALEDEF_SOURCE	:= $(SRCDIR)/$(HOST_LOCALEDEF).$(HOST_LOCALEDEF_SUFFIX)
HOST_LOCALEDEF_DIR	:= $(HOST_BUILDDIR)/$(HOST_LOCALEDEF)
HOST_LOCALEDEF_LICENSE	:= LGPL-2.1+, GPL-2.0+

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_LOCALEDEF_PATH	:= PATH=$(HOST_PATH)
HOST_LOCALEDEF_ENV     := $(HOST_ENV) \
       CFLAGS="-fgnu89-inline -g -O2"

#
# autoconf
#
HOST_LOCALEDEF_AUTOCONF	:= \
	--with-glibc=./eglibc \
	--prefix=/usr \
	CFLAGS="$(HOST_CFLAGS) -fgnu89-inline -g -O2"

HOST_LOCALEDEF_CFLAGS	:= -fgnu89-inline


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-localedef.install:
	@$(call targetinfo)
	cp $(HOST_LOCALEDEF_DIR)/localedef $(PTXCONF_SYSROOT_HOST)/bin
	@$(call touch)

# vim: syntax=make
