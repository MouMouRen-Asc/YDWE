#include <warcraft3/jass/func_value.h>
#include <warcraft3/war3_searcher.h>
#include <warcraft3/hashtable.h>
#include <warcraft3/jass.h>	
#include <base/util/do_once.h>
#include <map>
#include <string>  
#include <assert.h>

namespace warcraft3::jass {

	namespace detail {

#pragma pack(push)
#pragma pack(1)
		struct asm_opcode_5
		{
			uint8_t  opcode;   
			uint32_t value;
		};

		// push    offset aRR      ; "(R)R"           68 3C D9 95 6F
		// mov     edx, offset aDeg2rad ; "Deg2Rad"   BA 34 D9 95 6F
		// mov     ecx, offset sub_6F3B3510           B9 10 35 3B 6F
		// call    sub_6F455C20                       E8 A7 10 08 00 
		struct asm_register_native_function
		{
		private:
			asm_opcode_5 Push;
			asm_opcode_5 MovEdx;
			asm_opcode_5 MovEcx;
			asm_opcode_5 Call;

		public:
			bool verify() const
			{
				return ((Push.opcode  == 0x68)
					&& (MovEdx.opcode == 0xBA)
					&& (MovEcx.opcode == 0xB9)
					&& (Call.opcode   == 0xE8));
			}

			const char* get_param() const
			{
				return (const char*)(Push.value);
			}

			const char* get_name() const
			{
				return (const char*)(MovEdx.value);
			}

			uintptr_t get_address() const
			{
				return (uintptr_t)(MovEcx.value);
			}
		};
#pragma pack(pop)

	}

	func_mapping initialize_mapping(const char* startfunc)
	{
		func_mapping m;

		uintptr_t start = get_war3_searcher().search_string(startfunc);
		if (start)
		{
			for (detail::asm_register_native_function* ptr = (detail::asm_register_native_function*)(start - 6); ptr->verify(); ++ptr)
			{
				m.insert(std::make_pair(ptr->get_name(), func_value(ptr->get_param(), ptr->get_address())));
			}
		}

		return std::move(m);
	}

	func_value::func_value()
		: return_(TYPE_NONE)
		, param_()
		, address_(0)
		, has_sleep_(false)
	{ }

	func_value::func_value(const char* param, uintptr_t address)
		: return_(TYPE_NONE)
		, param_()
		, address_(address)
		, has_sleep_(false)
	{
		if (!param || param[0] != '(')
		{
			return ;
		}

		const char* it = ++param;
		bool is_end = false;
		for (; *it; ++it)
		{
			if (*it == ')')
			{
				is_end = true;
			}
			else if (isupper(static_cast<unsigned char>(*it)))
			{
				if (is_end)
				{
					return_ = (variable_type)(*it);
					break;
				}
				else
				{
					param_.push_back((variable_type)(*it));
				}
			}
		}
	}

	func_value::func_value(func_value const& that, uintptr_t address)
		: return_(that.return_)
		, param_(that.param_)
		, address_(address)
		, has_sleep_(false)
	{ }

	bool func_value::is_valid() const
	{
		return return_ != TYPE_NONE;
	}

	std::vector<variable_type> const& func_value::get_param() const
	{
		return param_;
	}

	variable_type const& func_value::get_return() const
	{
		return return_;
	}

	uintptr_t func_value::get_address() const
	{
		return address_;
	}

	uintptr_t func_value::call(const uintptr_t* param) const
	{
		return jass::call(address_, param, param_.size());
	}

	uintptr_t func_value::call(const call_param& param) const
	{
		assert(param.size() == param_.size());
		return call(param.data());
	}

	bool func_value::has_sleep() const
	{
		return has_sleep_;
	}

	void func_value::set_sleep(bool v)
	{
		has_sleep_ = v;
	}

	func_mapping jass_function;
	func_mapping japi_function;

	func_value const* jass_func(const char* proc_name)
	{
		if (!proc_name)
		{
			return nullptr;
		}

		DO_ONCE_NOTHREADSAFE()
		{
			jass_function = initialize_mapping("Deg2Rad");
		}

		auto it = jass_function.find(proc_name);
		if (it != jass_function.end() && it->second.is_valid())
		{
			return &(it->second);
		}

		return nullptr;
	}

	func_mapping undetected_natives;

	func_value const* japi_func(const char* proc_name)
	{
		if (!proc_name)
		{
			return nullptr;
		}

		if (undetected_natives.empty()) {
			uintptr_t ptr = get_war3_searcher().get_instance(5);
			uintptr_t end = *(uintptr_t*)(ptr + 0x20) - 0x10;
			uintptr_t start = *(uintptr_t*)(ptr + 0x24);
			if (start && end) {
				for (uintptr_t current = start; current != end; current = *(uintptr_t*)(current + 0x14))
					undetected_natives.insert(std::make_pair(*(char**)(current + 0x18), func_value(*(char**)(current + 0x24), *(uintptr_t*)(current + 0x1C))));
				for (auto& i : jass_function)
					if (undetected_natives.find(i.first) != undetected_natives.end())
						undetected_natives.erase(i.first);
				for (auto& i : japi_function)
					if (undetected_natives.find(i.first) != undetected_natives.end())
						undetected_natives.erase(i.first);
				// ���Ӻ��ˣ� ���÷���
				jass_function.insert(undetected_natives.begin(), undetected_natives.end());
				japi_function.insert(undetected_natives.begin(), undetected_natives.end());
			}
		}

		auto it = japi_function.find(proc_name);
		if (it != japi_function.end() && it->second.is_valid())
		{
			return &(it->second);
		}

		return nullptr;
	}
	
	bool japi_func_add(const char* proc_name, uintptr_t new_proc)
	{
		func_value const* nf = jass_func(proc_name);
		if (!nf)
		{
			return false;
		}
		return japi_function.insert(std::make_pair(proc_name, func_value(*nf, new_proc))).second;
	}

	bool japi_func_add(const char* proc_name, uintptr_t new_proc, const char* param)
	{
		return japi_function.insert(std::make_pair(proc_name, func_value(param, new_proc))).second;
	}

	bool japi_func_clean()
	{
		for (auto& i : undetected_natives) {
			// Ӧ��û��Ҫ����Ƿ����
			jass_function.erase(i.first);
			// ����Ĵ���ֱ�������
			//japi_function.erase(i.first);
		}
		undetected_natives.clear();
		japi_function.clear();
		return true;
	}
}
