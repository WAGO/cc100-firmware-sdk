# -*-makefile-*-
#
# Copyright (C) 2020 by <elrest>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DOUT_DRV) += dout_drv

#
# Paths and names and versions
#
DOUT_DRV_VERSION	:= 1.0
DOUT_DRV		:= dout_drv-$(DOUT_DRV_VERSION)
DOUT_DRV_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/$(DOUT_DRV)
DOUT_DRV_DIR		:= $(BUILDDIR)/$(DOUT_DRV)
DOUT_DRV_LICENSE	:= unknown

ifdef PTXCONF_DOUT_DRV
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/dout_drv.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/dout_drv.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/dout_drv.compile:
	@$(call targetinfo)
	$(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DOUT_DRV_DIR) \
		modules
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dout_drv.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dout_drv.targetinstall:
	@$(call targetinfo)
	$(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DOUT_DRV_DIR) \
		modules_install
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/dout_drv.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DOUT_DRV)

# vim: syntax=make
