#
# Copyright (c) 2015 Pavlo Lavrenenko
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required(VERSION 2.6.0)
project (SVDLL)

set (SVDLL_LIBRARY server)

add_definitions(-DCLIENT_WEAPONS)

if(NOT MSVC)
	add_compile_options(-fno-exceptions) # GCC/Clang flag
	add_compile_options(-Wno-invalid-offsetof) # GCC/Clang flag
	add_compile_options(-fvisibility=hidden) # GCC/Clang flag
	add_definitions(-D_LINUX) # It seems enough for all non-Win32 systems
	add_definitions(-Dstricmp=strcasecmp -Dstrnicmp=strncasecmp -D_snprintf=snprintf -D_vsnprintf=vsnprintf )
else()
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
endif()

set (SVDLL_SOURCES
	agrunt.cpp
	airtank.cpp
	aflock.cpp
	animating.cpp
	animation.cpp
	apache.cpp
	barnacle.cpp
	barney.cpp
	bigmomma.cpp
	bloater.cpp
	bmodels.cpp
	bullsquid.cpp
	buttons.cpp
	cbase.cpp
	client.cpp
	combat.cpp
	controller.cpp
	defaultai.cpp
	doors.cpp
	effects.cpp
	explode.cpp
	flyingmonster.cpp
	func_break.cpp
	func_tank.cpp
	game.cpp
	gamerules.cpp
	gargantua.cpp
	genericmonster.cpp
	ggrenade.cpp
	globals.cpp
	gman.cpp
	h_ai.cpp
	h_battery.cpp
	h_cine.cpp
	h_cycler.cpp
	h_export.cpp
	hassassin.cpp
	headcrab.cpp
	healthkit.cpp
	hgrunt.cpp
	hornet.cpp
	houndeye.cpp
	ichthyosaur.cpp
	islave.cpp
	items.cpp
	leech.cpp
	lights.cpp
	maprules.cpp
	monstermaker.cpp
	monsters.cpp
	monsterstate.cpp
	mortar.cpp
	multiplay_gamerules.cpp
	nihilanth.cpp
	nodes.cpp
	observer.cpp
	osprey.cpp
	pathcorner.cpp
	plane.cpp
	plats.cpp
	player.cpp
	playermonster.cpp
	rat.cpp
	roach.cpp
	schedule.cpp
	scientist.cpp
	scripted.cpp
	singleplay_gamerules.cpp
	skill.cpp
	sound.cpp
	soundent.cpp
	spectator.cpp
	squadmonster.cpp
	subs.cpp
	talkmonster.cpp
	teamplay_gamerules.cpp
	tempmonster.cpp
	tentacle.cpp
	triggers.cpp
	turret.cpp
	util.cpp
	weapons.cpp
	world.cpp
	xen.cpp
	zombie.cpp
	bots/bot_ai.cpp
	bots/bot_fightstyle.cpp
	bots/bot_memory.cpp
	bots/bot_misc.cpp
	bots/bot_move.cpp
	bots/bot_nav.cpp
	bots/bot_stats.cpp
	bots/bot_cvars.cpp
	bots/botgamerulesinterface.cpp
	bots/profiles/botprofiletable.cpp
	bots/profiles/botprofileparser.cpp
	../pm_shared/pm_debug.c
	../pm_shared/pm_math.c
	../pm_shared/pm_shared.c
	../game_shared/weapons/crossbow.cpp
	../game_shared/weapons/crowbar.cpp
	../game_shared/weapons/egon.cpp
	../game_shared/weapons/gauss.cpp
	../game_shared/weapons/glock.cpp
	../game_shared/weapons/handgrenade.cpp
	../game_shared/weapons/hornetgun.cpp
	../game_shared/weapons/mp5.cpp
	../game_shared/weapons/python.cpp
	../game_shared/weapons/rpg.cpp
	../game_shared/weapons/satchel.cpp
	../game_shared/weapons/shotgun.cpp
	../game_shared/weapons/squeakgrenade.cpp
	../game_shared/weapons/tripmine.cpp
	../game_shared/weaponattributes/weaponatts_collection.cpp
	../game_shared/weaponattributes/weaponatts_skillrecord.cpp
	../game_shared/weapons/weaponregistry.cpp
	../game_shared/weapons/genericweapon.cpp
	../game_shared/weapons/generichitscanweapon.cpp
	../game_shared/weapons/genericprojectileweapon.cpp
	../game_shared/weapons/genericmeleeweapon.cpp
	../game_shared/weapons/ammodefs.cpp
	../game_shared/weapons/weaponslots.cpp
	../game_shared/studio_utils_shared.cpp
	../game_shared/weapons/weapon_p99.cpp
	../game_shared/weapons/weapon_frinesi.cpp
	../game_shared/weapons/weapon_grenadelauncher.cpp
	../game_shared/weapons/weapon_raptor.cpp
	../game_shared/weapons/weapon_fists.cpp
	../game_shared/rapidjson_helpers/rapidjson_helpers.cpp
	../game_shared/gameplay/radialdamage.cpp
	../miniutl/bitstring.cpp
	../miniutl/generichash.cpp
	../miniutl/ipv6text.c
	../miniutl/strtools.cpp
	../miniutl/utlbuffer.cpp
	../miniutl/utlmemory.cpp
	../miniutl/utlstring.cpp
)

include_directories (
	.
	../common
	../engine
	../pm_shared
	../game_shared
	../public
	../game_shared/weapons
	../game_shared/weaponattributes
	../game_shared/util
	../game_shared/gameplay
	../miniutl
	bots
	bots/profiles
	bots/itemattributes)

if(USE_VOICEMGR)
	set(SVDLL_SOURCES
		${SVDLL_SOURCES}
		../game_shared/voice_gamemgr.cpp)
else()
	add_definitions(-DNO_VOICEGAMEMGR)
endif()

if(MSVC)
	set(SVDLL_SOURCES
		${SVDLL_SOURCES}
		hl.def)
endif()

add_library (${SVDLL_LIBRARY} SHARED ${SVDLL_SOURCES})

set_target_properties (${SVDLL_LIBRARY} PROPERTIES
	POSITION_INDEPENDENT_CODE 1)

if(APPLE OR WIN32 OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(SVDLL_NAME "${SERVER_LIBRARY_NAME}")
	if(64BIT)
		set(SVDLL_NAME "${SERVER_LIBRARY_NAME}64")
	endif()

	set_target_properties(${SVDLL_LIBRARY} PROPERTIES
		OUTPUT_NAME ${SVDLL_NAME}
		PREFIX "")
endif()

install( TARGETS ${SVDLL_LIBRARY}
	DESTINATION "${GAMEDIR}/${SERVER_INSTALL_DIR}/"
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
	    GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE)
