# -*-makefile-*-
#
# Copyright (C) 2012 by Jan Luebbe <jlu@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
IMAGE_PACKAGES-$(PTXCONF_IMAGE_BOOT_MLO) += image-boot-mlo

#
# Paths and names
#
IMAGE_BOOT_MLO		:= image-boot-mlo
IMAGE_BOOT_MLO_DIR	:= $(BUILDDIR)/$(IMAGE_BOOT_MLO)
IMAGE_BOOT_MLO_IMAGE	:= $(IMAGEDIR)/boot.vfat
IMAGE_BOOT_MLO_CONFIG	:= boot.vfat.config

# ----------------------------------------------------------------------------
# Image
# ----------------------------------------------------------------------------
IMAGE_BOOT_MLO_ENV := \
	VFAT_CONFIG='$(shell cat $(PTXDIST_PLATFORMCONFIGDIR)/config/images/vfat.config)'


$(IMAGE_BOOT_MLO_IMAGE): $(STATEDIR)/dtc.targetinstall
	@$(call targetinfo)
	@$(call image/genimage, IMAGE_BOOT_MLO)
	@$(call finish)

# vim: syntax=make
