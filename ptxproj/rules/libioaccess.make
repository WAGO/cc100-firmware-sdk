# -*-makefile-*-
#
# Copyright (C) 2013 by <elrest>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBIOACCESS) += libioaccess

#
# Paths and names
#
LIBIOACCESS_VERSION	:= 0.0.1
LIBIOACCESS_MD5		:=
LIBIOACCESS		:= libioaccess
LIBIOACCESS_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, wago_intern/$(LIBIOACCESS))
LIBIOACCESS_BUILDROOT_DIR  := $(BUILDDIR)/$(LIBIOACCESS)
LIBIOACCESS_BUILDCONFIG    := Release
LIBIOACCESS_DIR            := $(LIBIOACCESS_BUILDROOT_DIR)/src
LIBIOACCESS_BUILD_DIR      := $(LIBIOACCESS_BUILDROOT_DIR)/bin/$(LIBIOACCESS_BUILDCONFIG)
LIBIOACCESS_LICENSE	:= WAGO
LIBIOACCESS_CONF_TOOL	:= NO
LIBIOACCESS_MAKE_ENV	:= $(CROSS_ENV) \
BUILDCONFIG=$(LIBIOACCESS_BUILDCONFIG) \
BIN_DIR=$(LIBIOACCESS_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/libioaccess.make)

LIBIOACCESS_PACKAGE_NAME             := $(LIBIOACCESS)_$(LIBIOACCESS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBIOACCESS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libioaccess.extract: 
	@$(call targetinfo)
	@mkdir -p $(LIBIOACCESS_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(LIBIOACCESS_DIR) ]; then \
	  ln -s $(LIBIOACCESS_SRC_DIR) $(LIBIOACCESS_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/libioaccess.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	@$(call world/prepare, LIBIOACCESS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libioaccess.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	@$(call world/compile, LIBIOACCESS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libioaccess.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(LIBIOACCESS_PKGDIR) && \
	  tar xvzf $(LIBIOACCESS_PLATFORMCONFIGPACKAGEDIR)/$(LIBIOACCESS_PACKAGE_NAME).tgz -C $(LIBIOACCESS_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, LIBIOACCESS)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(LIBIOACCESS_PLATFORMCONFIGPACKAGEDIR)
	@cd $(LIBIOACCESS_PKGDIR) && tar cvzf $(LIBIOACCESS_PLATFORMCONFIGPACKAGEDIR)/$(LIBIOACCESS_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libioaccess.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libioaccess)
	@$(call install_fixup, libioaccess,PRIORITY,optional)
	@$(call install_fixup, libioaccess,SECTION,base)
	@$(call install_fixup, libioaccess,AUTHOR,"<elrest>")
	@$(call install_fixup, libioaccess,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_lib, libioaccess, 0, 0, 0644, libioaccess)
	@$(call install_finish, libioaccess)
	@$(call touch)


# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libioaccess.clean:
	@$(call targetinfo)
	@-cd $(LIBIOACCESS_DIR) && \
		$(LIBIOACCESS_MAKE_ENV) $(LIBIOACCESS_PATH) $(MAKE) clean
	@$(call clean_pkg, LIBIOACCESS)

# vim: syntax=make
