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
PACKAGES-$(PTXCONF_TPM2_TOOLS) += tpm2-tools

#
# Paths and names
#
TPM2_TOOLS_VERSION		:= 5.6
TPM2_TOOLS_MD5			:= c8362a622634646d3a5dbe012b9307d8
TPM2_TOOLS			:= tpm2-tools-$(TPM2_TOOLS_VERSION)
TPM2_TOOLS_SUFFIX		:= tar.gz
TPM2_TOOLS_URL			:= https://github.com/tpm2-software/tpm2-tools/releases/download/$(TPM2_TOOLS_VERSION)/$(TPM2_TOOLS).$(TPM2_TOOLS_SUFFIX)
TPM2_TOOLS_SOURCE		:= $(SRCDIR)/$(TPM2_TOOLS).$(TPM2_TOOLS_SUFFIX)
TPM2_TOOLS_DIR			:= $(BUILDDIR)/$(TPM2_TOOLS)
TPM2_TOOLS_LICENSE		:= BSD-3-Clause
TPM2_TOOLS_LICENSE_FILES	:= file://docs/LICENSE;md5=a846608d090aa64494c45fc147cc12e3

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
TPM2_TOOLS_CONF_TOOL	:= autoconf
TPM2_TOOLS_CONF_ENV := \
	$(CROSS_ENV) \
	$(call ptx/ifdef, PTXCONF_TPM2_TOOLS_USE_EFIVAR,,ac_cv_header_efivar_efivar_h=no)

TPM2_TOOLS_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-debug=info \
	--disable-code-coverage \
	--disable-unit \
	--enable-hardening \
	--enable-fapi \
	--without-gcov \
	--with-bashcompdir=/ignore

TPM2_TOOLS_CFLAGS	:= \
	-fPIC

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

TPM2_TOOLS_TPM2_PROGS := \
	tpm2_activatecredential \
	tpm2_certify \
	tpm2_certifycreation \
	tpm2_certifyX509certutil \
	tpm2_changeauth \
	tpm2_changeeps \
	tpm2_changepps \
	tpm2_checkquote \
	tpm2_clear \
	tpm2_clearcontrol \
	tpm2_clockrateadjust \
	tpm2_commit \
	tpm2_create \
	tpm2_createak \
	tpm2_createek \
	tpm2_createpolicy \
	tpm2_createprimary \
	tpm2_dictionarylockout \
	tpm2_duplicate \
	tpm2_ecdhkeygen \
	tpm2_ecdhzgen \
	tpm2_ecephemeral \
	tpm2_encodeobject \
	tpm2_encryptdecrypt \
	tpm2_eventlog \
	tpm2_evictcontrol \
	tpm2_flushcontext \
	tpm2_getcap \
	tpm2_getcommandauditdigest \
	tpm2_geteccparameters \
	tpm2_getekcertificate \
	tpm2_getpolicydigest \
	tpm2_getrandom \
	tpm2_getsessionauditdigest \
	tpm2_gettestresult \
	tpm2_gettime \
	tpm2_hash \
	tpm2_hierarchycontrol \
	tpm2_hmac \
	tpm2_import \
	tpm2_incrementalselftest \
	tpm2_load \
	tpm2_loadexternal \
	tpm2_makecredential \
	tpm2_nvcertify \
	tpm2_nvdefine \
	tpm2_nvextend \
	tpm2_nvincrement \
	tpm2_nvread \
	tpm2_nvreadlock \
	tpm2_nvreadpublic \
	tpm2_nvsetbits \
	tpm2_nvundefine \
	tpm2_nvwrite \
	tpm2_nvwritelock \
	tpm2_pcrallocate \
	tpm2_pcrevent \
	tpm2_pcrextend \
	tpm2_pcrread \
	tpm2_pcrreset \
	tpm2_policyauthorize \
	tpm2_policyauthorizenv \
	tpm2_policyauthvalue \
	tpm2_policycommandcode \
	tpm2_policycountertimer \
	tpm2_policycphash \
	tpm2_policyduplicationselect \
	tpm2_policylocality \
	tpm2_policynamehash \
	tpm2_policynv \
	tpm2_policynvwritten \
	tpm2_policyor \
	tpm2_policypassword \
	tpm2_policypcr \
	tpm2_policyrestart \
	tpm2_policysecret \
	tpm2_policysigned \
	tpm2_policytemplate \
	tpm2_policyticket \
	tpm2_print \
	tpm2_quote \
	tpm2_rc_decode \
	tpm2_readclock \
	tpm2_readpublic \
	tpm2_rsadecrypt \
	tpm2_rsaencrypt \
	tpm2_selftest \
	tpm2_send \
	tpm2_sessionconfig \
	tpm2_setclock \
	tpm2_setcommandauditstatus \
	tpm2_setprimarypolicy \
	tpm2_shutdown \
	tpm2_sign \
	tpm2_startauthsession \
	tpm2_startup \
	tpm2_stirrandom \
	tpm2_testparms \
	tpm2_unseal \
	tpm2_verifysignature \
	tpm2_zgen2phase

TPM2_TOOLS_TSS2_PROGS := \
	tss2_authorizepolicy \
	tss2_changeauth \
	tss2_createkey \
	tss2_createnv \
	tss2_createseal \
	tss2_decrypt \
	tss2_delete \
	tss2_encrypt \
	tss2_exportkey \
	tss2_exportpolicy \
	tss2_getappdata \
	tss2_getcertificate \
	tss2_getdescription \
	tss2_getinfo \
	tss2_getplatformcertificates \
	tss2_getrandom \
	tss2_gettpm2object \
	tss2_gettpmblobs \
	tss2_import \
	tss2_list \
	tss2_nvextend \
	tss2_nvincrement \
	tss2_nvread \
	tss2_nvsetbits \
	tss2_nvwrite \
	tss2_pcrextend \
	tss2_pcrread \
	tss2_provision \
	tss2_quote \
	tss2_setappdata \
	tss2_setcertificate \
	tss2_setdescription \
	tss2_sign \
	tss2_unseal \
	tss2_verifyquote \
	tss2_verifysignature \
	tss2_writeauthorizenv

$(STATEDIR)/tpm2-tools.targetinstall:
	@$(call targetinfo)

	@$(call install_init, tpm2-tools)
	@$(call install_fixup, tpm2-tools,PRIORITY,optional)
	@$(call install_fixup, tpm2-tools,SECTION,base)
	@$(call install_fixup, tpm2-tools,AUTHOR,"Marc Kleine-Budde <mkl@pengutronix.de>")
	@$(call install_fixup, tpm2-tools,DESCRIPTION,missing)

	@$(call install_copy, tpm2-tools, 0, 0, 0755, -, /usr/bin/tpm2)
	@$(call install_copy, tpm2-tools, 0, 0, 0755, -, /usr/bin/tss2)

	@$(foreach prog, $(TPM2_TOOLS_TPM2_PROGS), \
		@$(call install_link, tpm2-tools, tpm2, /usr/bin/$(prog))$(ptx/nl))

	@$(foreach prog, $(TPM2_TOOLS_TSS2_PROGS), \
		@$(call install_link, tpm2-tools, tss2, /usr/bin/$(prog))$(ptx/nl))

	@$(call install_finish, tpm2-tools)

	@$(call touch)

# vim: syntax=make
