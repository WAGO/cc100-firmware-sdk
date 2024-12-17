# -*-makefile-*-
#
# Copyright (C) 2021 by <elrest>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GPTFDISK) += gptfdisk

#
# Paths and names
#
GPTFDISK_VERSION	:= 1.0.4
GPTFDISK_MD5		:= 5ecc3c44913bb6b53d3708d1ac7ac295
GPTFDISK			:= gptfdisk-$(GPTFDISK_VERSION)
GPTFDISK_SUFFIX		:= tar.gz
GPTFDISK_URL		:= https://git.code.sf.net/p/gptfdisk/code gptfdisk-code/$(GPTDISK)-$(GPTDISK_VERSION).$(GPTDISK_SUFFIX)
GPTFDISK_SOURCE		:= $(SRCDIR)/$(GPTFDISK).$(GPTFDISK_SUFFIX)
GPTFDISK_DIR		:= $(BUILDDIR)/$(GPTFDISK)
GPTFDISK_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/gptfdisk.extract:
	@$(call targetinfo)
	@$(call clean, $(GPTFDISK_DIR))
	@$(call extract, GPTFDISK)
	@$(call patchin, GPTFDISK)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

GPTFDISK_CONF_ENV	:= $(CROSS_ENV)

$(STATEDIR)/gptfdisk.prepare:
	@$(call targetinfo)
	cd $(GPTFDISK_DIR) && $(MAKE) clean
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/gptfdisk.compile:
	@$(call targetinfo)
	cd $(GPTFDISK_DIR) && \
	CROSS_COMPILE=$(COMPILER_PREFIX) CXX=$(COMPILER_PREFIX)g++ \
	CFLAGS="-I$(PTXDIST_SYSROOT_TARGET)/usr/include" \
	LDFLAGS="-L$(PTXCONF_SYSROOT_TARGET)/usr/lib" make
	@$(call touch)


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/gptfdisk.install:
	@$(call targetinfo)
#	@$(call world/install, GPTFDISK)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/gptfdisk.targetinstall:
	@$(call targetinfo)

	@$(call install_init, gptfdisk)
	@$(call install_fixup, gptfdisk, PRIORITY, optional)
	@$(call install_fixup, gptfdisk, SECTION, base)
	@$(call install_fixup, gptfdisk, AUTHOR, "<elrest>")
	@$(call install_fixup, gptfdisk, DESCRIPTION, missing)

	@$(call install_copy, gptfdisk, 0, 0, 0755, $(GPTFDISK_DIR)/gdisk, /usr/bin/gdisk)
	@$(call install_copy, gptfdisk, 0, 0, 0755, $(GPTFDISK_DIR)/sgdisk, /usr/bin/sgdisk)

	@$(call install_finish, gptfdisk)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/gptfdisk.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, GPTFDISK)

# vim: syntax=make
