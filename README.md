# Automatically choose mounts based on player riding skill and location

This module mimics mounts that "change their appearance based on rider skill and location" without hardcoding every mount in Azeroth Core `spell_gen_mount()` (how Invincible is implemented).

Specific speed increase mount spells are selected, based on rider skill and location, as follows (baseSpell is the dummy mount spell, see `72286` as an example):

- `baseSpell + 1` = `60%`, ground
- `baseSpell + 2` = `100%`, ground
- `baseSpell + 3` = `150%`, flying
- `baseSpell + 4` = `280%`, flying
- `baseSpell + 5` = `310%`, flying

All aforementioned spells should exist in `Spell.dbc`, otherwise spell script will fail validation and disable itself.

## Setup

Checkout the module into `modules` server directory, and recompile the server.

For every new adaptive mount, you will need to add 5 specific speed increase spells and a basic dummy spell. Example spells are provided in `Spells.csv` (IDs `250010`-`250015`).

Then, assign spell script to the dummy mount spell:

```sql
insert into spell_script_names (spell_id, ScriptName) values (250010, 'spell_mount_autoselect');
```

Where `250010` is your custom dummy mount spell. Example is, again, provided in `Spell.csv`.

If everything worked correctly, you should be able to summon the mount everywhere, ground-only where appropriate, or flying if allowed.
