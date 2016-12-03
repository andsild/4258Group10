# -*-makefile-*-
#
# Copyright (C) 2014 by 
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GAME) += game

#
# Paths and names
#
GAME_VERSION	:= 1.0
GAME_MD5		:=
GAME		:= game
GAME_URL		:= file://local_src/$(GAME)-$(GAME_VERSION)
GAME_DIR		:= $(BUILDDIR)/$(GAME)
GAME_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#GAME_PATH	:= PATH=$(CROSS_PATH)
GAME_CONF_TOOL	:= NO
GAME_MAKE_ENV	:= $(CROSS_ENV)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/game.targetinstall:
	@$(call targetinfo)

	@$(call install_init, game)
	@$(call install_fixup, game,PRIORITY,optional)
	@$(call install_fixup, game,SECTION,base)
	@$(call install_fixup, game,AUTHOR,"")
	@$(call install_fixup, game,DESCRIPTION,missing)

	@$(call install_copy, game, 0, 0, 0755, $(GAME_DIR)/game, /usr/bin/$(GAME))
	
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/umbrella.bmp, /usr/local/share/game/images/umbrella.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/presstart.bmp, /usr/local/share/game/images/resstart.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/umbrella.bmp, /usr/local/share/game/images/umbrella.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/arrow.bmp, /usr/local/share/game/images/arrow.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/GreenArrow.bmp, /usr/local/share/game/images/GreenArrow.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/PurpleArrow.bmp, /usr/local/share/game/images/PurpleArrow.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/YellowArrow.bmp, /usr/local/share/game/images/YellowArrow.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/RedArrow.bmp, /usr/local/share/game/images/RedArrow.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/GameOverlay.bmp, /usr/local/share/game/images/GameOverlay.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/GodEmperorTrump.bmp, /usr/local/share/game/images/GodEmperorTrump.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/0a.bmp, /usr/local/share/game/images/0a.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/0.bmp, /usr/local/share/game/images/0.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/1.bmp, /usr/local/share/game/images/1.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/2.bmp, /usr/local/share/game/images/2.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/3.bmp, /usr/local/share/game/images/3.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/4.bmp, /usr/local/share/game/images/4.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/5.bmp, /usr/local/share/game/images/5.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/6.bmp, /usr/local/share/game/images/6.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/7.bmp, /usr/local/share/game/images/7.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/8.bmp, /usr/local/share/game/images/8.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/9.bmp, /usr/local/share/game/images/9.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/Great.bmp, /usr/local/share/game/images/Great.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/Miss.bmp, /usr/local/share/game/images/Miss.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/Perfect.bmp, /usr/local/share/game/images/Perfect.bmp)
	@$(call install_copy, game, 0, 0, 0777, $(GAME_DIR)/images/Boo.bmp, /usr/local/share/game/images/Boo.bmp)

	@$(call install_finish, game)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/game.clean:
	@$(call targetinfo)
	@-cd $(GAME_DIR) && \
		$(GAME_ENV) $(GAME_PATH) $(MAKE) clean
	@$(call clean_pkg, GAME)

# vim: syntax=make
