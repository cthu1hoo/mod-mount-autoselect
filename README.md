# Automatically limit mount speed based on player riding skill and location

This module mimics mounts that "change their appearance based on rider skill and location" without creating multiple (five) spells for every mount and hardcoding those in Azeroth Core `spell_gen_mount()` (how Invincible is implemented).

Approach used for this module has some minor issues:

- Speed buff text is not displayed in the UI;
- Transitioning into `AREA_FLAG_NO_FLY_ZONE` (i.e. Dalaran city) is not handled correctly: the debuff doesn't unmount you for some reason, although it does stop you from flying;

Still, it's good enough for me, and could be useful as a proof of concept.

Alternatively, see `spell-id-ranges` branch for a more traditional approach.

## Setup

Checkout the module into `modules` server directory, and recompile the server.

You will need to add 5 invisible service spells which apply actual speed increase and flying auras, those are hardcoded at IDs `240001` (60%) - `240005` (310%).

Service spells are provided in included `Spells.csv`, import those into `Spells.dbc`.

Then, add actual mount summoning spell.

For the spell to work properly, it needs to have the following effects:

1. `APPLY_AURA` -> `SPELL_AURA_MOUNTED` -> `NPC ID` is passed via Misc Value A. (this activates the mount)
2. `APPLY_AURA` -> `SPELL_AURA_DUMMY`
3. `SCRIPT_EFFECT`

Module expects effects 2 and 3 (which normally do nothing) while effect 1 is needed to actually call in the mount. You can use base Invincible spell (`72286`) as an example.

Then, assign spell script to your mount spell:

```sql
insert into spell_script_names (spell_id, ScriptName) values (251003, 'spell_mount_autoselect');
```

Where `251003` is your custom mount spell. Example is, again, provided in `Spell.csv`.

If everything worked correctly, you should be able to summon the mount everywhere, ground-only where appropriate, or flying if allowed.
