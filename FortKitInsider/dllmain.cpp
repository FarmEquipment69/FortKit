#include "consts.h"
#include "dumper.h"
#include "framework.h"
#include "memory.h"
#include "util.h"
#include "memcury.h"

static void Main(HMODULE hModule)
{
    auto Start = std::chrono::steady_clock::now();

    Util::SetupConsole();

    auto GObjectsAdd = Memcury::Scanner::FindPattern(Patterns::GObjects)
                           .RelativeOffset(3)
                           .GetAs<void*>();
    if (!GObjectsAdd)
    {
        MessageBoxW(nullptr, L"Cannot find GObjects.", L"Error", MB_OK);
        return;
    }

    GObjects = decltype(GObjects)(GObjectsAdd);

    auto FNameToStringAdd = Memcury::Scanner::FindStringRef(StringRefs::FNameToString)
                                .ScanFor({ Memcury::ASM::CALL }, false)
                                .RelativeOffset(1)
                                .GetAs<void*>();
    if (!FNameToStringAdd)
    {
        MessageBoxW(nullptr, L"Cannot find FNameToString.", L"Error", MB_OK);
        return;
    }

    FNameToString = decltype(FNameToString)(FNameToStringAdd);

    auto End = std::chrono::steady_clock::now();

    printf("[=] Init Time: %.02f ms\n", (End - Start).count() / 1000000.);

    Dumper::Dump();
    // Dumper::GenerateUsmap();

    FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&Main), hModule, 0, nullptr);

        break;
    }
    default:
        break;
    }
    return TRUE;
}
