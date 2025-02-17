#pragma once
#include <stdint.h>
#include <Windows.h>

// ֻ��Ϊ�˷������
template<class t = uint32_t>
inline t ReadMemory(uint32_t addr) {
	return *(t*)addr;
}
template<class t = uint32_t>
inline void WriteMemory(uint32_t addr, t value) {
	*(t*)addr = value;
}
template<class t = uint32_t>
void WriteMemoryEx(uint32_t addr, t value) {
    DWORD old;
    VirtualProtect((void*)addr, sizeof(t), PAGE_EXECUTE_READWRITE, &old); // �����ɹ���� ��Ϊʧ������Ϸ���ܾͻ��첽 ����ֱ�ӱ���Ϸ
    WriteMemory(addr, value);
    VirtualProtect((void*)addr, sizeof(t), old, &old);
    FlushInstructionCache(GetCurrentProcess(), (void*)addr, sizeof(t)); // ��ȫ������Ǽ��Ϻ���
}
template<class t>
inline void PatchCallRelative(uint32_t addr, t func) {
    WriteMemoryEx(addr + 1, (uint32_t)func - addr - 5);
}