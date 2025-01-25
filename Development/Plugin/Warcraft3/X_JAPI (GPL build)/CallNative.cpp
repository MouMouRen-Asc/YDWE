#include <warcraft3/jass/opcode.h>
#include <warcraft3/hashtable.h>
#include <warcraft3/war3_searcher.h>
#include <base/hook/inline.h>
#include <base/hook/fp_call.h>
#include <base/util/memory.h>
#include <vector>

namespace warcraft3::japi {
    uint32_t searchJassVM_CallNative() {
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

        addr = warcraft3::next_opcode(addr, 0xFF, 7);		//������һ��call
        addr += 0x3;
        addr = *(uintptr_t*)addr;							//ȡ����ת��ַ���ַ
        addr += 0x4C;										//��ת��ƫ��0x13 * 4�ĵ�ַ��
        addr = *(uintptr_t*)addr;
        addr = warcraft3::next_opcode(addr, 0xE8, 5);		//������һ��call
        addr += 0x5;										//+5
        addr = warcraft3::next_opcode(addr, 0xE8, 5);		//�������ڶ���call
        addr = warcraft3::convert_function(addr);			//����

        return addr;

    }
    
    uint32_t getJassVM_CallNative (){
        static uint32_t ret = searchJassVM_CallNative();
        return ret;
    }

    uint32_t searchConvertFuncAddr() {
        uint32_t addr = getJassVM_CallNative();
        addr = warcraft3::next_opcode(addr, 0xE8, 5);		//������һ��call
        addr += 0x5;										//+5
        addr = warcraft3::next_opcode(addr, 0xE8, 5);		//�������ڶ���call
        addr = warcraft3::convert_function(addr);			//����
        return addr;
    }

    uint32_t __fastcall fake_JassVM_CallNative(jass_vm_t* _this, uint32_t edx, char* funcName) {
        edx;
        hashtable::native_func_node* nf = get_native_function_hashtable()->find(funcName);
        char* param = (char*)nf->param_list_ + 1; // ����ǰ���"("
        stackframe_t* stack = _this->stackframe;
        uint32_t param_count = nf->param_num_;
        std::vector<uint32_t> args(param_count);
        std::vector<float> args_float(param_count);
        uint32_t tmp;
        while (param_count) {
            hashtable::variable_node* var = ReadMemory<hashtable::variable_node*>((uint32_t)stack + 4 * (stack->index - param_count--) + 12);
            switch (*param++) {
            case 'C':
            {
                static uint32_t convertFuncAddr = searchConvertFuncAddr();
                if (var->array_ == jass::OPCODE_VARIABLE_TYPE::OPCODE_VARIABLE_CODE)
                    args[param_count] = base::this_call<int>(convertFuncAddr, _this, var->value_);
                else
                    args[param_count] = 0;
                break;
            }
            case 'R':
                args_float[param_count] = *(float*)&var->value_; // ��ֹnf�޸Ĳ���
                args[param_count] = (uint32_t)&args_float[param_count];
                break;
            case 'S':
                args[param_count] = _this->string_table->get_no_ref(var->value_);
                break;
            case 'H':
                if (param_count) {// ���һ������û��Ҫѭ��
                    while (*++param != ';')
                        ;
                    ++param;
                }
                args[param_count] = var->value_;
                break;
            default:
                args[param_count] = var->value_;
                break;
            }
        }
        stack->index -= nf->param_num_;

        for (auto i = args.begin(); i != args.end(); i++) {
            tmp = (*i);
            _asm push tmp;
        }

        /*
        tmp = nf->param_num_ * 4;
        ((uint32_t(__cdecl*)())(nf->func_address_))();
        _asm {
            add esp, tmp;
            mov tmp, eax;
        }
        */

        _asm mov tmp, esp; // return bug ���ܻ��޸ĺ���ָ��Ϊ����stdcall����, ����ȼ��㲢����һ��stackָ��
        tmp += nf->param_num_ * 4;
        ((uint32_t(__cdecl*)())(nf->func_address_))();
        _asm {
            mov esp, tmp;
            mov tmp, eax;
        }

        return tmp;
    }

    // ԭ��ֻ֧��15������, ����15����ɻ��������
    void InitializeFix_JassVM_CallNative() {
        uint32_t real_JassVM_CallNative = getJassVM_CallNative();
        base::hook::install(&real_JassVM_CallNative, (uintptr_t)fake_JassVM_CallNative);
    }
}