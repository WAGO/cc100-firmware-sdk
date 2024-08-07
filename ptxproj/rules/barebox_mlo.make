# -*-makefile-*-
#
# Copyright (C) 2012 Jan Weitzel <j.weitzel@phytec.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_BAREBOX_MLO) += barebox_mlo

#
# Paths and names
#
BAREBOX_MLO_VERSION	:= $(call remove_quotes, \
		$(PTXCONF_BAREBOX_MLO_VERSION)-$(PTXCONF_BAREBOX_MLO_LOCALVERSION))
BAREBOX_MLO_SUFFIX := tgz
BAREBOX_MLO_MD5		:= $(call remove_quotes,$(PTXCONF_BAREBOX_MLO_MD5))
BAREBOX_MLO_MD5_FILE := src/barebox_mlo-$(BAREBOX_MLO_VERSION).$(BAREBOX_MLO_SUFFIX).md5
BAREBOX_MLO		:= barebox_mlo-$(BAREBOX_MLO_VERSION)
BAREBOX_MLO_URL := $(call jfrog_template_to_url, BAREBOX_MLO)
BAREBOX_MLO_DIR		:= $(BUILDDIR)/barebox_mlo-$(BAREBOX_MLO_VERSION)
BAREBOX_MLO_SOURCE	= $(SRCDIR)/$(BAREBOX_MLO).$(BAREBOX_MLO_SUFFIX)
BAREBOX_MLO_LICENSE	:= GPL-2.0

BAREBOX_MLO_CONFIG	:= $(call remove_quotes, \
		$(PTXDIST_PLATFORMCONFIGDIR)/$(PTXCONF_BAREBOX_MLO_CONFIG))

BAREBOX_MLO_PFC200_G1_MAX_SIZE := 61900

# ----------------------------------------------------------------------------
# Include
# ----------------------------------------------------------------------------
#include $(call ptx/in-path, PTXDIST_PATH, rules/other/definitions.make)
include $(call ptx/in-platformconfigdir, rules/other/definitions.make)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
$(BAREBOX_MLO_SOURCE):
	@$(call targetinfo)
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
		'$(BAREBOX_MLO_URL)' src/barebox_mlo-$(BAREBOX_MLO_VERSION).$(BAREBOX_MLO_SUFFIX) \
			'$(BAREBOX_MLO_MD5_FILE)'
	@$(call touch)
# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

BAREBOX_MLO_WRAPPER_BLACKLIST := \
	TARGET_HARDEN_RELRO \
	TARGET_HARDEN_BINDNOW \
	TARGET_HARDEN_PIE \
	TARGET_DEBUG \
	TARGET_BUILD_ID

BAREBOX_MLO_PATH	:= PATH=$(CROSS_PATH)
BAREBOX_MLO_ENV	    := KCONFIG_NOTIMESTAMP=1 \
 pkg_wrapper_blacklist="$(BAREBOX_MLO_WRAPPER_BLACKLIST)" 

BAREBOX_MLO_CONF_TOOL	:= kconfig
BAREBOX_MLO_CONF_OPT	:= \
			V=$(PTXDIST_VERBOSE) \
			HOSTCC=$(HOSTCC) \
			ARCH=$(PTXCONF_BAREBOX_MLO_ARCH_STRING) \
			CROSS_COMPILE=$(BOOTLOADER_CROSS_COMPILE)

#BAREBOX_MLO_MAKE_OPT	:= $(BAREBOX_MLO_CONF_OPT)
BAREBOX_MLO_MAKEVARS	:= $(BAREBOX_MLO_CONF_OPT)

ifdef PTXCONF_BAREBOX_MLO
$(BAREBOX_MLO_CONFIG):
	@echo
	@echo "****************************************************************************"
	@echo "*Please generate a barebox MLO config with 'ptxdist menuconfig barebox_mlo'*"
	@echo "****************************************************************************"
	@echo
	@echo
	@exit 1
endif

ifneq ($(wildcard $(BAREBOX_MLO_CONFIG)),)

$(STATEDIR)/barebox_mlo.prepare: $(BAREBOX_MLO_CONFIG)

else

