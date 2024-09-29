// w4454962 fix (issue 197)
#include <warcraft3/jass/trampoline.h>
#include <warcraft3/hashtable.h>
#include <warcraft3/war3_searcher.h>
#include <base/hook/fp_call.h>

uintptr_t get_conver_func_addr()
{
	warcraft3::war3_searcher& game = warcraft3::get_war3_searcher();
	uintptr_t addr = game.search_string("config");
	addr += sizeof uintptr_t;

	addr = warcraft3::next_opcode(addr, 0xE8, 5);		//���������һ��call
	addr = warcraft3::convert_function(addr);			//�����������

	addr = warcraft3::next_opcode(addr, 0xE8, 5);		//������һ��call
	addr += 0x5;										//+5
	addr = warcraft3::next_opcode(addr, 0xE8, 5);		//�������ڶ���call
	addr = warcraft3::convert_function(addr);			//����


	addr = warcraft3::next_opcode(addr, 0xE8, 5);		//������һ��call
	addr = warcraft3::convert_function(addr);			//����

	addr += 0x8A;										//ƫ�Ƶ�swatch��ת��
	addr = *(uintptr_t*)addr;							//ȡ����ת��ַ���ַ
	addr += 0x48;										//��ת��ƫ��0x12 * 4�ĵ�ַ��
	addr = *(uintptr_t*)addr;
	addr += 0x22;										//ƫ�Ƶ�Ŀ�꺯��λ��
	addr = warcraft3::convert_function(addr);			//ȡĿ���ַ

	addr = warcraft3::next_opcode(addr, 0xE8, 5);		//������һ��call
	addr += 0x5;										//+5
	addr = warcraft3::next_opcode(addr, 0xE8, 5);		//�������ڶ���call
	addr = warcraft3::convert_function(addr);			//����

	return addr;

}

//��ָ��jass������ַת��Ϊcode id
int convert_code_id(uintptr_t func_addr)
{
	static uintptr_t addr = get_conver_func_addr();
	uintptr_t jvm = (uintptr_t)warcraft3::get_jass_vm();
	return base::this_call<int>(addr, jvm, func_addr);
}

namespace warcraft3 { namespace jass {

	trampoline::trampoline()
		: mybase()
	{

	}


	uintptr_t trampoline::code() const
	{
		uintptr_t address = (uintptr_t)this;
		return convert_code_id(address);
	}
}}