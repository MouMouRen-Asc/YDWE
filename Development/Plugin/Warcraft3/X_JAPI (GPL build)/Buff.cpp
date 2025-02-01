#include <base/hook/fp_call.h>
#include <warcraft3/jass/hook.h>
#include <warcraft3/war3_searcher.h>    
#include <warcraft3/version.h>
#include <base/util/memory.h>
#include "util.h"

uint32_t searchCUnit_AddAbility() {
    uint32_t ptr = get_vfn_ptr(".?AVCAbilityNeutral@@");
    ptr = ReadMemory(ptr + 0x80); // vftable + 0x80
    ptr = next_opcode(ptr, 0xE8, 5);
    ptr += 5;
    ptr = next_opcode(ptr, 0xE8, 5);
    ptr += 5;
    ptr = next_opcode(ptr, 0xE8, 5);
    ptr += 5;
    ptr = next_opcode(ptr, 0xE8, 5);
    ptr += 5;
    ptr = next_opcode(ptr, 0xE8, 5);
    ptr += 5;
    ptr = next_opcode(ptr, 0xE8, 5);
    if (get_war3_searcher().get_version() >= version_127a) {
        ptr += 5;
        ptr = next_opcode(ptr, 0xE8, 5);
    }
    return convert_function(ptr);
}

uint32_t searchCBuff_UpdateDisplay() {
    uint32_t ptr = get_vfn_ptr(".?AVCBuff@@");
    ptr = ReadMemory(ptr + 0xA4); // vftable + 0xA4
    ptr = next_opcode(ptr, 0xE8, 5);
    return convert_function(ptr);
}

void CUnit_AddBuff(uint32_t pUnit, uint32_t pBuff) {
    static uint32_t pCUnit_AddAbility = searchCUnit_AddAbility();
    base::this_call<void>(pCUnit_AddAbility, pUnit, pBuff);
    static uint32_t pCBuff_UpdateDisplay = searchCBuff_UpdateDisplay();
    base::this_call<void>(pCBuff_UpdateDisplay, pBuff);
}

