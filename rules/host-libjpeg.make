# -*-makefile-*-
#
# Copyright (C) 2018 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_LIBJPEG) += host-libjpeg

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_LIBJPEG_CONF_TOOL	:= autoconf
HOST_LIBJPEG_CONF_OPT := \
	$(HOST_AUTOCONF) \
	--with-jpeg7 \
	--with-jpeg8 \
	--with-mem-srcdst \
	--without-arith-enc \
	--without-arith-dec \
	--without-12bit \
	--without-turbojpeg \
	--without-java \
	--without-gas-preprocessor \
	--without-simd

# vim: syntax=make
