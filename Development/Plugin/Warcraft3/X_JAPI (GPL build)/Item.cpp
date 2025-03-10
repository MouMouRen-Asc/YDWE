﻿#include <map>

#include <warcraft3/event.h>  
#include <warcraft3/hashtable.h>
#include <warcraft3/jass/hook.h>

#include "util.h"

static uint32_t hashid(uint32_t id) {
    static uint32_t box[] = {
        0x098AA3D0C, 0x0F67BCA9E, 0x0C46CA84C, 0x02AC9D845,
        0x09A1CF1DD, 0x06450148E, 0x08516213D, 0x0C0882BBF,
        0x0F10C2A9C, 0x09D7CF013, 0x0CD845F5E, 0x01D4BD837,
        0x01055F69A, 0x0A6A87DCD, 0x0312D8D9E, 0x0645A1CEC,
    };
    uint32_t hashlo = 0x7FED7FED;
    uint32_t hashhi = 0xEEEEEEEE;
    for (; id > 0;) {
        hashlo = (box[(id >> 4) & 0x0F] - box[id & 0x0F]) ^ (hashhi + hashlo);
        hashhi = hashhi + (id & 0xFF) + 32 * hashhi + hashlo + 3;
        id >>= 8;
    }
    return hashlo;
}

enum class ITEM_DATA_TYPE_INTEGER {
    ITEM_DATA_LEVEL = 1,
    ITEM_DATA_TYPE,
    ITEM_DATA_POWERUP,
    ITEM_DATA_SELLABLE,
    ITEM_DATA_PAWNABLE,
    ITEM_DATA_COLOR,
};
struct item_data_node : public hashtable::node {
    uint32_t unk1[8];
    uint32_t level;
    uint32_t type;
    uint32_t unk2[3];
    uint32_t powerup;
    uint32_t sellable;
    uint32_t pawnable;
    uint32_t unk3[15];
    uint32_t color;
};

uint32_t __cdecl X_GetItemDataInteger(uint32_t typeID, uint32_t dataType) {
    hashtable::table<item_data_node>* table = (hashtable::table<item_data_node>*)ADDR::item_data_table;
    if (!table)
        return false;

    item_data_node* ptr = table->find(hashid(typeID));
    if (!ptr)
        return false;

    switch ((ITEM_DATA_TYPE_INTEGER)dataType) {
    case ITEM_DATA_TYPE_INTEGER::ITEM_DATA_LEVEL:
        return ptr->level;
    case ITEM_DATA_TYPE_INTEGER::ITEM_DATA_TYPE:
        return ptr->type;
    case ITEM_DATA_TYPE_INTEGER::ITEM_DATA_POWERUP:
        return ptr->powerup;
    case ITEM_DATA_TYPE_INTEGER::ITEM_DATA_SELLABLE:
        return ptr->sellable;
    case ITEM_DATA_TYPE_INTEGER::ITEM_DATA_PAWNABLE:
        return ptr->pawnable;
    case ITEM_DATA_TYPE_INTEGER::ITEM_DATA_COLOR:
        return ptr->color;
    default:
        return 0;
    }
}
uint32_t __cdecl X_SetItemDataInteger(uint32_t typeID, uint32_t dataType, uint32_t value) {
    hashtable::table<item_data_node>* table = (hashtable::table<item_data_node>*)ADDR::item_data_table;
    if (!table) 
        return false;

    item_data_node* ptr = table->find(hashid(typeID));
    if (!ptr)
        return false;

    switch ((ITEM_DATA_TYPE_INTEGER)dataType) {
    case ITEM_DATA_TYPE_INTEGER::ITEM_DATA_LEVEL:
        ptr->level = value;
        return true;
    case ITEM_DATA_TYPE_INTEGER::ITEM_DATA_TYPE:
        if (value < 0 || value > 7)
            return false;
        ptr->type = value;
        return true;
    case ITEM_DATA_TYPE_INTEGER::ITEM_DATA_POWERUP:
        ptr->powerup = !!value;
        return true;
    case ITEM_DATA_TYPE_INTEGER::ITEM_DATA_SELLABLE:
        ptr->sellable = !!value;
        return true;
    case ITEM_DATA_TYPE_INTEGER::ITEM_DATA_PAWNABLE:
        ptr->pawnable = !!value;
        return true;
    case ITEM_DATA_TYPE_INTEGER::ITEM_DATA_COLOR:
        ptr->color = value;
        return true;
    default:
        return false;
    }
}

std::map<CItem*, uint32_t> item_color;
uint32_t* (__fastcall* real_GetItemColorById)(uint32_t* output, uint32_t itemID);
uint32_t* __fastcall fake_GetItemColorById(uint32_t* out, uint32_t pItemID /* 原本是物品类型, 换成 lea 了所以现在是指针 */) {
    CItem* pItem = (CItem*)(pItemID - 0x30);
    auto i = item_color.find(pItem);
    if (i == item_color.end())
        return real_GetItemColorById(out, ReadMemory(pItemID));
    *out = i->second;
    return out;
}

uint32_t __cdecl X_GetItemColor(uint32_t item) {
    CItem* pItem = ConvertHandle<CItem>(item);
    if (!pItem)
        return 0xFFFFFFFF;
    return *fake_GetItemColorById((uint32_t*)&pItem/* 复用变量 */, (uint32_t)&pItem->ID);
}
uint32_t __cdecl X_SetItemColor(uint32_t item, uint32_t color) {
    CItem* pItem = ConvertHandle<CItem>(item);
    if (!pItem)
        return false;
    item_color[pItem] = color;
    return true;
}
uint32_t __cdecl X_ResetItemColor(uint32_t item) {
    CItem* pItem = ConvertHandle<CItem>(item);
    if (!pItem)
        return false;
    auto i = item_color.find(pItem);
    if (i == item_color.end())
        return false;
    item_color.erase(i);
    return true;
}

init_L(Item) {
    jass::japi_add((uint32_t)X_GetItemDataInteger, "X_GetItemDataInteger",     "(II)I");
    jass::japi_add((uint32_t)X_SetItemDataInteger, "X_SetItemDataInteger",     "(III)B");
    jass::japi_add((uint32_t)X_GetItemColor,       "X_GetItemColor",           "(Hitem;)I");
    jass::japi_add((uint32_t)X_SetItemColor,       "X_SetItemColor",           "(Hitem;I)B");
    jass::japi_add((uint32_t)X_ResetItemColor,     "X_ResetItemColor",         "(Hitem;)B");

    real_GetItemColorById = FUNC::GetItemColorById;
    WriteMemoryEx<uint8_t>(ASM::CItem_UpdateColor_mov_itemID, 0x8D); // mov -> lea
    PatchCallRelative(ASM::CItem_UpdateColor_call_GetItemColorById, fake_GetItemColorById);
    event_agent_destructor([](uint32_t _this) {
        auto i = item_color.find((CItem*)_this);
        if (i != item_color.end()) {
            item_color.erase(i);
        }
    });
}