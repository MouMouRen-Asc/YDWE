#include <base/hook/fp_call.h>
#include <base/util/memory.h>

#include <warcraft3/jass.h>
#include <warcraft3/jass/hook.h>
#include <warcraft3/war3_searcher.h>

#include "util.h"

uint32_t searchClearMessage() {
    uint32_t ptr;

    //=========================================
    // (1)
    //
    // push     "()V"
    // mov      edx, "ClearTextMessages"
    // mov      ecx, [ClearTextMessages�����ĵ�ַ] <----
    // call     BindNative
    //=========================================
    ptr = get_war3_searcher().search_string("ClearTextMessages");
    ptr = *(uint32_t*)(ptr + 0x05);

    //=========================================
    // (2)
    //  ClearTextMessages:
    //    xor       edx, edx
    //    lea       ecx, [edx + 1]
    //    call      GetGameUI
    //    mov       ecx, eax
    //    jmp       ClearTextMessageFrame          <----
    //=========================================
    ptr = next_opcode(ptr, 0xE9, 5);
    ptr = convert_function(ptr);

    //=========================================
    // (3)
    //  ClearTextMessageFrame:
    //    mov       ecx, [ecx + 0x3E8]
    //    jmp       ClearMessage                   <----
    //=========================================
    ptr = next_opcode(ptr, 0xE9, 5);
    ptr = convert_function(ptr);

    return ptr;
}

// ͸������Ч ��ʱ��һ�����������͸����
// λ��:
// 0 ������Ϣ (ħ��ֵ����, ...)
// 1 ��Ϸ��Ϣ (DisplayTextToPlayer, ...)
// 2 ������Ϣ (X_DisplayChat, ...)
// 3 ά�޵ȼ� (��ά�޵ȼ�, ...)
uint32_t __cdecl X_DisplayMessage(uint32_t message, float* duration, uint32_t where, uint32_t color) {
    if (where > 3)
        return false;

    uint32_t CGameUI = get_war3_searcher().get_gameui(0, 0);

    if (!CGameUI)
        return false;

    uint32_t pFrame = ReadMemory(CGameUI + 0x3E4 + 4 * where);
    if (!pFrame)
        return false;

    base::this_call_vf<void>(pFrame, 0x6C, jass::from_trigstring(jass::from_string(message)), &color, *duration, 0);
    return true;
}

// λ��ͬ��
uint32_t __cdecl X_ClearMessage(uint32_t where) {
    if (where > 3)
        return false;

    static uint32_t ClearMessage = searchClearMessage();
    uint32_t CGameUI = get_war3_searcher().get_gameui(0, 0);

    if (!CGameUI)
        return false;

    uint32_t pFrame = ReadMemory(CGameUI + 0x3E4 + 4 * where);
    if (!pFrame)
        return false;

    base::this_call<void>(ClearMessage, pFrame);
    return true;
}

// ���˰� (�������ܱ���JASS������nf��)
// λ��ͬ��
uint32_t __cdecl X_SetMessage(uint32_t message, float* duration, uint32_t where, uint32_t color) {
    if (where > 3)
        return false;

    uint32_t CGameUI = get_war3_searcher().get_gameui(0, 0);

    if (!CGameUI)
        return false;

    uint32_t pFrame = ReadMemory(CGameUI + 0x3E4 + 4 * where);
    if (!pFrame)
        return false;

    static uint32_t ClearMessage = searchClearMessage();
    base::this_call<void>(ClearMessage, pFrame);
    base::this_call_vf<void>(pFrame, 0x6C, jass::from_trigstring(jass::from_string(message)), &color, *duration, 0);
    return true;
}

init(GameUI_Message) {
    jass::japi_add((uint32_t)X_DisplayMessage,     "X_DisplayMessage",     "(SRII)B");
    jass::japi_add((uint32_t)X_ClearMessage,       "X_ClearMessage",       "(I)B");
    jass::japi_add((uint32_t)X_SetMessage,         "X_SetMessage",         "(SRII)B");
}