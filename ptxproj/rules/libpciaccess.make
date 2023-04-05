# -*-makefile-*-
#
# Copyright (C) 2008 by Juergen Beisert <jbe@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBPCIACCESS) += libpciaccess

#
# Paths and names
#
LIBPCIACCESS_VERSION	:= 0.16
LIBPCIACCESS_MD5	:= b34e2cbdd6aa8f9cc3fa613fd401a6d6
LIBPCIACCESS		:= libpciaccess-$(LIBPCIACCESS_VERSION)
LIBPCIACCESS_SUFFIX	:= tar.bz2
LIBPCIACCESS_URL	:= $(call ptx/mirror, XORG, individual/lib/$(LIBPCIACCESS).$(LIBPCIACCESS_SUFFIX))
LIBPCIACCESS_SOURCE	:= $(SRCDIR)/$(LIBPCIACCESS).$(LIBPCIACCESS_SUFFIX)
LIBPCIACCESS_DIR	:= $(BUILDDIR)/$(LIBPCIACCESS)
LIBPCIACCESS_LICENSE	:= MIT

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBPCIACCESS_CONF_ENV	:= $(CROSS_ENV)

ifdef PTXCONF_LIBPCIACCESS_MTRR
LIBPCIACCESS_CONF_ENV += ac_cv_file__usr_include_asm_mtrr_h=yes
else
LIBPCIACCESS_CONF_ENV += ac_cv_file__usr_include_asm_mtrr_h=no
endif

#
# autoconf
#
LIBPCIACCESS_CONF_TOOL	:= autoconf
LIBPCIACCESS_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-strict-compilation \
	--disable-linux-rom-fallback \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--$(call ptx/wwo, PTXCONF_LIBPCIACCESS_ZLIB)-zlib

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libpciaccess.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libpciaccess)
	@$(call install_fixup, libpciaccess,PRIORITY,optional)
	@$(call install_fixup, libpciaccess,SECTION,base)
	@$(call install_fixup, libpciaccess,AUTHOR,"Juergen Beisert <j.beisert@pengutronix.de>")
	@$(call install_fixup, libpciaccess,DESCRIPTION,missing)

	@$(call install_lib, libpciaccess, 0, 0, 0644, libpciaccess)

	@$(call install_finish, libpciaccess)

	@$(call touch)

# vim: syntax=make
