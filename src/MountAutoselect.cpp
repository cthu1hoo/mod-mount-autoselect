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

    // these should be defined in Spells.dbc:
    SPELL_MOUNT_SPEED_EFFECT_60         = 240001,
    SPELL_MOUNT_SPEED_EFFECT_100        = 240002,
    SPELL_MOUNT_SPEED_EFFECT_150        = 240003,
    SPELL_MOUNT_SPEED_EFFECT_280        = 240004,
    SPELL_MOUNT_SPEED_EFFECT_310        = 240005,
};

class MountAutoselect : public SpellScriptLoader
{
    public:
        MountAutoselect(): SpellScriptLoader("spell_mount_autoselect") { }

        class MountAutoselect_AuraScript : public AuraScript
        {
            PrepareAuraScript(MountAutoselect_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_MOUNT_SPEED_EFFECT_60)
                || !sSpellMgr->GetSpellInfo(SPELL_MOUNT_SPEED_EFFECT_100)
                || !sSpellMgr->GetSpellInfo(SPELL_MOUNT_SPEED_EFFECT_150)
                || !sSpellMgr->GetSpellInfo(SPELL_MOUNT_SPEED_EFFECT_280)
                || !sSpellMgr->GetSpellInfo(SPELL_MOUNT_SPEED_EFFECT_310))
            {
                return false;
            }
            return true;
        }

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Player* target = (Player*)GetTarget();

                /* mostly copy-pasted from spell_generic.cpp:4827 HandleMount() -fox */

                uint32 petNumber = target->GetTemporaryUnsummonedPetNumber();
                target->SetTemporaryUnsummonedPetNumber(0);

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
                    case 0:
                        mount = SPELL_MOUNT_SPEED_EFFECT_60;
                        break;
                    case 75:
                        mount = SPELL_MOUNT_SPEED_EFFECT_100;
                        break;
                    case 150:
                        mount = SPELL_MOUNT_SPEED_EFFECT_150;
                        break;
                    case 225:
                        if (canFly)
                            mount = SPELL_MOUNT_SPEED_EFFECT_150;
                        else
                            mount = SPELL_MOUNT_SPEED_EFFECT_100;
                        break;
                    case 300:
                        if (canFly)
                        {
                            if (target->Has310Flyer(false))
                                mount = SPELL_MOUNT_SPEED_EFFECT_310;
                            else
                                mount = SPELL_MOUNT_SPEED_EFFECT_280;
                        }
                        else
                            mount = SPELL_MOUNT_SPEED_EFFECT_100;
                        break;
                    default:
                        break;
                }

                //LOG_ERROR("scripts.spells", "Selecting spell %d based on skill %d canFly %d", mount, target->GetBaseSkillValue(SKILL_RIDING), canFly);

                if (mount)
                {
                    target->CastSpell(target, mount, aurEff);
                }

                if (petNumber)
                    target->SetTemporaryUnsummonedPetNumber(petNumber);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();

                target->RemoveAurasByType(SPELL_AURA_MOUNTED, ObjectGuid::Empty);
                target->RemoveAurasByType(SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED, ObjectGuid::Empty);
                target->RemoveAurasByType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED, ObjectGuid::Empty);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(MountAutoselect_AuraScript::OnApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(MountAutoselect_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript * GetAuraScript() const override
        {
            return new MountAutoselect_AuraScript();
        }
};

void AddMountAutoselectScripts()
{
    new MountAutoselect();
}
