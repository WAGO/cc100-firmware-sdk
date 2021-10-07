# -*-makefile-*-
#
# Copyright (C) 2006 by Erwin Rol
#               2007, 2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
# Copyright (C) 2017 by Philipp Zabel <p.zabel@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBJPEG) += libjpeg

#
# Paths and names
#
LIBJPEG_VERSION	:= 1.5.2
LIBJPEG_MD5	:= 6b4923e297a7eaa255f08511017a8818
LIBJPEG_SUFFIX	:= tar.gz
LIBJPEG		:= libjpeg-turbo-$(LIBJPEG_VERSION)
LIBJPEG_TARBALL	:= $(LIBJPEG).$(LIBJPEG_SUFFIX)
LIBJPEG_URL	:= $(call ptx/mirror, SF, libjpeg-turbo/$(LIBJPEG_VERSION)/$(LIBJPEG_TARBALL))
LIBJPEG_SOURCE	:= $(SRCDIR)/$(LIBJPEG_TARBALL)
LIBJPEG_DIR	:= $(BUILDDIR)/$(LIBJPEG)
LIBJPEG_LICENSE	:= IJG, BSD-3-Clause, Zlib
LIBJPEG_LICENSE_FILES := \
	file://LICENSE.md;md5=f5bae2e0391ad876f09ae61ee2bcce69

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


LIBJPEG_SIMD :=
ifdef PTXCONF_ARCH_X86
LIBJPEG_SIMD := y
endif
ifdef PTXCONF_ARCH_ARM_NEON
LIBJPEG_SIMD := y
endif

LIBJPEG_CONF_TOOL := autoconf
LIBJPEG_CONF_OPT := \
	$(CROSS_AUTOCONF_USR) \
	--with-jpeg7 \
	--with-jpeg8 \
	--with-mem-srcdst \
	--without-arith-enc \
	--without-arith-dec \
	--without-12bit \
	--without-turbojpeg \
	--without-java \
	--without-gas-preprocessor \
	--$(call ptx/wwo, LIBJPEG_SIMD)-simd \
	--with-build-date=`date --utc --date @$(SOURCE_DATE_EPOCH) +%Y%m%d`

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libjpeg.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libjpeg)
	@$(call install_fixup, libjpeg,PRIORITY,optional)
	@$(call install_fixup, libjpeg,SECTION,base)
	@$(call install_fixup, libjpeg,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libjpeg,DESCRIPTION,missing)

	@$(call install_lib, libjpeg, 0, 0, 0644, libjpeg)
ifdef PTXCONF_LIBJPEG_BIN
	@$(call install_copy, libjpeg, 0, 0, 0755, -, /usr/bin/cjpeg)
	@$(call install_copy, libjpeg, 0, 0, 0755, -, /usr/bin/djpeg)
endif

	@$(call install_finish, libjpeg)

	@$(call touch)

# vim: syntax=make
