﻿#include <deque>

#include <warcraft3/jass/hook.h>
#include <base/hook/replace_pointer.h>
#include <base/hook/inline.h>

#include "TriggerEvent.h"
#include "util.h"

struct war3_event_damage_data {
    CUnit* source_unit;
    uint32_t weapon_type;
    uint32_t unk2;
    uint32_t flag;
    uint32_t amount;
    uint32_t damage_type;
    uint32_t unk6;
    uint32_t unk7;
    uint32_t attack_type;
};

struct event_damage_data {
    bool                    physical;
    bool                    change;
    war3_event_damage_data* data;
    uint32_t                old_amount;
    uint32_t                new_amount;

    event_damage_data(uint32_t is_physical, war3_event_damage_data* ptr)
        : physical(!!is_physical)
        , change(false)
        , data(ptr)
        , old_amount(0)
        , new_amount(0)
    { }

    bool is_same(uint32_t* damage1, uint32_t* damage2) const {
        if (!old_amount) 
            return true;

        if ((*damage1 ^ *damage2) != 0x80000000)
            return false;

        return old_amount == *damage1;
    }

    uint32_t damage_type() const {
        uint32_t v = data->damage_type;
        uint32_t n = 0;
        for (; v; v >>= 1)
            ++n;

        return n-1;
    }
};

definePlayerUnitEventData(X_PlayerUnitDamageEventData, 'pumd', 0, uint32_t amount;);

std::deque<event_damage_data> g_edd;

uint32_t RealGetEventDamage = 0;
uint32_t __cdecl FakeGetEventDamage() {
    if (!g_edd.empty()) {
        event_damage_data& edd = g_edd.back();
        if (edd.change)
            return g_edd.back().new_amount;
    }

    switch (TriggerEvent::GetTriggerEventId()) {
    case TriggerEvent::EVENT_PLAYER_UNIT_DAMAGED:
    case TriggerEvent::EVENT_PLAYER_UNIT_DAMAGING:
        if (X_PlayerUnitDamageEventData* pEventData = (X_PlayerUnitDamageEventData*)TriggerEvent::GetTriggerEventData('pumd'))
            return pEventData->amount;
        else
            return 0;
    default:
        return base::c_call<uint32_t>(RealGetEventDamage);
    }
}

uint32_t real_GetEventDamageSource = 0;
uint32_t __cdecl fake_GetEventDamageSource() {
    switch (TriggerEvent::GetTriggerEventId()) {
    case TriggerEvent::EVENT_PLAYER_UNIT_DAMAGED:
    case TriggerEvent::EVENT_PLAYER_UNIT_DAMAGING:
        if (X_PlayerUnitDamageEventData* pEventData = (X_PlayerUnitDamageEventData*)TriggerEvent::GetTriggerEventData('pumd'))
            return CGameWar3::GetInstance()->GetGameState()->ToHandle(UTIL::GetAgentByTag<CUnit>(pEventData->agent), 0);
        else
            return 0;
    default:
        return base::c_call<uint32_t>(real_GetEventDamageSource);
    }
}

uint32_t(__thiscall* RealUnitDamageFunc)(CUnit* pUnit, uint32_t a2, uint32_t ptr, uint32_t is_physical, CUnit* source_unit);
uint32_t __fastcall FakeUnitDamageFunc(CUnit* _this, uint32_t _edx, uint32_t a2, war3_event_damage_data* ptr, uint32_t is_physical, CUnit* source_unit) {
    g_edd.push_back(event_damage_data(is_physical, ptr));

    if (((CUnit*)_this)->GetOwner()->IsEventRegistered(TriggerEvent::EVENT_PLAYER_UNIT_DAMAGING)) {
        X_PlayerUnitDamageEventData* pPlayerUnitDamageEventData = (X_PlayerUnitDamageEventData*)Agile::CreateObject(X_PlayerUnitDamageEventData::typeID, false);

        pPlayerUnitDamageEventData->amount = ptr->amount;

        TriggerEvent::FirePlayerUnitEvent(pPlayerUnitDamageEventData, _this, ptr->source_unit, TriggerEvent::EVENT_PLAYER_UNIT_DAMAGING);
    }

    if (g_edd.back().change)
        ptr->amount = g_edd.back().new_amount; // 不用管 C6011:取消NULL指针引用

    g_edd.back() = event_damage_data(is_physical, ptr);

    uint32_t retval = RealUnitDamageFunc(_this, a2, (uint32_t)ptr, is_physical, source_unit);
    g_edd.pop_back();

    return retval;
}


