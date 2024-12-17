# -*-makefile-*-
#
# Copyright (C) 2021 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PP_ONBOARDIO) += pp_onboardio

#
# Paths and names
#
PP_ONBOARDIO_VERSION        := 1.0.0
PP_ONBOARDIO_MD5            :=
PP_ONBOARDIO_BASE           := pp_onboardio
PP_ONBOARDIO                := pp_onboardio-$(PP_ONBOARDIO_VERSION)
PP_ONBOARDIO_BUILDCONFIG    := Release
PP_ONBOARDIO_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_ONBOARDIO_BASE)
PP_ONBOARDIO_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_ONBOARDIO)
PP_ONBOARDIO_DIR            := $(PP_ONBOARDIO_BUILDROOT_DIR)/src
PP_ONBOARDIO_BUILD_DIR      := $(PP_ONBOARDIO_BUILDROOT_DIR)/bin/$(PP_ONBOARDIO_BUILDCONFIG)
PP_ONBOARDIO_LICENSE        := WAGO
PP_ONBOARDIO_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_ONBOARDIO_BUILDCONFIG) \
BIN_DIR=$(PP_ONBOARDIO_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_ONBOARDIO_PACKAGE_NAME             := $(PP_ONBOARDIO_BASE)_$(PP_ONBOARDIO_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_ONBOARDIO_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_onboardio.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_ONBOARDIO_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_ONBOARDIO_DIR) ]; then \
	  ln -s $(PP_ONBOARDIO_SRC_DIR) $(PP_ONBOARDIO_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PP_ONBOARDIO_CONF_TOOL	:= NO

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/pp_onboardio.prepare:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/pp_onboardio.compile:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_onboardio.install:
	@$(call targetinfo)
	
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: call "install"
	@$(call world/install, PP_ONBOARDIO)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_ONBOARDIO_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(PP_ONBOARDIO_PKGDIR) && tar cvzf $(PP_ONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/$(PP_ONBOARDIO_PACKAGE_NAME).tgz *
endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(PP_ONBOARDIO_PKGDIR) && \
	  tar xvzf $(PP_ONBOARDIO_PLATFORMCONFIGPACKAGEDIR)/$(PP_ONBOARDIO_PACKAGE_NAME).tgz -C $(PP_ONBOARDIO_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_onboardio.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_onboardio)
	@$(call install_fixup, pp_onboardio,PRIORITY,optional)
	@$(call install_fixup, pp_onboardio,SECTION,base)
	@$(call install_fixup, pp_onboardio,AUTHOR,"PEn - WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_onboardio,DESCRIPTION,missing)

ifdef PTXCONF_PP_ONBOARDIO_DAEMON
	@$(call install_copy, pp_onboardio, 0, 0, 0750, -, /usr/sbin/pp_onboardio)

# busybox init
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_PP_ONBOARDIO_DAEMON
	@$(call install_copy, pp_onboardio, 0, 0, 0700, -, /etc/init.d/pp_onboardio)

ifneq ($(call remove_quotes, $(PTXCONF_PP_ONBOARDIO_DAEMON_BBINIT_LINK)),)
	@$(call install_link, pp_onboardio, \
	  /etc/init.d/pp_onboardio, \
	  /etc/rc.d/$(PTXCONF_PP_ONBOARDIO_DAEMON_BBINIT_LINK))
endif
endif # ifdef PTXCONF_PP_ONBOARDIO_DAEMON
endif # PTXCONF_INITMETHOD_BBINIT

endif

	@$(call install_finish, pp_onboardio)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_onboardio.clean:
	@$(call targetinfo)
	@if [ -d $(PP_ONBOARDIO_DIR) ]; then \
	  $(PP_ONBOARDIO_MAKE_ENV) $(PP_ONBOARDIO_PATH) $(MAKE) $(MFLAGS) -C $(PP_ONBOARDIO_DIR) clean; \
	fi
	@$(call clean_pkg, PP_ONBOARDIO)
	@rm -rf $(PP_ONBOARDIO_BUILDROOT_DIR)

# vim: syntax=make
