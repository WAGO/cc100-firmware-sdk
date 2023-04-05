# -*-makefile-*-
#
# Copyright (C) 2021 by elrest GmbH <gliese@elrest.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_GPTFDISK) += host-gptfdisk

#
# Paths and names
#
HOST_GPTFDISK_DIR	= $(HOST_BUILDDIR)/$(GPTFDISK)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/host-gptfdisk.prepare:
	@$(call targetinfo)
	cd $(HOST_GPTFDISK_DIR) && make clean
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-gptfdisk.install:
	@$(call targetinfo)
	@cp $(HOST_GPTFDISK_DIR)/sgdisk $(PTXDIST_SYSROOT_HOST)/bin
	@cp $(HOST_GPTFDISK_DIR)/cgdisk $(PTXDIST_SYSROOT_HOST)/bin
	@cp $(HOST_GPTFDISK_DIR)/gdisk $(PTXDIST_SYSROOT_HOST)/bin
	@$(call touch)

# vim: syntax=make
