# -*-makefile-*-
# $Id$
#
# Copyright (C) 2013 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CDS3_TSCCURL) += cds3-tsccurl

CDS3_TSCCURL_VERSION	 := 0.0.4
CDS3_TSCCURL              := TscCurl
CDS3_TSCCURL_DIR          := $(BUILDDIR)/$(CDS3_TSCCURL)
CDS3_TSCCURL_URL          := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCCURL)
CDS3_TSCCURL_SRC_DIR      := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_TSCCURL)
CDS3_TSCCURL_SRC          := $(CDS3_TSCCURL_SRC_DIR)/$(CDS3_TSCCURL)
CDS3_TSCCURL_BIN          := lib$(CDS3_TSCCURL).so.$(CDS3_TSCCURL_VERSION)

CDS3_TSCCURL_PACKAGE_NAME := cds3-tsccurl_$(CDS3_TSCCURL_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_TSCCURL_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
CDS3_TSCCURL_PACKAGE_DIR := $(PTXDIST_TEMPDIR)/package/$(CDS3_TSCCURL_PACKAGE_NAME)

#CDS3_IODRVDAL_CMP_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Components/
#CDS3_IODRVDAL_PLATFORM_INCLUDE:=$(PLCLINUXRT_V3_DIR)/Platforms/Linux/

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsccurl.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsccurl.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_TSCCURL_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_TSCCURL_SRC_DIR)/ $(CDS3_TSCCURL_DIR)/

endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-tsccurl.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(MAKE) -C $(CDS3_TSCCURL_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) itf
	$(MAKE) -C $(CDS3_TSCCURL_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) dep
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_TSCCURL_PATH      := PATH=$(CROSS_PATH)
CDS3_TSCCURL_MAKE_ENV  := $(CROSS_ENV)
CDS3_TSCCURL_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-tsccurl.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_TSCCURL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TSCCURL_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/TscComSwitch
$(STATEDIR)/cds3-tsccurl.install:
	@$(call targetinfo)

	@mkdir -p $(TSCCURL_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_TSCCURL_DIR)/*.h $(TSCCURL_SYSROOT_INCLUDES)/
	@cp $(CDS3_TSCCURL_DIR)/*Itf.h $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_TSCCURL_DIR) && tar cvzf $(CDS3_TSCCURL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCCURL_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_TSCCURL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCCURL_PACKAGE_NAME).tgz -C $(TSCCURL_SYSROOT_INCLUDES)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsccurl.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-tsccurl)

	@$(call install_fixup,cds3-tsccurl,PRIORITY,optional)
	@$(call install_fixup,cds3-tsccurl,SECTION,base)
	@$(call install_fixup,cds3-tsccurl,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-tsccurl,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	mkdir -p $(CDS3_TSCCURL_PACKAGE_DIR)
	cd $(CDS3_TSCCURL_PACKAGE_DIR) && \
	ar -xov $(CDS3_TSCCURL_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_TSCCURL_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-tsccurl, 0, 0, $(CDS3_TSCCURL_PACKAGE_DIR)/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-tsccurl, 0, 0, 0750, $(CDS3_TSCCURL_DIR)/$(CDS3_TSCCURL_BIN), /usr/lib/$(CDS3_TSCCURL_BIN))
	@$(call install_link, cds3-tsccurl, ./$(CDS3_TSCCURL_BIN), /usr/lib/lib$(CDS3_TSCCURL).so);
	@$(call install_link, cds3-tsccurl, ../$(CDS3_TSCCURL_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_TSCCURL).so);

endif
	@$(call install_finish,cds3-tsccurl)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_TSCCURL_PACKAGE_NAME).ipk $(CDS3_TSCCURL_PLATFORMCONFIGPACKAGEDIR)/
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-tsccurl.clean:
	rm -rf $(STATEDIR)/cds3-tsccurl.*
	rm -rf $(PKGDIR)/cds3-tsccurl_*
	rm -rf $(CDS3_TSCCURL_DIR)
