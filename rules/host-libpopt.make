# -*-makefile-*-
#
# Copyright (C) 2019 by Bastian Krause <bst@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_LIBPOPT) += host-libpopt

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_LIBPOPT_CONF_TOOL	:= autoconf
HOST_LIBPOPT_CONF_OPT	:= \
	$(HOST_AUTOCONF) \
	--disable-build-gcov \
	--disable-nls \
	--disable-rpath

# vim: syntax=make
