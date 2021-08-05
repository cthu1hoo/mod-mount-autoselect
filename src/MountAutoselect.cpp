/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"

enum MountModSpells
{
    SPELL_COLD_WEATHER_FLYING           = 54197,
};

class MountAutoselect : public SpellScriptLoader
{
    public:
        MountAutoselect(): SpellScriptLoader("spell_mount_autoselect") { }

        class MountAutoselect_SpellScript : public SpellScript
        {
            PrepareSpellScript(MountAutoselect_SpellScript);

            bool Validate(SpellInfo const* spellInfo) override
            {
                int spellId = spellInfo->Id;

                return sSpellMgr->GetSpellInfo(spellId) &&
                    sSpellMgr->GetSpellInfo(spellId + 1) &&
                    sSpellMgr->GetSpellInfo(spellId + 2) &&
                    sSpellMgr->GetSpellInfo(spellId + 3) &&
                    sSpellMgr->GetSpellInfo(spellId + 4) &&
                    sSpellMgr->GetSpellInfo(spellId + 5);
            }

            void ChooseMount(SpellEffIndex effIndex)
            {
                int spellId = GetSpellInfo()->Id;

                //LOG_ERROR("scripts.spells", "Invoked for spell %d", spellId);

                int mount60 = spellId + 1;
                int mount100 = spellId + 2;
                int mount150 = spellId + 3;
                int mount280 = spellId + 4;
                int mount310 = spellId + 5;

                PreventHitDefaultEffect(effIndex);

                if (Player* target = GetHitPlayer())
                {
                    uint32 petNumber = target->GetTemporaryUnsummonedPetNumber();
                    target->SetTemporaryUnsummonedPetNumber(0);

                    // Prevent stacking of mounts and client crashes upon dismounting
                    target->RemoveAurasByType(SPELL_AURA_MOUNTED, ObjectGuid::Empty, GetHitAura());

                    // Triggered spell id dependent on riding skill and zone
                    bool canFly = false;
                    uint32 map = GetVirtualMapForMapAndZone(target->GetMapId(), target->GetZoneId());
                    if (map == 530 || (map == 571 && target->HasSpell(SPELL_COLD_WEATHER_FLYING)))
                        canFly = true;

                    AreaTableEntry const* area = sAreaTableStore.LookupEntry(target->GetAreaId());
                    // Xinef: add battlefield check
                    Battlefield* Bf = sBattlefieldMgr->GetBattlefieldToZoneId(target->GetZoneId());
                    if (!area || (canFly && ((area->flags & AREA_FLAG_NO_FLY_ZONE) || (Bf && !Bf->CanFlyIn()))))
                        canFly = false;

                    uint32 mount = 0;
                    switch (target->GetBaseSkillValue(SKILL_RIDING))
                    {
                        case 75:
                            mount = mount60;
                            break;
                        case 150:
                            mount = mount100;
                            break;
                        case 225:
                            if (canFly)
                                mount = mount150;
                            else
                                mount = mount100;
                            break;
                        case 300:
                            if (canFly)
                            {
                                if (target->Has310Flyer(false))
                                    mount = mount310;
                                else
                                    mount = mount280;
                            }
                            else
                                mount = mount100;
                            break;
                        default:
                            break;
                    }

                    //LOG_ERROR("scripts.spells", "Selecting spell %d based on skill %d canFly %d", mount, target->GetBaseSkillValue(SKILL_RIDING), canFly);

                    if (mount)
                    {
                        PreventHitAura();
                        target->CastSpell(target, mount, true);
                    }

                    if (petNumber)
                        target->SetTemporaryUnsummonedPetNumber(petNumber);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(MountAutoselect_SpellScript::ChooseMount, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript * GetSpellScript() const override
        {
            return new MountAutoselect_SpellScript();
        }
};

void AddMountAutoselectScripts()
{
    new MountAutoselect();
}
