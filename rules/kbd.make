# -*-makefile-*-
#
# Copyright (C) 2010 by Luotao Fu <l.fu@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_KBD) += kbd

#
# Paths and names
#
KBD_VERSION	:= 2.2.0
KBD_MD5		:= 5b07c5cf73f2a2bdff5a489368969f70
KBD		:= kbd-$(KBD_VERSION)
KBD_SUFFIX	:= tar.gz
KBD_URL		:= $(call ptx/mirror, KERNEL, utils/kbd/$(KBD).$(KBD_SUFFIX))
KBD_SOURCE	:= $(SRCDIR)/$(KBD).$(KBD_SUFFIX)
KBD_DIR		:= $(BUILDDIR)/$(KBD)
KBD_LICENSE	:= GPL-2.0-or-later

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
KBD_CONF_OPT := \
	$(CROSS_AUTOCONF_USR) \
	--disable-code-coverage \
	--disable-nls \
	--disable-rpath \
	--disable-optional-progs \
	--disable-libkeymap \
	--disable-vlock \
	--disable-tests \
	--without-libiconv-prefix \
	--without-libintl-prefix


KBD_TOOLS-y					:=
KBD_TOOLS-$(PTXCONF_KBD_CHVT)			+= chvt
KBD_TOOLS-$(PTXCONF_KBD_DEALLOCVT)		+= deallocvt
KBD_TOOLS-$(PTXCONF_KBD_DUMPKEYS)		+= dumpkeys
KBD_TOOLS-$(PTXCONF_KBD_FGCONSOLE)		+= fgconsole
KBD_TOOLS-$(PTXCONF_KBD_GETKEYCODES)		+= getkeycodes
KBD_TOOLS-$(PTXCONF_KBD_KBDINFO)		+= kbdinfo
KBD_TOOLS-$(PTXCONF_KBD_KBD_MODE)		+= kbd_mode
KBD_TOOLS-$(PTXCONF_KBD_KBDRATE)		+= kbdrate
KBD_TOOLS-$(PTXCONF_KBD_LOADKEYS)		+= loadkeys
KBD_TOOLS-$(PTXCONF_KBD_LOADUNIMAP)		+= loadunimap
KBD_TOOLS-$(PTXCONF_KBD_MAPSCRN)		+= mapscrn
KBD_TOOLS-$(PTXCONF_KBD_OPENVT)			+= openvt
KBD_TOOLS-$(PTXCONF_KBD_PSFXTABLE)		+= psfxtable
KBD_TOOLS-$(PTXCONF_KBD_SETFONT)		+= setfont
KBD_TOOLS-$(PTXCONF_KBD_SETKEYCODES)		+= setkeycodes
KBD_TOOLS-$(PTXCONF_KBD_SETLEDS)		+= setleds
KBD_TOOLS-$(PTXCONF_KBD_SETMETAMODE)		+= setmetamode
KBD_TOOLS-$(PTXCONF_KBD_SETVTRGB)		+= setvtrgb
KBD_TOOLS-$(PTXCONF_KBD_SHOWCONSOLEFONT)	+= showconsolefont
KBD_TOOLS-$(PTXCONF_KBD_SHOWKEY)		+= showkey
KBD_TOOLS-$(PTXCONF_KBD_UNICODE_START)		+= unicode_start
KBD_TOOLS-$(PTXCONF_KBD_UNICODE_STOP)		+= unicode_stop

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/kbd.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  kbd)
	@$(call install_fixup, kbd,PRIORITY,optional)
	@$(call install_fixup, kbd,SECTION,base)
	@$(call install_fixup, kbd,AUTHOR,"Luotao Fu <l.fu@pengutronix.de>")
	@$(call install_fixup, kbd,DESCRIPTION,missing)

	@$(foreach tool,$(KBD_TOOLS-y), \
		$(call install_copy, kbd, 0, 0, 0755, -, \
			/usr/bin/$(tool));)

ifdef PTXCONF_KBD_PSFXTABLE
	@$(call install_link, kbd, psfxtable, /usr/bin/psfaddtable)
	@$(call install_link, kbd, psfxtable, /usr/bin/psfgettable)
	@$(call install_link, kbd, psfxtable, /usr/bin/psfstriptable)
endif

ifdef PTXCONF_KBD_CONSOLEFONTS
	@cd $(KBD_PKGDIR)/usr/share/consolefonts && \
	find . -type f | while read file; do \
		$(call install_copy, kbd, 0, 0, 0644, -, \
			/usr/share/consolefonts/$$file); \
	done
endif

ifdef PTXCONF_KBD_CONSOLETRANS
	@cd $(KBD_PKGDIR)/usr/share/consoletrans && \
	find . -type f | while read file; do \
		$(call install_copy, kbd, 0, 0, 0644, -, \
			/usr/share/consoletrans/$$file); \
	done
endif

ifdef PTXCONF_KBD_KEYMAPS
	@cd $(KBD_PKGDIR)/usr/share/keymaps && \
	find . -type f | while read file; do \
		$(call install_copy, kbd, 0, 0, 0644, -, \
			/usr/share/keymaps/$$file); \
	done

	@$(call install_link, kbd, mac, /usr/share/keymaps/ppc)
endif

ifdef PTXCONF_KBD_UNIMAPS
	@cd $(KBD_PKGDIR)/usr/share/unimaps && \
	find . -type f | while read file; do \
		$(call install_copy, kbd, 0, 0, 0644, -, \
			/usr/share/unimaps/$$file); \
	done
endif

	@$(call install_finish, kbd)

	@$(call touch)

# vim: syntax=make
