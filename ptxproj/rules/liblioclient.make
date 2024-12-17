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
PACKAGES-$(PTXCONF_LIBLIOCLIENT) += liblioclient

#
# Paths and names
#
LIBLIOCLIENT_VERSION	:= 0.0.1
LIBLIOCLIENT_MD5		:=
LIBLIOCLIENT		:= liblioclient
LIBLIOCLIENT_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, wago_intern/$(LIBLIOCLIENT))
LIBLIOCLIENT_BUILDROOT_DIR  := $(BUILDDIR)/$(LIBLIOCLIENT)
LIBLIOCLIENT_BUILDCONFIG    := Release
LIBLIOCLIENT_DIR		    := $(LIBLIOCLIENT_BUILDROOT_DIR)/src
LIBLIOCLIENT_BUILD_DIR      := $(LIBLIOCLIENT_BUILDROOT_DIR)/bin/$(LIBLIOCLIENT_BUILDCONFIG)
LIBLIOCLIENT_LICENSE	:= WAGO
LIBLIOCLIENT_CONF_TOOL	:= NO
LIBLIOCLIENT_MAKE_ENV	:= $(CROSS_ENV) \
BUILDCONFIG=$(LIBLIOCLIENT_BUILDCONFIG) \
BIN_DIR=$(LIBLIOCLIENT_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/liblioclient.make)

LIBLIOCLIENT_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
LIBLIOCLIENT_PACKAGE_NAME := $(LIBLIOCLIENT)_$(LIBLIOCLIENT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/liblioclient.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBLIOCLIENT_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(LIBLIOCLIENT_DIR) ]; then \
	  ln -s $(LIBLIOCLIENT_SRC_DIR) $(LIBLIOCLIENT_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/liblioclient.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	@$(call world/prepare, LIBLIOCLIENT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/liblioclient.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	@$(call world/compile, LIBLIOCLIENT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/liblioclient.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(LIBLIOCLIENT_PKGDIR) && \
	  tar xvzf $(LIBLIOCLIENT_PLATFORMCONFIGPACKAGEDIR)/$(LIBLIOCLIENT_PACKAGE_NAME).tgz -C $(LIBLIOCLIENT_PKGDIR)
else
# normal mode, call "make install"
	@$(call world/install, LIBLIOCLIENT)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(LIBLIOCLIENT_PLATFORMCONFIGPACKAGEDIR)
	@cd $(LIBLIOCLIENT_PKGDIR) && tar cvzf $(LIBLIOCLIENT_PLATFORMCONFIGPACKAGEDIR)/$(LIBLIOCLIENT_PACKAGE_NAME).tgz *
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/liblioclient.targetinstall:
	@$(call targetinfo)

	@$(call install_init, liblioclient)
	@$(call install_fixup, liblioclient,PRIORITY,optional)
	@$(call install_fixup, liblioclient,SECTION,base)
	@$(call install_fixup, liblioclient,AUTHOR,"<elrest>")
	@$(call install_fixup, liblioclient,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_lib, liblioclient, 0, 0, 0644, liblioclient)
	@$(call install_finish, liblioclient)
	@$(call touch)


# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/liblioclient.clean:
	@$(call targetinfo)
	@-cd $(LIBLIOCLIENT_DIR) && \
		$(LIBLIOCLIENT_MAKE_ENV) $(LIBLIOCLIENT_PATH) $(MAKE) clean
	@$(call clean_pkg, LIBLIOCLIENT)

# vim: syntax=make
