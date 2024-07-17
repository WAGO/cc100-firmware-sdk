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
PACKAGES-$(PTXCONF_TPM2_TSS) += tpm2-tss

#
# Paths and names
#
TPM2_TSS_VERSION	:= 4.0.1
TPM2_TSS_MD5		:= fff676c669519097906bd8ce28fc4238
TPM2_TSS		:= tpm2-tss-$(TPM2_TSS_VERSION)
TPM2_TSS_SUFFIX		:= tar.gz
TPM2_TSS_URL		:= https://github.com/tpm2-software/tpm2-tss/releases/download/$(TPM2_TSS_VERSION)/$(TPM2_TSS).$(TPM2_TSS_SUFFIX)
TPM2_TSS_SOURCE		:= $(SRCDIR)/$(TPM2_TSS).$(TPM2_TSS_SUFFIX)
TPM2_TSS_DIR		:= $(BUILDDIR)/$(TPM2_TSS)
TPM2_TSS_LICENSE	:= BSD-2-Clause
TPM2_TSS_LICENSE_FILES	:= file://LICENSE;md5=500b2e742befc3da00684d8a1d5fd9da

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
TPM2_TSS_CONF_TOOL	:= autoconf
TPM2_TSS_CONF_ENV := \
	$(CROSS_ENV) \
	ac_cv_prog_groupadd=yes \
	ac_cv_prog_useradd=yes \
	ac_cv_prog_addgroup=yes \
	ac_cv_prog_adduser=yes

TPM2_TSS_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-debug=info \
	--disable-unit \
	--enable-fapi \
	--enable-tcti-device \
	--enable-tcti-mssim \
	--disable-tcti-fuzzing \
	--disable-nodl \
	--disable-integration \
	--disable-valgrind \
	--disable-valgrind-memcheck \
	--disable-valgrind-helgrind \
	--disable-valgrind-drd \
	--disable-valgrind-sgcheck \
	--disable-defaultflags \
	--disable-weakcrypto \
	--disable-doxygen-doc \
	--disable-doxygen-dot \
	--disable-doxygen-man \
	--disable-doxygen-rtf \
	--disable-doxygen-xml \
	--disable-doxygen-chm \
	--disable-doxygen-chi \
	--disable-doxygen-html \
	--disable-doxygen-ps \
	--disable-doxygen-pdf \
	--disable-code-coverage \
	--with-crypto=ossl \
	--with-udevrulesdir=/usr/lib/udev/rules.d \
	--with-fuzzing=none \
	--without-gcov

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tpm2-tss.targetinstall:
	@$(call targetinfo)

	@$(call install_init, tpm2-tss)
	@$(call install_fixup, tpm2-tss,PRIORITY,optional)
	@$(call install_fixup, tpm2-tss,SECTION,base)
	@$(call install_fixup, tpm2-tss,AUTHOR,"Marc Kleine-Budde <mkl@pengutronix.de>")
	@$(call install_fixup, tpm2-tss,DESCRIPTION,missing)

	@$(call install_lib, tpm2-tss, 0, 0, 0644, libtss2-esys)
	@$(call install_lib, tpm2-tss, 0, 0, 0644, libtss2-mu)
	@$(call install_lib, tpm2-tss, 0, 0, 0644, libtss2-sys)
	@$(call install_lib, tpm2-tss, 0, 0, 0644, libtss2-fapi)
	@$(call install_lib, tpm2-tss, 0, 0, 0644, libtss2-tcti-device)
	@$(call install_lib, tpm2-tss, 0, 0, 0644, libtss2-tcti-mssim)
	@$(call install_lib, tpm2-tss, 0, 0, 0644, libtss2-tctildr)
	@$(call install_lib, tpm2-tss, 0, 0, 0644, libtss2-rc)

	@$(call install_alternative, tpm2-tss, 0, 0, 0644, \
		/usr/lib/udev/rules.d/70-tpm-udev.rules)

	@$(call install_finish, tpm2-tss)

	@$(call touch)

# vim: syntax=make
