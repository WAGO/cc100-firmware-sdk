# -*-makefile-*-
#
# Copyright (C) 2013 by <elrest>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBMSGTOOL) += libmsgtool

#
# Paths and names
#
LIBMSGTOOL_VERSION	:= 0.0.1
LIBMSGTOOL_MD5		:=
LIBMSGTOOL		:= libmsgtool
LIBMSGTOOL_URL		:= file://local_src/$(LIBMSGTOOL)
LIBMSGTOOL_DIR		:= $(BUILDDIR)/$(LIBMSGTOOL)
LIBMSGTOOL_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/libmsgtool.extract:
#	@$(call targetinfo)
#	@$(call clean, $(LIBMSGTOOL_DIR))
#	@$(call extract, LIBMSGTOOL)
#	@$(call patchin, LIBMSGTOOL)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LIBMSGTOOL_PATH	:= PATH=$(CROSS_PATH)
LIBMSGTOOL_CONF_TOOL	:= NO
LIBMSGTOOL_MAKE_ENV	:= $(CROSS_ENV)

#$(STATEDIR)/libmsgtool.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, LIBMSGTOOL)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libmsgtool.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBMSGTOOL)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/libmsgtool.install:
#	@$(call targetinfo)
#	@$(call world/install, LIBMSGTOOL)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libmsgtool.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libmsgtool)
	@$(call install_fixup, libmsgtool,PRIORITY,optional)
	@$(call install_fixup, libmsgtool,SECTION,base)
	@$(call install_fixup, libmsgtool,AUTHOR,"<elrest>")
	@$(call install_fixup, libmsgtool,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_copy, libmsgtool, 0, 0, 0644, $(LIBMSGTOOL_DIR)/libmsgtool.so, /usr/lib/libmsgtool.so.$(LIBMSGTOOL_VERSION))
	@$(call install_link, libmsgtool, libmsgtool.so.$(LIBMSGTOOL_VERSION), /usr/lib/libmsgtool.so)
	@$(call install_link, libmsgtool, libmsgtool.so.$(LIBMSGTOOL_VERSION), /usr/lib/libmsgtool.so.0)
	@$(call install_finish, libmsgtool)
	@$(call touch)


# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libmsgtool.clean:
	@$(call targetinfo)
	@-cd $(LIBMSGTOOL_DIR) && \
		$(LIBMSGTOOL_MAKE_ENV) $(LIBMSGTOOL_PATH) $(MAKE) clean
	@$(call clean_pkg, LIBMSGTOOL)

# vim: syntax=make
