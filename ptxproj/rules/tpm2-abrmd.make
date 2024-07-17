# -*-makefile-*-
#
# Copyright (C) 2017 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_TPM2_ABRMD) += tpm2-abrmd

#
# Paths and names
#
TPM2_ABRMD_VERSION		:= 2.3.0
TPM2_ABRMD_MD5			:= 1326e92638d4f55cc4553260e5c24b6c
TPM2_ABRMD			:= tpm2-abrmd-$(TPM2_ABRMD_VERSION)
TPM2_ABRMD_SUFFIX		:= tar.gz
TPM2_ABRMD_URL			:= https://github.com/tpm2-software/tpm2-abrmd/releases/download/$(TPM2_ABRMD_VERSION)/$(TPM2_ABRMD).$(TPM2_ABRMD_SUFFIX)
TPM2_ABRMD_SOURCE		:= $(SRCDIR)/$(TPM2_ABRMD).$(TPM2_ABRMD_SUFFIX)
TPM2_ABRMD_DIR			:= $(BUILDDIR)/$(TPM2_ABRMD)
TPM2_ABRMD_LICENSE		:= BSD-2-Clause
TPM2_ABRMD_LICENSE_FILES	:= file://LICENSE;md5=500b2e742befc3da00684d8a1d5fd9da

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
TPM2_ABRMD_CONF_TOOL	:= autoconf
TPM2_ABRMD_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-debug=info \
	--disable-unit \
	--disable-code-coverage \
	--disable-valgrind \
	--disable-valgrind-memcheck \
	--disable-valgrind-helgrind \
	--disable-valgrind-drd \
	--disable-valgrind-sgcheck \
	--disable-dlclose \
	--disable-test-hwtpm \
	--disable-integration \
	--disable-defaultflags \
	--without-gcov \
	--with-systemdsystemunitdir=/usr/lib/systemd/system \
	--with-dbuspolicydir=/usr/share/dbus-1/system.d

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tpm2-abrmd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, tpm2-abrmd)
	@$(call install_fixup, tpm2-abrmd,PRIORITY,optional)
	@$(call install_fixup, tpm2-abrmd,SECTION,base)
	@$(call install_fixup, tpm2-abrmd,AUTHOR,"Marc Kleine-Budde <mkl@pengutronix.de>")
	@$(call install_fixup, tpm2-abrmd,DESCRIPTION,missing)

	@$(call install_lib, tpm2-abrmd, 0, 0, 0644, libtss2-tcti-tabrmd)
	@$(call install_copy, tpm2-abrmd, 0, 0, 0755, -, /usr/sbin/tpm2-abrmd)

	@$(call install_alternative, tpm2-abrmd, 0, 0, 0644, \
		/usr/lib/systemd/system/tpm2-abrmd.service)

	@$(call install_alternative, tpm2-abrmd, 0, 0, 0644, \
		/usr/share/dbus-1/system.d/tpm2-abrmd.conf)
	@$(call install_alternative, tpm2-abrmd, 0, 0, 0644, \
		/usr/share/dbus-1/system-services/com.intel.tss2.Tabrmd.service)

	@$(call install_finish, tpm2-abrmd)

	@$(call touch)

# vim: syntax=make