$(STATEDIR)/barebox_mlo.prepare:
	@$(call targetinfo)

	@echo "Using defconfigs"

	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifeq ($(wildcard $(BAREBOX_MLO_CONFIG)),)

$(STATEDIR)/barebox_mlo.compile:
	@$(call targetinfo)

	@for i in "$(PTXCONF_BAREBOX_MLO_CONFIG)"; \
	do \
		$(call build_target, BAREBOX_MLO, $${i}); \
	done

	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/barebox_mlo.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/barebox_mlo.targetinstall:
	@$(call targetinfo)
	@for image in `ls $(BAREBOX_MLO_DIR)/images/barebox-*.img`; do \
		> $(IMAGEDIR)/MLO; \
		_suffix=$$(basename $$(echo $${image} | sed 's/-mlo.img//g')); \
		_suffix=$$(echo $${_suffix} | sed 's/barebox-//g'); \
		install -D -m644 $$image $(IMAGEDIR)/`basename $$image`; \
		if [ ! -e "$(IMAGEDIR)/MLO.$${_suffix}" ]; then \
			ln -sf `basename $$image` $(IMAGEDIR)/MLO.$${_suffix}; \
		fi; \
	done
	@if [ ! -e "$(IMAGEDIR)/MLO" ]; then \
		ptxd_get_path "$(BAREBOX_MLO_DIR)/MLO" \
			"$(BAREBOX_MLO_DIR)/barebox.bin.ift" && \
		install -D -m644 "$${ptxd_reply}" "$(IMAGEDIR)/MLO"; \
	else \
		rm -f $(IMAGEDIR)/MLO; \
	fi
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/barebox_mlo.clean:
	@$(call targetinfo)
	@rm -rf $(SRCDIR)/barebox_mlo*
	@$(call clean_pkg, BAREBOX_MLO)

# ----------------------------------------------------------------------------
# oldconfig / menuconfig
# ----------------------------------------------------------------------------

barebox_mlo_oldconfig barebox_mlo_menuconfig: $(STATEDIR)/barebox_mlo.extract
	@if test -e $(BAREBOX_MLO_CONFIG); then \
		cp $(BAREBOX_MLO_CONFIG) $(BAREBOX_MLO_DIR)/.config; \
		$(eval COMPARE_CONFIG := $(BAREBOX_MLO_CONFIG)) \
	else \
		echo "** choose defconfig: $(PTXCONF_BAREBOX_MLO_CONFIG) "; \
		echo ""; \
		read _choosed_defconfig; \
		echo $${_choosed_defconfig} > $(BAREBOX_MLO_DIR)/_choosed_defconfig; \
		$(call defconfig, BAREBOX_MLO, $${_choosed_defconfig}); \
		cp $(BAREBOX_MLO_DIR)/.config $(BAREBOX_MLO_DIR)/tmpconfig; \
		$(eval COMPARE_CONFIG := $(BAREBOX_MLO_DIR)/tmpconfig) \
	fi

	cd $(BAREBOX_MLO_DIR) && \
		$(BAREBOX_MLO_PATH) $(BAREBOX_MLO_CONF_ENV) $(MAKE) \
			$(BAREBOX_MLO_CONF_OPT) $(subst barebox_mlo_,,$@)

	@if cmp -s $(BAREBOX_MLO_DIR)/.config $(COMPARE_CONFIG); then \
		echo "barebox_mlo configuration unchanged"; \
	elif [ -e $(BAREBOX_MLO_DIR)/_choosed_defconfig ]; then \
		$(call savedefconfig, BAREBOX_MLO); \
		mv $(BAREBOX_MLO_DIR)/defconfig $(BAREBOX_MLO_DIR)/arch/$(PTXCONF_BAREBOX_ARCH_STRING)/configs/$$(cat $(BAREBOX_MLO_DIR)/_choosed_defconfig); \
		echo "new kernel configuration saved $$(cat $(BAREBOX_DIR)/_choosed_defconfig)"; \
		rm -f $(COMPARE_CONFIG); \
		rm -f $(BAREBOX_MLO_DIR)/_choosed_defconfig; \
	else \
		cp $(BAREBOX_MLO_DIR)/.config $(BAREBOX_MLO_CONFIG); \
	fi

# vim: syntax=make
