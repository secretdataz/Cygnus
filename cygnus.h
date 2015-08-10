/**
 * Cygnus.h, simple C++ header file for changing a single executable's memory.
 *
 * @author Jittapan Pluemsumran
 * @version 0.2 7/3/2015
 */

#include <Windows.h>
#include <TlHelp32.h>

#ifndef CYGNUS
#define CYGNUS

#endif // CYGNUS

wchar_t* EXE = L"trgame.exe";

void GetProcId(wchar_t* ProcName);

DWORD ProcId = 0;

/**
 * Get process' PID and store in ProcId variable.
 *
 * @param ProcName Process' executable name.
 */
void GetProcId(wchar_t* ProcName)
{
    PROCESSENTRY32   pe32;
    HANDLE         hSnapshot = NULL;

    pe32.dwSize = sizeof( PROCESSENTRY32 );
    hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

    if( Process32First( hSnapshot, &pe32 ) )
    {
        do{
            if( lstrcmp( pe32.szExeFile, ProcName ) == 0 )
                break;
        }while( Process32Next( hSnapshot, &pe32 ) );
    }

    if( hSnapshot != INVALID_HANDLE_VALUE )
        CloseHandle( hSnapshot );

    ProcId = pe32.th32ProcessID;
}

HANDLE hProcess;

/**
 * @brief Get process' handle
 * @param procName Process name
 */
void Attach(wchar_t* procName)
{
    GetProcId(procName);
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ProcId);
}

/**
 * @param lpBaseAddr Memory address to write
 * @param buf Buffer to write
 */
void Write(LPVOID lpBaseAddr, LPCVOID *buf)
{
    WriteProcessMemory(hProcess, lpBaseAddr, buf, sizeof(*buf), NULL);
}

/*
 * ===========================
 * Process suspend and resume.
 * ===========================
 */

/**
 * @brief suspend
 * Suspends Process stored in ProcId variable
 */
void suspend()
{
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);

    Thread32First(hThreadSnapshot, &threadEntry);

    do
    {
        if (threadEntry.th32OwnerProcessID == ProcId)
        {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE,
                threadEntry.th32ThreadID);

            SuspendThread(hThread);
            CloseHandle(hThread);
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));

    CloseHandle(hThreadSnapshot);
}

/**
 * @brief resume
 * Resumes Process stored in ProcId variable
 */
void resume()
{
    HANDLE hThreadSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);

    Thread32First(hThreadSnapshot, &threadEntry);

    do
    {
        if (threadEntry.th32OwnerProcessID == ProcId)
        {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE,
                threadEntry.th32ThreadID);

            ResumeThread(hThread);
            CloseHandle(hThread);
        }
    } while (Thread32Next(hThreadSnapshot, &threadEntry));

    CloseHandle(hThreadSnapshot);
}
typedef LONG (NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);
typedef LONG (NTAPI *NtResumeProcess) (IN HANDLE ProcessHandle);

/*
 * Use at risk.
 */
void NtSuspend()
{
    HANDLE processHandle = hProcess;
    NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(
        GetModuleHandle(L"ntdll"), "NtSuspendProcess");

    pfnNtSuspendProcess(processHandle);
    CloseHandle(processHandle);
}

void NtResume()
{
    HANDLE processHandle = hProcess;
    NtResumeProcess pfnNtResumeProcess = (NtResumeProcess)GetProcAddress(
        GetModuleHandle(L"ntdll"), "NtResumeProcess");

    pfnNtResumeProcess(processHandle);
    CloseHandle(processHandle);
}

DWORD FindPtrAddress(int PointerLevel, DWORD Offsets[], DWORD BaseAddress)
{
    DWORD pointer = BaseAddress;
    DWORD tmp;
    DWORD pointerAddr;

    for (int i = 0; i < PointerLevel; i++)
    {
        if (i == 0)
        {
            ReadProcessMemory(hProcess, (LPCVOID)pointer, &tmp, sizeof(tmp), NULL);
        }
        pointerAddr = tmp + Offsets[i];
        ReadProcessMemory(hProcess, (LPCVOID)pointerAddr, &tmp, sizeof(tmp), NULL);
    }
    return pointerAddr;
}