uint32_t __cdecl X_UnitAddBuff(uint32_t target_unit, uint32_t src_unit, uint32_t typeID, uint32_t buffID, uint32_t level, uint32_t spell_steal_priority, uint32_t* duration, uint32_t* data1, uint32_t* data2, uint32_t* data3, uint32_t* data4, uint32_t* data5, uint32_t* data6, uint32_t* data7, uint32_t* data8, uint32_t* data9, uint32_t* data10, uint32_t* data11) {
    if (!target_unit)
        return false;
    switch (typeID) {
    case 'Bdet':
    case 'BUan':
    case 'BFig':
    case 'BEfn':
    case 'Bhwd':
    case 'Bplg':
    case 'Brai':
    case 'BHwe':
    case 'BTLF':
    case 'Bdef':
    case 'Bdig':
    case 'BHds':
    case 'BNdo':
    case 'BNdi':
    case 'BNdh':
    case 'BOeq':
    case 'Beat':
    case 'Bgra':
    case 'Bena':
    case 'Beng':
    case 'Bwea':
    case 'Bweb':
    case 'BEer':
    case 'Bcrs':
    case 'Beye':
    case 'Bfae':
    case 'Bshs':
    case 'BNic':
    case 'BNlm':
    case 'BNso':
    case 'BPSE':
    case 'BHfa':
    case 'BUfa':
    case 'BOhx':
    case 'BNht':
    case 'Bprg':
    case 'Bhea':
    case 'Brej':
    case 'BIrm':
    case 'BIrl':
    case 'BIrg':
    case 'Bfre':
    case 'BIcb':
    case 'BIrb':
    case 'BIpv':
    case 'BUcb':
    case 'BEia':
    case 'BEim':
    case 'BNpi':
    case 'Bpig':
    case 'BIcf':
    case 'BUim':
    case 'BNin':
    case 'Binf':
    case 'Binv':
    case 'Bvul':
    case 'Blsh':
    case 'Bams':
    case 'Bam2':
    case 'Bmfl':
    case 'BNms':
    case 'BOmi':
    case 'BIil':
    case 'BNpa':
    case 'BNpm':
    case 'Bpsh':
    case 'Bply':
    case 'BNsa':
    case 'BHtc':
    case 'BCtc':
    case 'BNfy':
    case 'BNcg':
    case 'BNto':
    case 'Buhf':
    case 'Buns':
    case 'BOvc':
    case 'BOvd':
    case 'BOwd':
    case 'BImo':
    case 'BNwm':
    case 'Bmec':
    case 'BNsg':
    case 'BNsq':
    case 'BNsw':
    case 'BOwk':
    case 'Bfrz':
    case 'Bliq':
    case 'BNab':
    case 'BNsl':
    case 'BHbn':
    case 'Bbsk':
    case 'BNdm':
    case 'BNba':
    case 'Bblo':
    case 'Bfzy':
    case 'Bpos':
    case 'Bpoc':
    case 'Bcmg':
    case 'Bclf':
    case 'BHca':
    case 'Bcri':
    case 'Bcy2':
    case 'Bcyc':
    case 'Broa':
    case 'BNbr':
    case 'BEst':
    case 'BEsh':
    case 'BNsi':
    case 'BUsl':
    case 'BUsp':
    case 'BNdc':
    case 'Bslo':
    case 'Bspe':
    case 'BNss':
    case 'Bspl':
    case 'BEsv':
    case 'Bvng':
    case 'BOsf':
    case 'BIsh':
    case 'Bstt':
        break;
        // ��֧�ֵ�����buff
    case 'Boar': // �⻷
    case 'Barm': // �⻷
    case 'Babr': // �⻷
    case 'Bsha': // �Ҳ����ĸ�����ʹ��
    case 'Btrv': // �Ҳ����ĸ�����ʹ�� - ʵ�����ƺ���ability
    case 'BUdd': // AOE
    case 'Bdvv': // �󶨼���
    case 'Bdtb': // ����
    case 'Bdcb': // ����
    case 'Bdtl': // ����
    case 'Bdcl': // ����
    case 'Bdtm': // ����
    case 'Bdcm': // ����
    case 'Bdbb': // ����
    case 'Bdbl': // ����
    case 'Bdbm': // ����
    case 'BOea': // AOE
    case 'BNef': // �󶨼���
    case 'BNva': // AOE
    case 'BHfs': // ������
    case 'Bchd': // ������
    case 'BIpi': // ����
    case 'BIpb': // ����
    case 'BIpd': // ����
    case 'Blsa': // AOE
    case 'Bmlt': // ����
    case 'Bmlc': // ����
    case 'Bmfa': // AOE
    case 'BEme': // �󶨼���
    case 'Bmil': // �󶨼���
    case 'Bpxf': // ����
    case 'Bphx': // �󶨼���
    case 'Bpsd': // ����
    case 'Bssd': // ����
    case 'Bpsi': // ����
    case 'Bpoi': // ����
    case 'Bssi': // ����
    case 'Bspo': // ����
    case 'BNcs': // AOE
    case 'Btdg': // AOE
    case 'Btsp': // ����
    case 'Btsa': // ����AOE
    case 'BOww': // AOE
    case 'Bbof': // ����
    case 'BNhs': // AOE
    case 'BNcr': // �󶨼���
    case 'BNtm': // ������
    case 'BHab': // �⻷
    case 'BOac': // �⻷
    case 'Bakb': // �⻷
    case 'BEar': // �⻷
    case 'BHad': // �⻷
    case 'BOae': // �⻷
    case 'Basl': // �⻷
    case 'Bapl': // �⻷
    case 'BEah': // �⻷
    case 'BUau': // �⻷
    case 'BUav': // �⻷
    case 'BHav': // �⻷
    case 'Bbar': // ������
    case 'BHbd': // ������
    case 'BHbz': // AOE
    case 'BNrf': // AOE
    case 'Bcsd': // ����
    case 'Bcsi': // ����
    case 'Brpl': // ���ӵķ�vftable call
    case 'Brpm': // ���ӵķ�vftable call
    case 'BUcs': // �󶨼���
    case 'BIsv': // �󶨼���
    case 'Bspa': // ������
    case 'Bprv': // �Ҳ����ĸ�����ʹ��
    case 'BTAR': // �Ҳ����ĸ�����ʹ��
    default:
        return false;
    }
    uint32_t pTargetUnit = handle_to_object(target_unit);
    if (!pTargetUnit)
        return false;
    uint32_t pSrcUnit = NULL;
    if (src_unit)
        pSrcUnit = handle_to_object(src_unit);
    if (!pSrcUnit)
        pSrcUnit = pTargetUnit;
    uint32_t pBuff = create_by_typeid(typeID);
    if (!pBuff)
        return false;
    uint32_t pBuffInfo[] = {
        buffID,
        0, // ???
        level ? level - 1 : 0,
        spell_steal_priority, // spell steal priority
        (bool)level, // show level
        0, // ??? (bool)
        0, // normal duration (�ƺ�û��)
        0, // hero/resistance skin duration (�ƺ�û��)
    };
    base::this_call<void>(ReadMemory(ReadMemory(pBuff) + 0x324), pBuff, pBuffInfo);
#define do_if(typeID, vfOff, ...) case typeID: base::this_call_vf<void>(pBuff, vfOff, __VA_ARGS__); break
    switch (typeID) {
        // Boar ���ٷֱȻظ�(����)
        // do_if('Boar', 0x36C, pSrcUnit, pTargetUnit, *data1);
        // Barm ���ٷֱȻظ�(����)
        // do_if('Barm', 0x36C, pSrcUnit, pTargetUnit, *data1);
        // Babr ���ٷֱȻظ�(����)
        // do_if('Babr', 0x36C, pSrcUnit, pTargetUnit, *data1);
        // ���(����) �������(����)
        do_if('Bdet', 0x354, pTargetUnit, *data1, duration, *data2);
        // Bsha
        // do_if('Bsha', 0x354, pTargetUnit, ?, ?, ?);
        // Btrv
        // do_if('Btrv', 0x84, pTargetUnit);
        // ������
        // do_if('BUan', 0x31C, pTargetUnit, duration);
        // �Ƿ��޵�(����) δ֪(1)
        do_if('BUan', 0x364, pTargetUnit, duration, *data1, *data2);
        // ������
        do_if('BFig', 0x31C, pTargetUnit, duration);
        // ������
        do_if('BEfn', 0x31C, pTargetUnit, duration);
        // ������
        do_if('Bhwd', 0x31C, pTargetUnit, duration);
        // ������
        do_if('Bplg', 0x31C, pTargetUnit, duration);
        // ������
        do_if('Brai', 0x31C, pTargetUnit, duration);
        // ������
        do_if('BHwe', 0x31C, pTargetUnit, duration);
        // ������
        do_if('BTLF', 0x31C, pTargetUnit, duration);
        // BUdd
        // ����
        do_if('Bdef', 0x354, pTargetUnit, duration, data1);
        // Bdvv ���(����) δ֪(0xFFFFFFFF)
        // do_if('Bdvv', 0x354, pSrcUnit, duration, *data1, duration?, 0xFFFFFFFF);
        // ÿ���˺�
        do_if('Bdig', 0x378, pTargetUnit, pSrcUnit, data1);
        // ������
        do_if('BHds', 0x31C, pTargetUnit, duration);
        // ÿ���˺� ���(����) �ٻ���λ����(����) �ٻ���λ����(����) �ٻ���λ����ʱ�� ħ��Ч��(����)
    case 'BNdo':
    {
        base::this_call_vf<void>(pBuff, 0x354, pTargetUnit, pSrcUnit, duration, data1);
        uint32_t pAgent = find_objectid_64({ ReadMemory<uint32_t>(pSrcUnit + 0xC), ReadMemory<uint32_t>(pSrcUnit + 0x10) });
        if (pAgent && ReadMemory<uint32_t>((uint32_t)pAgent + 0xC) == '+agl')
            WriteMemory(pBuff + 0xFC, objectid_64(ReadMemory<uint32_t>((uint32_t)pAgent + 0x14), ReadMemory<uint32_t>((uint32_t)pAgent + 0x18)));
        WriteMemory(pBuff + 0x108, *data2);
        WriteMemory(pBuff + 0x10C, *data3);
        WriteMemory(pBuff + 0x110, *data4);
        WriteMemory(pBuff + 0x118, *data5);
        WriteMemory(pBuff + 0x11C, *data6);
    }
        break;
        // ������
        do_if('BNdi', 0x31C, pTargetUnit, duration);
        // Bdtb
        // Bdcb
        // Bdtl
        // Bdcl
        // Bdtm
        // Bdcm
        // Bdbb
        // Bdbl
        // Bdbm
        // ��������(%) ��������(%) ��ֹ����(����) ʧ����(%)
        do_if('BNdh', 0x35C, pTargetUnit, duration, pSrcUnit, data1, data2, *data3, data4);
        // ��������(%) ��������(%)
        do_if('BOeq', 0x354, pTargetUnit, duration, pSrcUnit, data1, data2);
        // BOea δ֪ptr(1) δ֪ptr(0)
        // do_if('BOea', 0x354, pTargetUnit, duration, pSrcUnit, data1, data2);
        // �������ָ� ��ħ���ָ� δ֪(0) δ֪(0)
        do_if('Beat', 0x354, pTargetUnit, duration, data1, data2, *data3, *data4);
        // ��������(����) ��������(����) ��������(����) ��ľ����(����)
        do_if('Bgra', 0x354, pTargetUnit, duration, *data1, *data2, *data3, *data4);
        // BNef
        // do_if('BNef', 0x354, pTargetUnit, duration, pAbility);
        // ���(����) ���е�λ׹��ʱ�� ���е�λ�߶� ��ս������Χ
        do_if('Bena', 0x35C, pTargetUnit, pSrcUnit, *data1, duration, data2, data3, data4);
        // ���(����) ���е�λ׹��ʱ�� ���е�λ�߶� ��ս������Χ
        do_if('Beng', 0x35C, pTargetUnit, pSrcUnit, *data1, duration, data2, data3, data4);
        // ���(����) ���е�λ׹��ʱ�� ���е�λ�߶� ��ս������Χ
        do_if('Bwea', 0x35C, pTargetUnit, pSrcUnit, *data1, duration, data2, data3, data4);
        // ���(����) ���е�λ׹��ʱ�� ���е�λ�߶� ��ս������Χ
        do_if('Bweb', 0x35C, pTargetUnit, pSrcUnit, *data1, duration, data2, data3, data4);
        // ÿ���˺�
        do_if('BEer', 0x354, pTargetUnit, pSrcUnit, duration, data1);
        // ʧ����(%)
        do_if('Bcrs', 0x354, pTargetUnit, duration, pSrcUnit, data1);
        // ������
        do_if('Beye', 0x31C, pTargetUnit, duration);
        // ������Ұ�����(����) ���׼���
        do_if('Bfae', 0x35C, pTargetUnit, duration, *data1, data2);
        // ������Ұ�����(����) δ֪(-1)
        do_if('Bshs', 0x354, pTargetUnit, *data1, duration, *data2);
        // ���(����), δ֪(0), �˺�ֵ����, ȫ�˺���Χ, ȫ�˺�, ���˺���Χ, ���˺�, �ӳ�
        do_if('BNic', 0x354, pTargetUnit, duration, pSrcUnit, *data1, *data2, data3, data4, data5, data6, data7, data8);
        // ���Ѵ���(����), �����ӳ�, �������蹥������(����), �������ֵ����, �����������ڽ���, �����Ѵ���(����), ʣ����Ѵ���(����) ���Ѿ���
        do_if('BNlm', 0x364, pTargetUnit, duration, *data1, data2, *data3, data4, data5, *data6, *data7, data8);
        // �˺� �˺����� ���ټ���(%) ���ټ���(%) ��������(%)
        do_if('BNso', 0x364, pTargetUnit, duration, pSrcUnit, data1, data2, data3, data4, data5);
        // BNva
        // ������
        do_if('BPSE', 0x358, pTargetUnit, duration, pSrcUnit);
        // BHfs
        // �˺�ֵ δ֪(0)
        do_if('BHfa', 0x35C, pTargetUnit, duration, data1, *data2);
        // ������λ����ʱ�� �������� δ֪(0)
        do_if('BUfa', 0x354, pTargetUnit, duration, data1, data2, *data3);
        // �Ƿ�Ϊ����
        // do_if('Bfro', 0x35C, pTargetUnit, duration, pSrcUnit, *data1);
    case 'Bfro': // ��������(%) ��������(%)
        base::this_call_vf<void>(pBuff, 0x354, pTargetUnit, duration, pSrcUnit, data1, data2); {
        // ��λ��ɫ
            uint32_t frostCounter = ReadMemory(pTargetUnit + 0x150);
            WriteMemory(pTargetUnit + 0x150, ++frostCounter);
            if (frostCounter == 1)
                base::this_call_vf<void>(pTargetUnit, 0xB4); // ���µ�λ��ɫ
        }
        break;
        // ��λ����(����) δ֪ptr(0)
        do_if('BOhx', 0x358, pTargetUnit, duration, pSrcUnit, *data1, data2);
        // �˺�����(%) �������� �����ָ����� ħ���ָ�����
        do_if('BNht', 0x354, pTargetUnit, duration, data1, data2, data3, data4);
        // ���ٸ��´��� ���ٸ��´��� ��λ���ʱ�� Ӣ�����ʱ��
        do_if('Bprg', 0x354, pTargetUnit, duration, pSrcUnit, data1, data2, data3, data4);
        // Bchd ���ٸ��´��� ���ٸ��´���
        // do_if('Bchd', 0x354, pTargetUnit, duration, pSrcUnit, duration, data1, data2);
        // ������
        do_if('Bhea', 0x354, pTargetUnit, duration, pSrcUnit);
        // �������ָ� ��ħ���ָ� δ֪(1) δ֪(2)
        do_if('Brej', 0x354, pTargetUnit, duration, data1, data2, *data3, *data4);
        // ��ħ���ָ� δ֪(0) ������ɢ(����)
        do_if('BIrm', 0x368, pTargetUnit, duration, data1, *data2, *data3);
        // �������ָ� δ֪(0) ������ɢ(����)
        do_if('BIrl', 0x368, pTargetUnit, duration, data1, *data2, *data3);
        // �������ָ� ��ħ���ָ� δ֪(0) δ֪(0) ������ɢ(����)
        do_if('BIrg', 0x360, pTargetUnit, duration, data1, data2, *data3, *data4, *data5);
        // �Ƿ�Ϊ����(����)
        do_if('Bfre', 0x364, pTargetUnit, duration, pSrcUnit, *data1);
        // BIpi
        // BIpb
        // BIpd
        // ���׼���
        do_if('BIcb', 0x354, pTargetUnit, duration, data1);
        // ������
        do_if('BIrb', 0x31C, pTargetUnit, duration);
        // ����͵ȡֵ(%) �˺�����
        do_if('BIpv', 0x354, pTargetUnit, duration, data1, data2);
        // ������
        do_if('BUcb', 0x368, pTargetUnit, duration, pSrcUnit);
        // �˺�
        do_if('BEia', 0x358, pTargetUnit, duration, data1);
        // ÿ��ħ������ �˺���� Ӱ�췶Χ �˺� Ŀ������(����) ���(����) ����ID?(852177) �󶨼���(ptr)
        do_if('BEim', 0x368, pTargetUnit, duration, data1, data2, data3, data4, *data5, *data6, *data7, *data8);
        // ÿ��ħ������ �˺���� Ӱ�췶Χ �˺� Ŀ������(����) ���(����) ����ID?(852236) �󶨼���(ptr)
        do_if('BNpi', 0x368, pTargetUnit, duration, data1, data2, data3, data4, *data5, *data6, *data7, *data8);
        // ÿ��ħ������ �˺���� Ӱ�췶Χ �˺� Ŀ������(����) ���(����) ����ID?(852567) �󶨼���(ptr)
        do_if('Bpig', 0x368, pTargetUnit, duration, data1, data2, data3, data4, *data5, *data6, *data7, *data8);
        // ÿ��ħ������ �˺���� Ӱ�췶Χ �˺� Ŀ������(����) ���(����) ����ID?(852289) �󶨼���(ptr)
        do_if('BIcf', 0x368, pTargetUnit, duration, data1, data2, data3, data4, *data5, *data6, *data7, *data8);
        // �˺� ����ͣ��ʱ�� 
        do_if('BUim', 0x360, pTargetUnit, pSrcUnit, duration, data1, data2);
        // Ŀ��X Ŀ��Y
        do_if('BNin', 0x364, pTargetUnit, duration, pSrcUnit, data1, data2);
        // ��������(%) �������� �����ָ��ٶ� ħ���ָ��ٶ�
        do_if('Binf', 0x354, pTargetUnit, duration, data1, data2, data3, data4);
        // ����ʱ��
        do_if('Binv', 0x354, pTargetUnit, duration, data1);
        // ������
        do_if('Bvul', 0x31C, pTargetUnit, duration);
        // �˺���� Ӱ�췶Χ �˺� δ֪(2) δ֪(0) ����ID?(852110) �󶨼���(ptr)
        do_if('Blsh', 0x358, pTargetUnit, duration, data1, data2, data3, *data4, *data5, *data6, *data7);
        // Blsa δ֪ptr(0)
        // do_if('Blsa', 0x358, pTargetUnit, duration, data1);
        // δ֪ptr(0, ��Ч, ����ħ���˺�����)
        do_if('Bams', 0x354, pTargetUnit, duration, data1);
        // ��������
        do_if('Bam2', 0x354, pTargetUnit, duration, data1);
        // Bmlt
        // Bmlc
        // Bmfa
        // Ӱ�췶Χ ÿ��ħ������˺�(��λ) ÿ��ħ������˺�(Ӣ��) ����˺�(��λ) ����˺�(Ӣ��) ʩ������ ħ��ʩ��ʱ�� �������˺���ħ����λ(����) Ŀ������(����) ����id(852512) ħ��Ч��(����)
        do_if('Bmfl', 0x360, pTargetUnit, duration, data1, data2, data3, data4, data5, data6, data7, *data8, *data9, *data10, *data11);
        // ÿ��ħ���������˺�ֵ �˺�����(%)
        do_if('BNms', 0x354, pTargetUnit, duration, data1, data2);
        // BEme �󶨼���(ptr)
        // do_if('BEme', 0x358, pTargetUnit, duration, 0);
        // Bmil �󶨼���(ptr)
        //do_if('Bmil', 0x358, pTargetUnit, duration, 0);
        // ʩ���˺�(%) �����˺�(%) ���(����)
        do_if('BOmi', 0x364, pTargetUnit, duration, data1, data2, *data3);
        // ʩ���˺�(%) �����˺�(%) ���(����)
        do_if('BIil', 0x364, pTargetUnit, duration, data1, data2, *data4);
        // ÿ���˺� ���ټ���(%) ���ټ���(%) ��������(����) ��λ����(����) �ٻ���λ����(����) �ٻ���λ����ʱ�� �ٻ���λ�������(����) ħ��Ч��(����)
        do_if('BNpa', 0x388, pTargetUnit, duration, pSrcUnit, data1, data2, data3, *data4, *data5, *data6, data7, *data8, *data9);
        // ������
        do_if('BNpm', 0x31C, pTargetUnit, duration);
        // ������
        do_if('Bpsh', 0x358, pTargetUnit, duration, pSrcUnit);
        // Bpxf
        // Bphx
        // Bssd
        // Bpsi
        // Bpoi
        // Bssi
        // Bspo
        // ��λ����(����)  δ֪ptr(0)
        do_if('Bply', 0x358, pTargetUnit, duration, pSrcUnit, *data1, data2);
        // �ظ��ӳ� ÿ�������ָ� ħ���˺�����
        do_if('BNsa', 0x354, pTargetUnit, duration, data1, data2, data3);
        // Btau
        // ��������(%) ��������(%)
        do_if('BHtc', 0x354, pTargetUnit, duration, pSrcUnit, data1, data2);
        // ��������(%) ��������(%)
        do_if('BCtc', 0x354, pTargetUnit, duration, pSrcUnit, data1, data2);
        // ������λ��� ������λ����(����) ������λ����ʱ�� ħ��Ч��(����) ������λλ�� Լ����Χ
        do_if('BNfy', 0x364, pTargetUnit, duration, data1, *data2, data3, *data4, data5, data6);
        // ����X(����ԴΪNULLʱʹ��) ����Y(����ԴΪNULLʱʹ��) Լ����Χ
        do_if('BNcg', 0x364, pTargetUnit, duration, src_unit == 0 ? NULL : pSrcUnit /* �������ʹ��null */, data1, data2, data3);
        // BNcs δ֪ptr(0) δ֪ptr(0)
        // do_if('BNcs', 0x358, pTargetUnit, duration, data1, data2);
        // ������
        do_if('BNto', 0x31C, pTargetUnit, duration);
        // Btdg
        // Btsp
        // Btsa
        // ÿ���˺� ��������(%)
        do_if('Buhf', 0x378, pTargetUnit, duration, pSrcUnit, data1, data2);
        // ������Դ��(%) ÿ���˺�
        do_if('Buns', 0x354, pTargetUnit, pSrcUnit, data1, data2);
        // ������
        do_if('BOvc', 0x31C, pTargetUnit, duration);
        // ������
        do_if('BOvd', 0x31C, pTargetUnit, duration);
        // ������
        do_if('BOwd', 0x31C, pTargetUnit, duration);
        // Ӱ������ �����ӳ� ���ռ��
        do_if('BImo', 0x364, pTargetUnit, duration, data1, data2, data3);
        // ������
        do_if('BNwm', 0x31C, pTargetUnit, duration);
        // ������
        do_if('Bmec', 0x31C, pTargetUnit, duration);
        // ������
        do_if('BNsg', 0x31C, pTargetUnit, duration);
        // ������
        do_if('BNsq', 0x31C, pTargetUnit, duration);
        // ������
        do_if('BNsw', 0x31C, pTargetUnit, duration);
        // BOww
        // ת��ʱ�� ��������(%) �����˺��ӳ� AOE(��ʲô��?) �󶨼���(����) ����ID(����) �����˺��ӳ�(����)
        do_if('BOwk', 0x358, pTargetUnit, duration, data1, data2, data3, data4, *data5, *data6, data7);
        // Bbof ȫ�˺���ֵ ȫ�˺���� ���˺���ֵ ���˺���� �����˺�����(%)
        // do_if('Bbof', 0x378, pTargetUnit, pSrcUnit, data1, data2, data3, data4, data5);
        // ������
        do_if('Bfrz', 0x364, pTargetUnit, pSrcUnit, duration);
        // ÿ���˺� ���ټ���(%) ���ټ���(%) ��������(����)
        do_if('Bliq', 0x378, pTargetUnit, pSrcUnit, duration, data1, data2, data3, *data4);
        // BNhs
        // ���׼��� �˺���ֵ �˺���� ���ټ���(%) ���ټ���(%)
        do_if('BNab', 0x388, pTargetUnit, pSrcUnit, duration, data1, data2, data3, data4, data5);
        // BNcr
        // BNtm
        // do_if('BNtm', 0x368, pTargetUnit, duration, pSrcUnit, ?, ?, ?, ?);
        // ������
        do_if('BNsl', 0x354, pTargetUnit, duration, pSrcUnit);
        // BHab ���ٷֱȻָ�(����)
        // do_if('BHab', 0x36C, pSrcUnit, pTargetUnit, *data1);
        // BOac ��ս����(����) Զ�̽���(����) ʹ��ָ����ֵ(����)
        // do_if('BOac', 0x36C, pSrcUnit, pTargetUnit, *data1, *data2, *data3);
        // Bakb ��ս����(����) Զ�̽���(����) ʹ��ָ����ֵ(����)
        // do_if('Bakb', 0x36C, pSrcUnit, pTargetUnit, *data1, *data2, *data3);
        // BEar ��ս����(����) Զ�̽���(����) ʹ��ָ����ֵ(����)
        // do_if('BEar', 0x36C, pSrcUnit, pTargetUnit, *data1, *data2, *data3);BHad
        // BHad �ٷֱȽ���(����)
        // do_if('BHad', 0x36C, pSrcUnit, pTargetUnit, *data1);
        // BOae ������
        // do_if('BOae', 0x354, pSrcUnit, pTargetUnit);
        // Basl ������
        // do_if('Basl', 0x354, pSrcUnit, pTargetUnit);
        // Bapl
        // BEah ������
        // do_if('BEah', 0x354, pSrcUnit, pTargetUnit);
        // BUau ���ٷֱȷ���(����)
        // do_if('BUau', 0x36C, pSrcUnit, pTargetUnit, *data1);
        // BUav ������
        // do_if('BUav', 0x354, pSrcUnit, pTargetUnit);
        // BHav
        // ��������(%) ��������(%)
        do_if('BHbn', 0x354, pTargetUnit, duration, pSrcUnit, data1, data2);
        // Bbar
        // ��������(%) ��������(%) �����˺�����(%)
        do_if('Bbsk', 0x35C, pTargetUnit, duration, pSrcUnit, data1, data2, data3);
        // ������
        do_if('BNdm', 0x31C, pTargetUnit, duration);
        // ���ID(����) �ٻ���λ����(����) �ٻ���λ����(����) �ٻ���λ����ʱ�� ħ��Ч��(����)
        do_if('BNba', 0x354, pTargetUnit, duration, pSrcUnit, *data1, *data2, *data3, data4, *data5);
        // BHbd
        // BHbz �˺�ֵ
        // do_if('BHbz', 0x358, pTargetUnit, duration, data1);
        // ÿ���˺� δ֪(1)
        // do_if('BNrd', 0x0, pTargetUnit, duration, pSrcUnit, data1, 1);
    case 'BNrd': // ÿ���˺�
        base::this_call_vf<void>(pBuff, 0x354, pTargetUnit, duration, pSrcUnit, data1);
        // if (1)
        WriteMemory(pBuff + 0x20, ReadMemory(pBuff + 0x20) | 0x100);
        break;
        // BNrf
        // ��������(%) ��������(%) ģ�ͷŴ����(%)
        do_if('Bblo', 0x35C, pTargetUnit, duration, pSrcUnit, data1, data2, data3);
        // ��������(%) ��������(%) ģ�ͷŴ����(%, ������ѡ��Ȧ)
        do_if('Bfzy', 0x35C, pTargetUnit, duration, pSrcUnit, data1, data2, data3);
        // ÿ���˺� δ֪(1)
        // do_if('BNbf', 0x0, pTargetUnit, pSrcUnit, duration, data1, 1);
    case 'BNbf': // ÿ���˺�
        base::this_call_vf<void>(pBuff, 0x354, pTargetUnit, duration, pSrcUnit, data1);
        // if (1)
        WriteMemory(pBuff + 0x20, ReadMemory(pBuff + 0x20) | 0x100);
        break;
        // ÿ���˺� δ֪(1)
        // do_if('BCbf', 0x0, pTargetUnit, pSrcUnit, duration, data1, 1);
    case 'BCbf': // ÿ���˺�
        base::this_call_vf<void>(pBuff, 0x354, pTargetUnit, duration, pSrcUnit, data1);
        // if (1)
        WriteMemory(pBuff + 0x20, ReadMemory(pBuff + 0x20) | 0x100);
        break;
        // Ŀ���޵�(����) Ŀ��ħ������(����)
        do_if('Bpos', 0x368, pTargetUnit, duration, pSrcUnit, *data1, *data2);
        // ������
        do_if('Bpoc', 0x31C, pTargetUnit, duration);
        // ������
        do_if('Bcmg', 0x31C, pTargetUnit, duration);
        // ��������(%) ��������(%) ��ֹ����(����) ʧ�����(%)
        do_if('Bclf', 0x35C, pTargetUnit, duration, pSrcUnit, data1, data2, *data3, data4);
        // Bcsd
        // Bcsi
        // δ֪ptr(0) ���ټ���(%) ���ټ���(%) ���Ӳ���
        do_if('BHca', 0x378, pTargetUnit, pSrcUnit, duration, data1, data2, data3, *data4);
        // ���ټ���(%) ���ټ���(%) �˺�����(%)
        do_if('Bcri', 0x35C, pTargetUnit, duration, pSrcUnit, data1, data2, data3);
        // ������
        do_if('Bcy2', 0x354, pTargetUnit, pSrcUnit, duration);
        // ������
        do_if('Bcyc', 0x354, pTargetUnit, pSrcUnit, duration);
        // Brpl
        // do_if('Brpl', 0x0, pTargetUnit, pSrcUnit, duration);
        // Brpm
        // do_if('Brpm', 0x0, pTargetUnit, pSrcUnit, duration);
        // ��������(%) �������� �����ָ��ٶ� ħ������
        do_if('Broa', 0x354, pTargetUnit, duration, data1, data2, data3, data4);
        // �������� �������� �����ָ��ٶ� ħ������
        do_if('BNbr', 0x354, pTargetUnit, duration, data1, data2, data3, data4);
        // ������
        do_if('BEst', 0x31C, pTargetUnit, duration);
        // �����˺� �˺���� ���ټ���(%) ���ټ���(%) �ٶ�˥������
        do_if('BEsh', 0x388, pTargetUnit, pSrcUnit, duration, data1, data2, data3, data4, data5);
        // BUcs
        // ��������(%) ��������(%) ��ֹ����(����) ʧ����(%)
        do_if('BNsi', 0x35C, pTargetUnit, duration, pSrcUnit, data1, data2, *data3, data4);
        // ������
        do_if('BUsl', 0x354, pTargetUnit, duration, pSrcUnit);
        // ������
        do_if('BUsp', 0x358, pTargetUnit, duration, pSrcUnit);
        // ��λ����(����) // ʱ��̶�6��
        do_if('BNdc', 0x35C, pTargetUnit, duration, pSrcUnit, *data1);
        // ��������(%) ��������(%)
        do_if('Bslo', 0x354, pTargetUnit, duration, pSrcUnit, data1, data2);
        // BIsv ������
        // do_if('BIsv', 0x31C, pTargetUnit, duration);
        // ��������(%) ��������(%)
        do_if('Bspe', 0x354, pTargetUnit, duration, pSrcUnit, data1, data2);
        // ������
        do_if('BNss', 0x31C, pTargetUnit, duration);
        // �ֲ��˺�����(%)
        do_if('Bspl', 0x354, pTargetUnit, duration, data1);
        // ������
        do_if('BEsv', 0x31C, pTargetUnit, duration);
        // ������
        do_if('Bvng', 0x368, pTargetUnit, duration, pSrcUnit);
        // ������
        do_if('BOsf', 0x364, pTargetUnit, duration, pSrcUnit);
        // ������
        do_if('BIsh', 0x364, pTargetUnit, duration, pSrcUnit);
        // �����ӳ� ��鷶Χ δ֪ptr(0) ��ը��Χ ѣ�γ���ʱ��(��λ) ѣ�γ���ʱ��(Ӣ��)
        do_if('Bstt', 0x364, pTargetUnit, duration, data1, data2, data3, data4, data5, data6);
        // Bspa
        // Bprv
        // BTAR
    default:
        return false;
    }
#undef do_if
    CUnit_AddBuff(pTargetUnit, pBuff);
    return true;
}

init(Buff) {
    jass::japi_add((uint32_t)X_UnitAddBuff, "X_UnitAddBuff", "(Hunit;Hunit;IIIIRRRRRRRRRRRR)B");
}