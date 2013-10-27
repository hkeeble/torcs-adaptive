#include <tgf.h>

#ifdef _WIN32
BOOL WINAPI DllEntryPoint (HINSTANCE hDLL, DWORD dwReason, LPVOID Reserved)
{
    return TRUE;
}
#endif // _WIN32

static int winTel(int /* index */, void *pt)
{


	return 0;
}

extern "C" int winTel(tModInfo* modInfo)
{
	modInfo->name = strdup("WinTelemetry");
	modInfo->desc = strdup("Telemetry Functions for TORCS-Adaptive in Windows.");
}