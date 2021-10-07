# -*-makefile-*-
#
# Copyright (C) 2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_ICU) += icu

#
# Paths and names
#
ICU_VERSION	:= 60.3
ICU_MD5		:= 4f558e73b7bd1fa93caf3d10479de41b
ICU		:= icu4c-$(subst .,_,$(ICU_VERSION))-src
ICU_SUFFIX	:= tgz
ICU_URL		:= https://github.com/unicode-org/icu/releases/download/release-$(subst .,-,$(ICU_VERSION))/$(ICU).$(ICU_SUFFIX)
ICU_SOURCE	:= $(SRCDIR)/$(ICU).$(ICU_SUFFIX)
ICU_DIR		:= $(BUILDDIR)/$(ICU)
ICU_SUBDIR	:= source
ICU_LICENSE	:= MIT AND Unicode-TOU AND public_domain
ICU_LICENSE_FILES := \
	file://LICENSE;md5=675f2d069434d8a1e4e6b0dcf4379226

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#ICU_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
ICU_CONF_TOOL	:= autoconf
ICU_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--disable-debug \
	--enable-release \
	--enable-shared \
	--disable-static \
	--enable-draft \
	--enable-renaming \
	--disable-tracing \
	--enable-dyload \
	--disable-rpath \
	--disable-weak-threads \
	--disable-extras \
	--enable-icuio \
	--disable-layout \
	--enable-tools \
	--disable-tests \
	--disable-samples \
	--with-cross-build=$(HOST_ICU_DIR)/$(ICU_SUBDIR) \
	--with-data-packaging=archive

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/icu.targetinstall:
	@$(call targetinfo)

	@$(call install_init, icu)
	@$(call install_fixup, icu,PRIORITY,optional)
	@$(call install_fixup, icu,SECTION,base)
	@$(call install_fixup, icu,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, icu,DESCRIPTION,missing)

	@$(call install_lib, icu, 0, 0, 0644, libicudata)
	@$(call install_lib, icu, 0, 0, 0644, libicui18n)
	@$(call install_lib, icu, 0, 0, 0644, libicuio)
	@$(call install_lib, icu, 0, 0, 0644, libicutu)
	@$(call install_lib, icu, 0, 0, 0644, libicuuc)

	@$(call install_copy, icu, 0, 0, 0644, -, \
		/usr/share/icu/$(ICU_VERSION)/icudt$(basename $(ICU_VERSION))$(call ptx/ifdef,PTXCONF_ENDIAN_LITTLE,l,b).dat)

	@$(call install_finish, icu)

	@$(call touch)

# vim: syntax=make
