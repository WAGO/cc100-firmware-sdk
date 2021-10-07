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
PACKAGES-$(PTXCONF_DIN_SPI_DRV) += din_spi_drv

#
# Paths and names and versions
#
DIN_SPI_DRV_VERSION	:= 1.0
DIN_SPI_DRV		:= din_spi_drv-$(DIN_SPI_DRV_VERSION)
DIN_SPI_DRV_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/$(DIN_SPI_DRV)
DIN_SPI_DRV_DIR		:= $(BUILDDIR)/$(DIN_SPI_DRV)
DIN_SPI_DRV_LICENSE	:= unknown

ifdef PTXCONF_DIN_SPI_DRV
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/din_spi_drv.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/din_spi_drv.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/din_spi_drv.compile:
	@$(call targetinfo)
	$(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DIN_SPI_DRV_DIR) \
		modules
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/din_spi_drv.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/din_spi_drv.targetinstall:
	@$(call targetinfo)
	$(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DIN_SPI_DRV_DIR) \
		modules_install
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/din_spi_drv.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DIN_SPI_DRV)

# vim: syntax=make