enum EVENT_DAMAGE_DATA_TYPE {
    EVENT_DAMAGE_DATA_VAILD  = 0,
    EVENT_DAMAGE_DATA_PHYSICAL,
    EVENT_DAMAGE_DATA_IS_ATTACK,
    EVENT_DAMAGE_DATA_IS_RANGED,
    EVENT_DAMAGE_DATA_DAMAGE_TYPE,
    EVENT_DAMAGE_DATA_WEAPON_TYPE,
    EVENT_DAMAGE_DATA_ATTACK_TYPE,
};

uint32_t __cdecl EXGetEventDamageData(uint32_t type) {
    if (g_edd.empty())
        return 0;

    war3_event_damage_data* ptr = g_edd.back().data;

    switch (type) {
    case EVENT_DAMAGE_DATA_VAILD:
        return 1;
    case EVENT_DAMAGE_DATA_PHYSICAL:
        return g_edd.back().physical;
    case EVENT_DAMAGE_DATA_IS_ATTACK:
        return !!(ptr->flag & 0x100);
    case EVENT_DAMAGE_DATA_IS_RANGED:
        return !!(ptr->flag & 0x001);
    case EVENT_DAMAGE_DATA_DAMAGE_TYPE:
        //return ptr->damage_type;
        return g_edd.back().damage_type();
    case EVENT_DAMAGE_DATA_WEAPON_TYPE:
        return ptr->weapon_type;
    case EVENT_DAMAGE_DATA_ATTACK_TYPE:
        return ptr->attack_type;
    default:
        break;
    }

    return 0;
}

uint32_t __cdecl X_SetDamageEventDamageValue(uint32_t value) {
    if (g_edd.empty())
        return false;

    event_damage_data& edd = g_edd.back();
    edd.change = true;
    edd.old_amount = edd.data->amount;
    edd.new_amount = ReadMemory(value);

    return true;
}

void(__thiscall* real_CUnit_RunDamagedEvent)(CUnit* pUnit, float* amount, CUnit* pSrcUnit);
void __fastcall fake_CUnit_RunDamagedEvent(CUnit* _this, uint32_t, float* amount, CUnit* pSrcUnit) {
    if (_this->GetOwner()->IsEventRegistered(TriggerEvent::EVENT_PLAYER_UNIT_DAMAGED)) {
        X_PlayerUnitDamageEventData* pPlayerUnitDamageEventData = (X_PlayerUnitDamageEventData*)Agile::CreateObject('pumd', false);

        pPlayerUnitDamageEventData->amount = ReadMemory((uint32_t)amount);

        TriggerEvent::FirePlayerUnitEvent(pPlayerUnitDamageEventData, _this, pSrcUnit, TriggerEvent::EVENT_PLAYER_UNIT_DAMAGED);
    }
    real_CUnit_RunDamagedEvent(_this, amount, pSrcUnit);
    if (!g_edd.empty()) {
        event_damage_data& edd = g_edd.back();
        if (edd.change)
            *amount = *(float*)&edd.new_amount; // 不用管 C6011:取消NULL指针引用
    }
}

uint32_t __cdecl X_TriggerRegisterPlayerUnitDamagedEvent(uint32_t trigger, uint32_t player) {
    return TriggerEvent::TriggerRegisterPlayerUnitEvent(trigger, player, TriggerEvent::EVENT_PLAYER_UNIT_DAMAGED, NULL);
}

