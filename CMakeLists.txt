#
# Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
#

# Add scripts in project
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/MountAutoselect.cpp")

# Add scripts to script loader
AC_ADD_SCRIPT_LOADER("MountAutoselect" "${CMAKE_CURRENT_LIST_DIR}/src/loader.h")

# Add config file
AC_ADD_CONFIG_FILE("${CMAKE_CURRENT_LIST_DIR}/conf/mount_autoselect.conf.dist")
