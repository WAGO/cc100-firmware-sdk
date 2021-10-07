# -*-makefile-*-
#
# Copyright (C) 2014 by <elrest>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GETCONFVALUE) += getconfvalue

#
# Paths and names
#
GETCONFVALUE_VERSION	:= 1
GETCONFVALUE_MD5		:=
GETCONFVALUE		:= getconfvalue
GETCONFVALUE_URL		:= file://local_src/$(GETCONFVALUE)
GETCONFVALUE_DIR		:= $(BUILDDIR)/$(GETCONFVALUE)
GETCONFVALUE_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/getconfvalue.extract:
#	@$(call targetinfo)
#	@$(call clean, $(GETCONFVALUE_DIR))
#	@$(call extract, GETCONFVALUE)
#	@$(call patchin, GETCONFVALUE)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#GETCONFVALUE_PATH	:= PATH=$(CROSS_PATH)
GETCONFVALUE_CONF_TOOL	:= NO
GETCONFVALUE_MAKE_ENV	:= $(CROSS_ENV)

#$(STATEDIR)/getconfvalue.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, GETCONFVALUE)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/getconfvalue.compile:
#	@$(call targetinfo)
#	@$(call world/compile, GETCONFVALUE)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/getconfvalue.install:
#	@$(call targetinfo)
#	@$(call world/install, GETCONFVALUE)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/getconfvalue.targetinstall:
	@$(call targetinfo)

	@$(call install_init, getconfvalue)
	@$(call install_fixup, getconfvalue,PRIORITY,optional)
	@$(call install_fixup, getconfvalue,SECTION,base)
	@$(call install_fixup, getconfvalue,AUTHOR,"<elrest>")
	@$(call install_fixup, getconfvalue,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_copy, getconfvalue, 0, 0, 0755, $(GETCONFVALUE_DIR)/getconfvalue, /usr/bin/getconfvalue)

	@$(call install_finish, getconfvalue)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/getconfvalue.clean:
	@$(call targetinfo)
	@-cd $(GETCONFVALUE_DIR) && \
		$(GETCONFVALUE_MAKE_ENV) $(GETCONFVALUE_PATH) $(MAKE) clean
	@$(call clean_pkg, GETCONFVALUE)

# vim: syntax=make
