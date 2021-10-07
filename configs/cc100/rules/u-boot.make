# -*-makefile-*-
#
# Copyright (C) 2007 by Sascha Hauer
#               2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_U_BOOT) += u-boot

#
# Paths and names
#
U_BOOT_VERSION	:= $(call remove_quotes,$(PTXCONF_U_BOOT_VERSION))
U_BOOT_MD5	:= $(call remove_quotes,$(PTXCONF_U_BOOT_MD5))
U_BOOT		:= u-boot-$(U_BOOT_VERSION)
U_BOOT_SUFFIX	:= tar.bz2
U_BOOT_URL	:= ftp://ftp.denx.de/pub/u-boot/$(U_BOOT).$(U_BOOT_SUFFIX)
U_BOOT_SOURCE	:= $(SRCDIR)/$(U_BOOT).$(U_BOOT_SUFFIX)
U_BOOT_DIR	:= $(BUILDDIR)/$(U_BOOT)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

U_BOOT_PATH	:= PATH=$(CROSS_PATH)
U_BOOT_MAKE_OPT	:= CROSS_COMPILE=$(BOOTLOADER_CROSS_COMPILE) HOSTCC=$(HOSTCC)
U_BOOT_MAKE_PAR	:= NO
U_BOOT_TAGS_OPT	:= ctags cscope etags

$(STATEDIR)/u-boot.prepare:
	@$(call targetinfo)
	cd $(U_BOOT_DIR) && \
		$(U_BOOT_PATH) \
		$(MAKE) $(U_BOOT_MAKE_OPT) $(PTXCONF_U_BOOT_CONFIG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/u-boot.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/u-boot.targetinstall:
	@$(call targetinfo)
	@install -D -m644 $(U_BOOT_DIR)/u-boot.bin $(IMAGEDIR)/u-boot.bin
ifdef PTXCONF_U_BOOT_INSTALL_SREC
	@install -D -m644 $(U_BOOT_DIR)/u-boot.srec $(IMAGEDIR)/u-boot.srec
endif
ifdef PTXCONF_U_BOOT_INSTALL_ELF
	@install -D -m644 $(U_BOOT_DIR)/u-boot $(IMAGEDIR)/u-boot.elf
endif
ifdef PTXCONF_U_BOOT_INSTALL_MLO
	@install -D -m644 $(U_BOOT_DIR)/MLO $(IMAGEDIR)/MLO
endif
ifdef PTXCONF_U_BOOT_INSTALL_U_BOOT_IMG
	@install -D -m644 $(U_BOOT_DIR)/u-boot.img $(IMAGEDIR)/u-boot.img
	@install -D -m644 $(U_BOOT_DIR)/u-boot-spl.stm32 $(IMAGEDIR)/u-boot-spl.stm32
endif
ifdef PTXCONF_U_BOOT_INSTALL_U_BOOT_IMX
	@install -D -m644 $(U_BOOT_DIR)/u-boot.imx $(IMAGEDIR)/u-boot.imx
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/u-boot.clean:
	@$(call targetinfo)
	@$(call clean_pkg, U_BOOT)
	@rm -f $(IMAGEDIR)/u-boot.bin $(IMAGEDIR)/u-boot.srec $(IMAGEDIR)/u-boot.elf
	@rm -f $(IMAGEDIR)/u-boot.imx

# vim: syntax=make
