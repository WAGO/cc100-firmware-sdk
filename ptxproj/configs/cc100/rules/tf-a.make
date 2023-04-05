# -*-makefile-*-
#
# Copyright (C) 2018 by Rouven Czerwinski <r.czerwinski@pengutronix.de>
#               2019 by Ahmad Fatoum <a.fatoum@pengutronix.de>
#               2021 by Ralf Gliese <gliese@pelrest.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_TF_A) += tf-a

#
# Paths and names
#
TF_A_VERSION	:= $(call remove_quotes, $(PTXCONF_TF_A_VERSION))
TF_A_MD5	:= $(call remove_quotes,$(PTXCONF_TF_A_MD5))
TF_A		:= tf-a-$(TF_A_VERSION)
TF_A_SUFFIX	:= tar.gz
TF_A_URL	:= https://git.trustedfirmware.org/TF-A/trusted-firmware-a.git/snapshot/$(TF_A_VERSION).$(TF_A_SUFFIX)
TF_A_SOURCE	:= $(SRCDIR)/$(TF_A).$(TF_A_SUFFIX)
TF_A_DIR	:= $(BUILDDIR)/$(TF_A)
TF_A_BUILDDIR	:= $(TF_A_DIR)/build
#TF_A_BUILD_OOT	:= YES
TF_A_LICENSE	:= BSD-3-Clause AND BSD-2-Clause \
		   AND (GPL-2.0-or-later OR BSD-2-Clause) \
		   AND (NCSA OR MIT) \
		   AND Zlib \
		   AND (GPL-2.0-or-later OR BSD-3-Clause)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

TF_A_PLATFORMS		:= $(call remove_quotes, $(PTXCONF_TF_A_PLATFORMS))
TF_A_ARTIFACTS		:= $(call remove_quotes, $(PTXCONF_TF_A_ARTIFACTS))

TF_A_WRAPPER_BLACKLIST	:= \
	$(PTXDIST_LOWLEVEL_WRAPPER_BLACKLIST)

TF_A_PATH	:= PATH=$(CROSS_PATH)

TF_A_MAKE_OPT	:= \
	-C $(TF_A_DIR) \
	CROSS_COMPILE=$(BOOTLOADER_CROSS_COMPILE) \
	HOSTCC=$(HOSTCC) \
	PLAT=$(call remove_quotes, $(PTXCONF_TF_A_PLATFORMS)) \
	ARCH=$(call remove_quotes, $(PTXCONF_TF_A_ARCH_STRING)) \
	ARM_ARCH_MAJOR=$(call remove_quotes, $(PTXCONF_TF_A_ARM_ARCH_MAJOR)) \
	$(call remove_quotes,$(PTXCONF_TF_A_EXTRA_ARGS)) \
	all

ifdef PTXCONF_TF_A_BL32_TSP
TF_A_MAKE_OPT += ARM_TSP_RAM_LOCATION=$(PTXCONF_TF_A_BL32_TSP_RAM_LOCATION_STRING)
endif
ifdef PTXCONF_TF_A_ARM_ARCH_MINOR
TF_A_MAKE_OPT += ARM_ARCH_MINOR=$(PTXCONF_TF_A_ARM_ARCH_MINOR)
endif
ifdef PTXCONF_TF_A_BL32_SP_MIN
TF_A_MAKE_OPT += AARCH32_SP=sp_min
endif

ifdef PTXCONF_TF_A
ifeq ($(TF_A_ARTIFACTS),)
$(error TF_A_ARTIFACTS is empty. Nothing to install.)
endif
endif

TF_A_CONF_TOOL	:= NO

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

TF_A_MAKE_ENV	:= $(CROSS_ENV)

$(STATEDIR)/tf-a.compile:
	@$(call targetinfo)
#	cd $(TF_A_DIR) && make distclean && make $(TF_A_MAKE_OPT)
	cd $(TF_A_DIR) && make $(TF_A_MAKE_OPT)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

tf-a/inst_plat = $(foreach artifact, \
	$(wildcard $(TF_A_BUILDDIR)/$(1)/release/$(TF_A_ARTIFACTS)), \
	install -v -D -m 644 $(artifact) \
		$(2)/$(1)-$(notdir $(artifact))$(ptx/nl))

tf-a/inst_bins = $(foreach plat, $(TF_A_PLATFORMS), $(call tf-a/inst_plat,$(plat),$(1)))

$(STATEDIR)/tf-a.install:
	@$(call targetinfo)
	@$(call tf-a/inst_bins,$(TF_A_PKGDIR)/usr/lib/firmware)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tf-a.targetinstall:
	@$(call targetinfo)
	@$(call tf-a/inst_bins,$(IMAGEDIR))
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/tf-a.clean:
	@$(call targetinfo)
	@rm -vf $(addprefix $(IMAGEDIR)/, $(notdir $(TF_A_ARTIFACTS_SRC)))
	@$(call clean_pkg, TF_A)

# vim: syntax=make
