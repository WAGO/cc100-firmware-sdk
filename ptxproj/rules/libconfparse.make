# -*-makefile-*-
#
# Copyright (C) 2012 by <RG>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBCONFPARSE) += libconfparse

#
# Paths and names
#
LIBCONFPARSE_VERSION	:= 1
LIBCONFPARSE		:= libconfparse
LIBCONFPARSE_URL		:= file://local_src/$(LIBCONFPARSE)
LIBCONFPARSE_DIR		:= $(BUILDDIR)/$(LIBCONFPARSE)
LIBCONFPARSE_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/libconfparse.extract:
#	@$(call targetinfo)
#	@$(call clean, $(LIBCONFPARSE_DIR))
#	@$(call extract, LIBCONFPARSE)
#	@$(call patchin, LIBCONFPARSE)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LIBCONFPARSE_PATH	:= PATH=$(CROSS_PATH)
LIBCONFPARSE_CONF_TOOL	:= NO
LIBCONFPARSE_MAKE_ENV	:= $(CROSS_ENV)

#$(STATEDIR)/libconfparse.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, LIBCONFPARSE)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libconfparse.compile:
#	@$(call targetinfo)
#	cd $(LIBCONFPARSE_DIR) && \
#	LIBS="-L$(PREFIX)/lib -R$(PREFIX)/lib" make CFLAGS=-I/$(BUILDDIR)/../sysroot-target/usr/include
#	@$(call world/compile, LIBCONFPARSE)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libconfparse.install:
	@$(call targetinfo)
	@$(call world/install, LIBCONFPARSE)
	cp $(LIBCONFPARSE_DIR)/confparse.h $(BUILDDIR)/../sysroot-target/usr/include && \
	cp $(LIBCONFPARSE_DIR)/libconfparse.so $(BUILDDIR)/../sysroot-target/usr/lib/libconfparse.so  
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libconfparse.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libconfparse)
	@$(call install_fixup, libconfparse,PRIORITY,optional)
	@$(call install_fixup, libconfparse,SECTION,base)
	@$(call install_fixup, libconfparse,AUTHOR,"<RG>")
	@$(call install_fixup, libconfparse,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_copy, libconfparse, 0, 0, 0644, $(LIBCONFPARSE_DIR)/libconfparse.so, /usr/lib/libconfparse.so.0.0.0)
	@$(call install_link, libconfparse, libconfparse.so.0.0.0, /usr/lib/libconfparse.so)
	@$(call install_link, libconfparse, libconfparse.so.0.0.0, /usr/lib/libconfparse.so.0)
	@$(call install_finish, libconfparse)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libconfparse.clean:
	@$(call targetinfo)
	@-cd $(LIBCONFPARSE_DIR) && \
		$(LIBCONFPARSE_MAKE_ENV) $(LIBCONFPARSE_PATH) $(MAKE) clean
	@$(call clean_pkg, LIBCONFPARSE)

# vim: syntax=make
