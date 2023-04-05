# -*-makefile-*-
#
# Copyright (C) 2003-2006 by Robert Schwebel <r.schwebel@pengutronix.de>
#               2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_FREETYPE) += freetype

#
# Paths and names
#
FREETYPE_VERSION	:= 2.10.2
FREETYPE_MD5		:= 7c0d5a39f232d7eb9f9d7da76bf08074
FREETYPE		:= freetype-$(FREETYPE_VERSION)
FREETYPE_SUFFIX		:= tar.xz
FREETYPE_SOURCE		:= $(SRCDIR)/$(FREETYPE).$(FREETYPE_SUFFIX)
FREETYPE_DIR		:= $(BUILDDIR)/$(FREETYPE)
FREETYPE_LICENSE	:= BSD-2-Clause, FTL, GPL-2.0+
FREETYPE_LICENSE_FILES	:= \
	file://docs/LICENSE.TXT;md5=4af6221506f202774ef74f64932878a1 \
	file://docs/GPLv2.TXT;md5=8ef380476f642c20ebf40fecb0add2ec \
	file://docs/FTL.TXT;md5=13b25413274c9b3b09b63e4028216ff4 \
	file://src/bdf/README;startline=98;endline=140;md5=d0c2c2e2e102c393a12869bc34515be2 \
	file://src/pcf/README;startline=69;endline=88;md5=e0f11f550450e58753f2d54ddaf17d34

FREETYPE_URL := \
	http://download.savannah.gnu.org/releases/freetype/$(FREETYPE).$(FREETYPE_SUFFIX) \
	http://download.savannah.gnu.org/releases/freetype/freetype-old/$(FREETYPE).$(FREETYPE_SUFFIX)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
FREETYPE_CONF_TOOL	:= autoconf
FREETYPE_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-static \
	--disable-biarch-config \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--enable-mmap \
	--with-zlib \
	--without-bzip2 \
	--without-png \
	--without-harfbuzz \
	--without-old-mac-fonts \
	--without-fsspec \
	--without-fsref \
	--without-quickdraw-toolbox \
	--without-quickdraw-carbon \
	--without-ats


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/freetype.targetinstall:
	@$(call targetinfo)

	@$(call install_init, freetype)
	@$(call install_fixup, freetype,PRIORITY,optional)
	@$(call install_fixup, freetype,SECTION,base)
	@$(call install_fixup, freetype,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, freetype,DESCRIPTION,missing)

	@$(call install_lib, freetype, 0, 0, 0644, libfreetype)

	@$(call install_finish, freetype)

	@$(call touch)

# vim: syntax=make
