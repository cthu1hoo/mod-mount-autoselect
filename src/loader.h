/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#ifndef _MOUNT_AUTOSELECT_MODULE_LOADER_H_
#define _MOUNT_AUTOSELECT_MODULE_LOADER_H_

// From SC
void AddMountAutoselectScripts();

// Add all
void AddAllScripts()
{
    AddMountAutoselectScripts();
}

#endif // _MOUNT_AUTOSELECT_MODULE_LOADER_H_
