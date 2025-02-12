# -*-makefile-*-
#
# Copyright (C) 2022 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PLCHANDLER) += plchandler

#
# Paths and names
#
PLCHANDLER_VERSION        := 3.5.19.50
PLCHANDLER                := PLCHandler_SDK_Linux_$(PLCHANDLER_VERSION)
PLCHANDLER_URL            := $(call jfrog_template_to_url, PLCHANDLER)
PLCHANDLER_SUFFIX         := $(suffix $(PLCHANDLER_URL))
PLCHANDLER_MD5             = $(shell [ -f $(PLCHANDLER_MD5_FILE) ] && cat $(PLCHANDLER_MD5_FILE))
PLCHANDLER_ARTIFACT        = $(call jfrog_get_filename,$(PLCHANDLER_URL))
PLCHANDLER_SRC            := wago_intern/artifactory_sources
PLCHANDLER_BUILDROOT_DIR  := $(BUILDDIR)/$(PLCHANDLER)
PLCHANDLER_MD5_FILE       := $(PLCHANDLER_SRC)/$(PLCHANDLER)$(PLCHANDLER_SUFFIX).md5
PLCHANDLER_LIB_NAME       := libCmpPLCHandler
# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/plchandler.get:
	@$(call targetinfo)
	@mkdir -p $(PLCHANDLER_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES   
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
	'$(PLCHANDLER_URL)' $(PLCHANDLER_SRC)/$(PLCHANDLER)$(PLCHANDLER_SUFFIX) '$(PLCHANDLER_MD5_FILE)'
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/plchandler.extract: 
	@$(call targetinfo)
	@mkdir -p $(PLCHANDLER_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@unzip -o $(PLCHANDLER_SRC)/$(PLCHANDLER)$(PLCHANDLER_SUFFIX) -d $(PLCHANDLER_BUILDROOT_DIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/plchandler.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PLCHANDLER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/plchandler.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PLCHANDLER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/plchandler.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	mkdir -p $(PTXDIST_SYSROOT_TARGET)/usr/include/plchandler
	mkdir -p $(PTXDIST_SYSROOT_TARGET)/usr/lib/plchandler
	
	cp $(PLCHANDLER_BUILDROOT_DIR)/Include/Linux/*.h $(PTXDIST_SYSROOT_TARGET)/usr/include/plchandler
	cp $(PLCHANDLER_BUILDROOT_DIR)/Include/*.h $(PTXDIST_SYSROOT_TARGET)/usr/include/plchandler

ifeq ($(PTXCONF_ARCH_STRING),arm)
	cp $(PLCHANDLER_BUILDROOT_DIR)/bin/Linux/arm/$(PLCHANDLER_LIB_NAME)-arm.a $(PTXDIST_SYSROOT_TARGET)/usr/lib/$(PLCHANDLER_LIB_NAME).a
endif

ifeq ($(PTXCONF_ARCH_STRING),arm64)
	cp $(PLCHANDLER_BUILDROOT_DIR)/bin/Linux/arm64/$(PLCHANDLER_LIB_NAME)-arm64.a $(PTXDIST_SYSROOT_TARGET)/usr/lib/$(PLCHANDLER_LIB_NAME).a
endif

ifeq ($(PTXCONF_ARCH_STRING),x86)
	cp $(PLCHANDLER_BUILDROOT_DIR)/bin/Linux/x86/$(PLCHANDLER_LIB_NAME)-x86.a $(PTXDIST_SYSROOT_TARGET)/usr/lib/$(PLCHANDLER_LIB_NAME).a
endif

ifeq ($(PTXCONF_ARCH_STRING),x86_64)
	cp $(PLCHANDLER_BUILDROOT_DIR)/bin/Linux/x64/$(PLCHANDLER_LIB_NAME)-x64.a $(PTXDIST_SYSROOT_TARGET)/usr/lib/$(PLCHANDLER_LIB_NAME).a
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/plchandler.targetinstall:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/plchandler.clean:
	@$(call targetinfo)
	@$(call clean_pkg, PLCHANDLER)

	@rm -rf $(PLCHANDLER_BUILDROOT_DIR)
	@rm -rf $(PTXDIST_SYSROOT_TARGET)/usr/include/plchandler
	@rm -rf $(PTXDIST_SYSROOT_TARGET)/usr/lib/$(PLCHANDLER_LIB_NAME).a

# vim: syntax=make
