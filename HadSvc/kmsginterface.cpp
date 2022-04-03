#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>
#include <map>

#include "kmsginterface.h"
#include "sysinfo.h"
#include "msgassist.h"

#include "ArkSsdt.h"
#include "ArkIdt.h"
#include "ArkDpcTimer.h"
#include "ArkFsd.h"
#include "ArkMouseKeyBoard.h"
#include "ArkNetwork.h"
#include "ArkProcessInfo.h"
#include "AkrSysDriverDevInfo.h"

//rapidjson
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

//nlohmannjson
#include <json.hpp>
using json_t = nlohmann::json;

static ArkSsdt		        g_grpc_ssdtobj;
static ArkIdt				g_grpc_idtobj;
static ArkDpcTimer		    g_grpc_dpcobj;
static ArkFsd				g_grpc_fsdobj;
static ArkMouseKeyBoard	    g_grpc_mousekeyboardobj;
static ArkNetwork			g_grpc_networkobj;
static ArkProcessInfo		g_grpc_processinfo;
static AkrSysDriverDevInfo	g_grpc_sysmodinfo;

void kMsgInterface::kMsg_taskPush(const int taskcode, std::vector<std::string>& vec_task_string)
{
    map<int, wstring>::iterator iter;
    map<int, wstring> Process_list;
    std::string tmpstr; wstring catstr;
    int i = 0, index = 0;
    DWORD dwAllocateMemSize = 0;
    char* ptr_Getbuffer;
    bool nstatus = Choose_mem(ptr_Getbuffer, dwAllocateMemSize, taskcode);
    if (false == nstatus || nullptr == ptr_Getbuffer || dwAllocateMemSize == 0)
        return;

    json_t j;
    switch (taskcode)
    {
    case NF_SSDT_ID:
    {
        if (g_grpc_ssdtobj.nf_init())
        {
            if (false == g_grpc_ssdtobj.nf_GetSysCurrentSsdtData((LPVOID)ptr_Getbuffer, dwAllocateMemSize))
                break;
            SSDTINFO* ssdtinfo = (SSDTINFO*)ptr_Getbuffer;
            if (!ssdtinfo)
                break;

            for (i = 0; i < 0x200; ++i)
            {
                if (!ssdtinfo[i].sstd_memoffset)
                    continue;
                j["win_rootkit_ssdt_id"] = to_string(ssdtinfo[i].ssdt_id).c_str();
                j["win_rootkit_ssdt_offsetaddr"] = to_string(ssdtinfo[i].sstd_memoffset).c_str();
                vec_task_string.push_back(j.dump());
            }
            std::cout << "Grpc Ssdt Send Pkg Success" << std::endl;
            break;
        }
    }
    break;
    case NF_IDT_ID:
    {
        if (g_grpc_idtobj.nf_init())
        {
            if (!g_grpc_idtobj.nf_GetIdtData((LPVOID)ptr_Getbuffer, dwAllocateMemSize))
                break;
            IDTINFO* idtinfo = (IDTINFO*)ptr_Getbuffer;
            if (!idtinfo)
                break;

            for (i = 0; i < 0x100; ++i)
            {
                if (!idtinfo[i].idt_isrmemaddr)
                    continue;
                j["win_rootkit_idt_id"] = to_string(idtinfo[i].idt_id).c_str();
                j["win_rootkit_idt_offsetaddr"] = to_string(idtinfo[i].idt_isrmemaddr).c_str();
                vec_task_string.push_back(j.dump());
            }
            std::cout << "Grpc Ssdt Send Pkg Success" << std::endl;
        }
    }
    break;
    case NF_DPC_ID:
    {
        if (false == g_grpc_dpcobj.nf_GetDpcTimerData((LPVOID)ptr_Getbuffer, dwAllocateMemSize))
            break;
        DPC_TIMERINFO* dpcinfo = (DPC_TIMERINFO*)ptr_Getbuffer;
        if (!dpcinfo)
            break;

        for (i = 0; i < 0x100; ++i)
        {
            if (!dpcinfo[i].dpc)
                continue;
            j["win_rootkit_dpc"] = to_string(dpcinfo[i].dpc).c_str();
            j["win_rootkit_dpc_timeobj"] = to_string(dpcinfo[i].timeroutine).c_str();
            j["win_rootkit_dpc_timeroutine"] = to_string(dpcinfo[i].timeroutine).c_str();
            j["win_rootkit_dpc_periodtime"] = to_string(dpcinfo[i].period).c_str();
            vec_task_string.push_back(j.dump());
        }
        std::cout << "Grpc Dpc Send Pkg Success" << std::endl;
    }
    break;
    case NF_FSD_ID:
    {
        if (false == g_grpc_fsdobj.nf_GetFsdInfo(ptr_Getbuffer, dwAllocateMemSize))
            break;

        ULONGLONG* MjAddrArry = (ULONGLONG*)ptr_Getbuffer;
        if (!MjAddrArry)
            break;

        j["win_rootkit_is_fsdmod"] = "1";
        for (i = 0; i < 0x1b; ++i)
        {
            j["win_rootkit_fsdfastfat_id"] = to_string(MjAddrArry[index]).c_str();
            j["win_rootkit_fsdfastfat_mjaddr"] = to_string(MjAddrArry[index]).c_str();
            vec_task_string.push_back(j.dump());
            index++;
        }
        std::cout << "FastFat MjFuction End" << std::endl;

        j.clear();
        j["win_rootkit_is_fsdmod"] = "2";
        for (i = 0; i < 0x1b; ++i)
        {
            j["win_rootkit_fsdntfs_id"] = to_string(MjAddrArry[index]).c_str();
            j["win_rootkit_fsdntfs_mjaddr"] = to_string(MjAddrArry[index]).c_str();
            vec_task_string.push_back(j.dump());
            index++;
        }
        std::cout << "Ntfs MjFuction End" << std::endl;
    }
    break;
    case NF_SYSCALLBACK_ID:
    {
    }
    break;
    case NF_MOUSEKEYBOARD_ID:
    {
        if (false == g_grpc_mousekeyboardobj.nf_GetMouseKeyInfoData(ptr_Getbuffer, dwAllocateMemSize))
            break;

        ULONGLONG* MjAddrArry = (ULONGLONG*)ptr_Getbuffer;
        if (!MjAddrArry)
            break;

        j["win_rootkit_is_mousekeymod"] = "1";
        for (i = 0; i < 0x1b; ++i)
        {
            j["win_rootkit_Mouse_id"] = to_string(MjAddrArry[index]).c_str();
            j["win_rootkit_Mouse_mjaddr"] = to_string(MjAddrArry[index]).c_str();
            vec_task_string.push_back(j.dump());
            index++;
        }
        std::cout << "Mouse MjFuction End" << std::endl;

        j["win_rootkit_is_mousekeymod"] = "2";
        for (i = 0; i < 0x1b; ++i)
        {
            j["win_rootkit_i8042_id"] = to_string(MjAddrArry[index]).c_str();
            j["win_rootkit_i8042_mjaddr"] = to_string(MjAddrArry[index]).c_str();
            vec_task_string.push_back(j.dump());
            index++;
        }
        std::cout << "i8042 MjFuction End" << std::endl;

        j["win_rootkit_is_mousekeymod"] = "3";
        for (i = 0; i < 0x1b; ++i)
        {
            j["win_rootkit_kbd_id"] = to_string(MjAddrArry[index]).c_str();
            j["win_rootkit_kbd_mjaddr"] = to_string(MjAddrArry[index]).c_str();
            vec_task_string.push_back(j.dump());
            index++;
        }
        std::cout << "kbd MjFuction End" << std::endl;
    }
    break;
    case NF_NETWORK_ID:
    {
        if (false == g_grpc_networkobj.nf_GetNteworkProcessInfo(ptr_Getbuffer, dwAllocateMemSize))
            break;

        PSYSNETWORKINFONODE networkinfo = (PSYSNETWORKINFONODE)ptr_Getbuffer;
        if (!networkinfo)
            break;

        
        // Tcp
        j["win_rootkit_is_mod"] = "1";
        for (i = 0; i < networkinfo->tcpcout; ++i)
        {
            j["win_rootkit_tcp_pid"] = to_string(networkinfo->systcpinfo[i].processinfo.dwTcpProId).c_str();
            j["win_rootkit_tcp_localIp_port"] = to_string(networkinfo->systcpinfo[i].TpcTable.localEntry.dwIP).c_str();
            j["win_rootkit_tcp_remoteIp_port"] = to_string(networkinfo->systcpinfo[i].TpcTable.remoteEntry.dwIP).c_str();
            j["win_rootkit_tcp_Status"] = to_string(networkinfo->systcpinfo[i].socketStatus.dwState).c_str();
            vec_task_string.push_back(j.dump());
        }
        std::cout << "Tpc Port Send Grpc Success" << std::endl;

        j.clear();
        j["win_rootkit_is_mod"] = "2";
        std::string udpipport;
        for (i = 0; i < networkinfo->udpcout; ++i)
        {
            udpipport = to_string(networkinfo->sysudpinfo[i].UdpTable.dwIP) + ":" + to_string(ntohs(networkinfo->sysudpinfo[i].UdpTable.Port));
            j["win_rootkit_udp_pid"] = to_string(networkinfo->sysudpinfo[i].processinfo.dwUdpProId).c_str();
            j["win_rootkit_udp_localIp_port"] = udpipport.c_str();
            vec_task_string.push_back(j.dump());
        }
        std::cout << "Udp Port Send Grpc Success" << std::endl;
    }
    break;
    case NF_PROCESS_ENUM:
    {
        if (false == g_grpc_processinfo.nf_EnumProcess(ptr_Getbuffer, dwAllocateMemSize))
            break;

        PHANDLE_INFO phandleinfo = (PHANDLE_INFO)ptr_Getbuffer;
        if (phandleinfo && phandleinfo[0].CountNum)
        {

            for (i = 0; i < phandleinfo[0].CountNum; ++i)
            {
                wcout << "Pid: " << phandleinfo[i].ProcessId << " - Process: " << phandleinfo[i].ProcessPath << endl;// " - ProcessName: " << phandleinfo[i].ProcessName << endl;
                // ȥ��
                catstr = phandleinfo[i].ProcessPath;
                catstr += L" - ";
                catstr += phandleinfo[i].ProcessName;
                Process_list[phandleinfo[i].ProcessId] = catstr;
                catstr.clear();
            }


            for (iter = Process_list.begin(); iter != Process_list.end(); iter++)
            {
                j["win_rootkit_process_pid"] = to_string(iter->first).c_str();
                tmpstr.clear();
                Wchar_tToString(tmpstr, (wchar_t*)iter->second.data());
                tmpstr = String_ToUtf8(tmpstr);
                j["win_rootkit_process_info"] = tmpstr.c_str();
                vec_task_string.push_back(j.dump());
            }

            std::cout << "processinfo to server Success" << std::endl;
        }
    }
    break;
    case NF_PROCESS_MOD:
    {
        int Process_Pid = 4;
         cout << "Please Input Pid: ";
         scanf("%d", &Process_Pid);
        // Ĭ�ϲ���
        if (false == g_grpc_processinfo.nf_GetProcessMod(Process_Pid, ptr_Getbuffer, dwAllocateMemSize))
            break;

        PPROCESS_MOD modptr = (PPROCESS_MOD)ptr_Getbuffer;
        if (modptr)
        {
            j["win_rootkit_processmod_pid"] = to_string(Process_Pid).c_str();
            for (i = 0; i < 1024 * 2; ++i)
            {
                if (0 == modptr[i].EntryPoint && 0 == modptr[i].SizeOfImage && 0 == modptr[i].DllBase)
                    continue;
                j["win_rootkit_process_DllBase"] = to_string(modptr[i].DllBase).c_str();
                j["win_rootkit_process_SizeofImage"] = to_string(modptr[i].SizeOfImage).c_str();
                j["win_rootkit_process_EntryPoint"] = to_string(modptr[i].EntryPoint).c_str();
                tmpstr.clear();
                Wchar_tToString(tmpstr, modptr[i].BaseDllName);
                tmpstr = String_ToUtf8(tmpstr);
                j["win_rootkit_process_BaseDllName"] = tmpstr.c_str();
                tmpstr.clear();
                Wchar_tToString(tmpstr, modptr[i].FullDllName);
                tmpstr = String_ToUtf8(tmpstr);
                j["win_rootkit_process_FullDllName"] = tmpstr.c_str();
                vec_task_string.push_back(j.dump());
            }
        }
        std::cout << "Process Mod Success" << std::endl;
    }
    break;
    case NF_PROCESS_KILL:
    {
         g_grpc_processinfo.nf_KillProcess();
    }
    break;
    case NF_SYSMOD_ENUM:
    {
        if (false == g_grpc_sysmodinfo.nf_EnumSysMod(ptr_Getbuffer, dwAllocateMemSize))
            break;


        PPROCESS_MOD modptr = (PPROCESS_MOD)ptr_Getbuffer;
        if (modptr)
        {
            for (i = 0; i < 1024 * 2; ++i)
            {
                // Bug
                if (0 == modptr[i].EntryPoint && 0 == modptr[i].SizeOfImage && 0 == modptr[i].DllBase)
                    continue;
                j["win_rootkit_sys_DllBase"] = to_string(modptr[i].DllBase).c_str();
                j["win_rootkit_sys_SizeofImage"] = to_string(modptr[i].SizeOfImage).c_str();
                j["win_rootkit_sys_EntryPoint"] = to_string(modptr[i].EntryPoint).c_str();
                tmpstr.clear();
                Wchar_tToString(tmpstr, modptr[i].BaseDllName);
                tmpstr = String_ToUtf8(tmpstr);
                j["win_rootkit_sys_BaseDllName"] = tmpstr.c_str();
                tmpstr.clear();
                Wchar_tToString(tmpstr, modptr[i].FullDllName);
                tmpstr = String_ToUtf8(tmpstr);
                j["win_rootkit_sys_FullDllName"] = tmpstr.c_str();
                vec_task_string.push_back(j.dump());
            }
        }
        std::cout << "SystemDriver Enum Success" << std::endl;
    }
    break;

    default:
        break;
    }
}