uint32_t __cdecl X_TriggerRegisterPlayerUnitDamagingEvent(uint32_t trigger, uint32_t player) {
    return TriggerEvent::TriggerRegisterPlayerUnitEvent(trigger, player, TriggerEvent::EVENT_PLAYER_UNIT_DAMAGING, NULL);
}

uint32_t __cdecl X_SetDamageEventAttackType(uint32_t type) {
    if (g_edd.empty() || type > 6)
        return false;
    g_edd.back().data->attack_type = type;
    return true;
}

uint32_t __cdecl X_SetDamageEventDamageType(uint32_t type) {
    if (g_edd.empty() || type > 26 || type == 1 || type == 2 || type == 3 || type == 6 || type == 7)
        return false;
    g_edd.back().data->damage_type = 1 << type;
    return true;
}

uint32_t __cdecl X_SetDamageEventWeaponType(uint32_t type) {
    if (TriggerEvent::GetTriggerEventId() != TriggerEvent::EVENT_PLAYER_UNIT_DAMAGING || g_edd.empty() || type > 23)
        return false;
    g_edd.back().data->weapon_type = type;
    return true;
}

init_L(UnitEvent_DamageData) {
    // TODO: 添加 指定单位事件 后重搞这整个源代码 (不使用 g_edd)
    setupEventData(X_PlayerUnitDamageEventData, 'pumd', TriggerEvent::EVENT_PLAYER_UNIT_DAMAGING, TriggerEvent::EVENT_PLAYER_UNIT_DAMAGED);

    // 覆盖掉 yd_jass_api.dll 的 hook
    RealUnitDamageFunc = FUNC::CUnit_OnDamage;
    base::hook::replace_pointer(VFN::CUnit + 0x120, (uint32_t)FakeUnitDamageFunc);
    // 覆盖掉 yd_jass_api 的函数
    jass::japi_hook("GetEventDamage", &RealGetEventDamage, (uint32_t)FakeGetEventDamage);
    jass::japi_add((uint32_t)EXGetEventDamageData,                     "EXGetEventDamageData",                     "(I)I");
    jass::japi_add((uint32_t)X_SetDamageEventDamageValue,              "EXSetEventDamage",                         "(R)B");
    // alias
    jass::japi_add((uint32_t)X_SetDamageEventDamageValue,              "X_SetDamageEventDamageValue",              "(R)B");

    // 玩家单位接受伤害事件
    real_CUnit_RunDamagedEvent = FUNC::CUnit_DispatchDamaged;
    base::hook::install((uint32_t*)&real_CUnit_RunDamagedEvent, (uint32_t)fake_CUnit_RunDamagedEvent);
    jass::japi_hook("GetEventDamageSource", &real_GetEventDamageSource, (uint32_t)fake_GetEventDamageSource);
    jass::japi_add((uint32_t)X_TriggerRegisterPlayerUnitDamagedEvent,  "X_TriggerRegisterPlayerUnitDamagedEvent",  "(Htrigger;Hplayer;)Hevent;");

    // 玩家单位接受伤害事件 (计算护甲前)
    jass::japi_add((uint32_t)X_TriggerRegisterPlayerUnitDamagingEvent, "X_TriggerRegisterPlayerUnitDamagingEvent", "(Htrigger;Hplayer;)Hevent;");
    jass::japi_add((uint32_t)X_SetDamageEventAttackType,               "X_SetDamageEventAttackType",               "(Hattacktype;)B");
    jass::japi_add((uint32_t)X_SetDamageEventDamageType,               "X_SetDamageEventDamageType",               "(Hdamagetype;)B");
    jass::japi_add((uint32_t)X_SetDamageEventWeaponType,               "X_SetDamageEventWeaponType",               "(Hweapontype;)B");
}