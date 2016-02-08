// ME3 - binkw32 - main.cpp
#include "header.h"
#include <windows.h>
#include <stdio.h>

HINSTANCE hOriginalBink = NULL;
FARPROC p[72] = {0};
BYTE pattern [] = {   0x8B, 0x11, 0x8B, 0x42, 0x0C,
                        0x57, 0x56, 0xFF, 0xD0, 
                        0x8B, 0xC3, // <-- move eax,ebx; offset 0x9; will be replaced with 0xB0 0x01 to get mov al,1;
                        0x8B, 0x4D, 0xF4, 0x64, 
                        0x89, 0x0D, 0x00, 0x00, 0x00, 
                        0x00, 0x59, 0x5F, 0x5E, 0x5B, 
                        0x8B, 0xE5, 0x5D, 0xC2, 0x08, 
                        0x00, 0xCC, 0xCC, 0xCC, 0x8B, 
                        0x41, 0x04, 0x56, 0x85, 0xC0 };
BYTE pattern2 [] = { 
                        0x8B, 0x45, 0x0C,                       // mov     eax, [ebp+arg_4]
						0xC7, 0x00, 0x01, 0x00, 0x00, 0x00,     // mov     dword ptr [eax], 1
						0x5D,                                   // pop     ebp
						0xC2, 0x08, 0x00,                       // retn    8
						0x8B, 0x4D, 0x0C,                       // mov     ecx, [ebp+arg_4]
						0xC7, 0x01, 0x01, 0x00, 0x00, 0x00,     // mov     dword ptr [ecx], 1
						0x5D,                                   // pop     ebp
						0xC2, 0x08, 0x00,                       // retn    8
						0xCC, 0xCC, 0xCC, 0xCC, 0xCC
};
BYTE pattern3 [] = { 0xB8, 0xE4, 0xFF, 0xFF, 0xFF, 0x5B, 0x59, 0xC3 };

// Returns current executable's path, including "\"
char *GetExecutableFolder()
{
	char buffer[FILENAME_MAX];
	GetModuleFileName (NULL, buffer, FILENAME_MAX);
	int x = strlen(buffer) - 1;
	while (buffer[x] != '\\')
		x--;
	buffer[x + 1] = '\0';
	return buffer;
}

// --- Load Plugins ---
void loadPlugins (FILE *Log, char *folder)
{
	DWORD typeMask = 0x6973612e; // '.asi'
	WIN32_FIND_DATA fd;
	char targetfilter[FILENAME_MAX];
	char currfile[FILENAME_MAX];
	char exebasefolder[FILENAME_MAX];
	strcpy_s (exebasefolder, GetExecutableFolder());
	strcpy_s (targetfilter, exebasefolder);
	strcat_s (targetfilter, folder);
	strcat_s (targetfilter, "\\*.asi");
	HANDLE asiFile = FindFirstFile (targetfilter, &fd);
	if (asiFile == INVALID_HANDLE_VALUE)
		return;
	do
	{
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			int pos = 0;
			while (fd.cFileName[pos])
				pos++;
			DWORD type = *(DWORD *)(fd.cFileName+pos-4);
			type |= 0x20202020; // convert letter to lowercase, "\0" to space
			if (type == typeMask)
			{
				strcpy_s (currfile, exebasefolder);
				strcat_s (currfile, folder);
				strcat_s (currfile, "\\");
				strcat_s (currfile, fd.cFileName);
				if (LoadLibrary (currfile)) 
					fprintf (Log, "Plugin loaded: %s\n", currfile);
				else
					fprintf (Log, "Plugin error: %s\n", currfile);
			}
		}
	} while (FindNextFile (asiFile, &fd));
	FindClose (asiFile);
}

bool DataCompare(const BYTE* OpCodes, const BYTE* Mask, const char* StrMask)  
{  
	while (*StrMask)  
	{  
		if(*StrMask == 'x' && *OpCodes != *Mask )  
			return false;  
		++StrMask;  
		++OpCodes;  
		++Mask;  
	}  
	return true;
} 

DWORD FindPattern(DWORD StartAddress, DWORD CodeLen, BYTE* Mask, char* StrMask, unsigned short ignore)  
{  
	unsigned short Ign = 0;  
	DWORD i = 0;  
	while (Ign <= ignore)  
	{  
		if(DataCompare((BYTE*)(StartAddress + i++), Mask,StrMask)) 
			++Ign;  
		else if (i>=CodeLen)  
			return 0;  
	} 
	return StartAddress + i - 1;  
} 

void GetAddresses()
{
		hOriginalBink = LoadLibrary("binkw23.dll");
		if (hOriginalBink)
		{
			p[0] = GetProcAddress(hOriginalBink,"_BinkBufferBlit@12");
			p[1] = GetProcAddress(hOriginalBink,"_BinkBufferCheckWinPos@12");
			p[2] = GetProcAddress(hOriginalBink,"_BinkBufferClear@8");
			p[3] = GetProcAddress(hOriginalBink,"_BinkBufferClose@4");
			p[4] = GetProcAddress(hOriginalBink,"_BinkBufferGetDescription@4");
			p[5] = GetProcAddress(hOriginalBink,"_BinkBufferGetError@0");
			p[6] = GetProcAddress(hOriginalBink,"_BinkBufferLock@4");
			p[7] = GetProcAddress(hOriginalBink,"_BinkBufferOpen@16");
			p[8] = GetProcAddress(hOriginalBink,"_BinkBufferSetDirectDraw@8");
			p[9] = GetProcAddress(hOriginalBink,"_BinkBufferSetHWND@8");
			p[10] = GetProcAddress(hOriginalBink,"_BinkBufferSetOffset@12");
			p[11] = GetProcAddress(hOriginalBink,"_BinkBufferSetResolution@12");
			p[12] = GetProcAddress(hOriginalBink,"_BinkBufferSetScale@12");
			p[13] = GetProcAddress(hOriginalBink,"_BinkBufferUnlock@4");
			p[14] = GetProcAddress(hOriginalBink,"_BinkCheckCursor@20");
			p[15] = GetProcAddress(hOriginalBink,"_BinkClose@4");
			p[16] = GetProcAddress(hOriginalBink,"_BinkCloseTrack@4");
			p[17] = GetProcAddress(hOriginalBink,"_BinkControlBackgroundIO@8");
			p[18] = GetProcAddress(hOriginalBink,"_BinkControlPlatformFeatures@8");
			p[19] = GetProcAddress(hOriginalBink,"_BinkCopyToBuffer@28");
			p[20] = GetProcAddress(hOriginalBink,"_BinkCopyToBufferRect@44");
			p[21] = GetProcAddress(hOriginalBink,"_BinkDDSurfaceType@4");
			p[22] = GetProcAddress(hOriginalBink,"_BinkDX8SurfaceType@4");
			p[23] = GetProcAddress(hOriginalBink,"_BinkDX9SurfaceType@4");
			p[24] = GetProcAddress(hOriginalBink,"_BinkDoFrame@4");
			p[25] = GetProcAddress(hOriginalBink,"_BinkDoFrameAsync@12");
			p[26] = GetProcAddress(hOriginalBink,"_BinkDoFrameAsyncWait@8");
			p[27] = GetProcAddress(hOriginalBink,"_BinkDoFramePlane@8");
			p[28] = GetProcAddress(hOriginalBink,"_BinkGetError@0");
			p[29] = GetProcAddress(hOriginalBink,"_BinkGetFrameBuffersInfo@8");
			p[30] = GetProcAddress(hOriginalBink,"_BinkGetKeyFrame@12");
			p[31] = GetProcAddress(hOriginalBink,"_BinkGetPalette@4");
			p[32] = GetProcAddress(hOriginalBink,"_BinkGetRealtime@12");
			p[33] = GetProcAddress(hOriginalBink,"_BinkGetRects@8");
			p[34] = GetProcAddress(hOriginalBink,"_BinkGetSummary@8");
			p[35] = GetProcAddress(hOriginalBink,"_BinkGetTrackData@8");
			p[36] = GetProcAddress(hOriginalBink,"_BinkGetTrackID@8");
			p[37] = GetProcAddress(hOriginalBink,"_BinkGetTrackMaxSize@8");
			p[38] = GetProcAddress(hOriginalBink,"_BinkGetTrackType@8");
			p[39] = GetProcAddress(hOriginalBink,"_BinkGoto@12");
			p[40] = GetProcAddress(hOriginalBink,"_BinkIsSoftwareCursor@8");
			p[41] = GetProcAddress(hOriginalBink,"_BinkLogoAddress@0");
			p[42] = GetProcAddress(hOriginalBink,"_BinkNextFrame@4");
			p[43] = GetProcAddress(hOriginalBink,"_BinkOpen@8");
			p[44] = GetProcAddress(hOriginalBink,"_BinkOpenDirectSound@4");
			p[45] = GetProcAddress(hOriginalBink,"_BinkOpenMiles@4");
			p[46] = GetProcAddress(hOriginalBink,"_BinkOpenTrack@8");
			p[47] = GetProcAddress(hOriginalBink,"_BinkOpenWaveOut@4");
			p[48] = GetProcAddress(hOriginalBink,"_BinkPause@8");
			p[49] = GetProcAddress(hOriginalBink,"_BinkRegisterFrameBuffers@8");
			p[50] = GetProcAddress(hOriginalBink,"_BinkRequestStopAsyncThread@4");
			p[51] = GetProcAddress(hOriginalBink,"_BinkRestoreCursor@4");
			p[52] = GetProcAddress(hOriginalBink,"_BinkService@4");
			p[53] = GetProcAddress(hOriginalBink,"_BinkSetError@4");
			p[54] = GetProcAddress(hOriginalBink,"_BinkSetFrameRate@8");
			p[55] = GetProcAddress(hOriginalBink,"_BinkSetIO@4");
			p[56] = GetProcAddress(hOriginalBink,"_BinkSetIOSize@4");
			p[57] = GetProcAddress(hOriginalBink,"_BinkSetMemory@8");
			p[58] = GetProcAddress(hOriginalBink,"_BinkSetMixBinVolumes@20");
			p[59] = GetProcAddress(hOriginalBink,"_BinkSetMixBins@16");
			p[60] = GetProcAddress(hOriginalBink,"_BinkSetPan@12");
			p[61] = GetProcAddress(hOriginalBink,"_BinkSetSimulate@4");
			p[62] = GetProcAddress(hOriginalBink,"_BinkSetSoundOnOff@8");
			p[63] = GetProcAddress(hOriginalBink,"_BinkSetSoundSystem@8");
			p[64] = GetProcAddress(hOriginalBink,"_BinkSetSoundTrack@8");
			p[65] = GetProcAddress(hOriginalBink,"_BinkSetVideoOnOff@8");
			p[66] = GetProcAddress(hOriginalBink,"_BinkSetVolume@12");
			p[67] = GetProcAddress(hOriginalBink,"_BinkShouldSkip@4");
			p[68] = GetProcAddress(hOriginalBink,"_BinkStartAsyncThread@8");
			p[69] = GetProcAddress(hOriginalBink,"_BinkWait@4");
			p[70] = GetProcAddress(hOriginalBink,"_BinkWaitStopAsyncThread@4");
			p[71] = GetProcAddress(hOriginalBink,"_RADTimerRead@0");
		}
}

DWORD WINAPI Start(LPVOID lpParam)
{
		FILE* Log;
		fopen_s ( &Log, "binkw32log.txt", "w" );
		fprintf(Log, "ME3 Autopatcher by Warranty Voider\n");
		fprintf(Log, "Enhancements by Erik JS\n");
		GetAddresses();
		if (hOriginalBink)
		{
			fprintf(Log, "binkw23.dll - loaded\n");
		}
		else
		{
			fprintf(Log, "Error loading binkw23.dll!\n");
			return 0;
		}
		DWORD patch1, patch2, patch3;
		DWORD dwProtect;
		patch1 = FindPattern(0x401000, 0xE52000, pattern, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",0);
		if(patch1)
		{
			VirtualProtect( (void*)(patch1 + 9), 0x2, PAGE_READWRITE, &dwProtect );
			BYTE* p = (BYTE *)(patch1 + 9);
			*p++ = 0xB0;
			*p = 0x01;
			VirtualProtect( (void*)(patch1 + 9), 0x2, dwProtect, &dwProtect );
			fprintf(Log, "Patch position 1 (DLC): patched at 0x%x\n", patch1);
		}
		else
		{
			fprintf(Log, "Patch position 1 (DLC): byte pattern not found\n");
		}
		patch2 = FindPattern(0x401000, 0xE52000, pattern2, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",0);
		if(patch2)
		{
			VirtualProtect( (void*)patch2, 0x16, PAGE_READWRITE, &dwProtect );
			BYTE* p = (BYTE *)(patch2 + 5);
			*p++ = 0;
			*p++ = 0;
			*p++ = 0;
			*p = 0;
			p = (BYTE *)(patch2 + 0x12);
			*p++ = 0;
			*p++ = 0;
			*p++ = 0;
			*p = 0;
			VirtualProtect( (void*)patch2, 0x16, dwProtect, &dwProtect );
			fprintf(Log, "Patch position 2 (console): patched at 0x%x\n", patch2);
		}
		else
		{
			fprintf(Log, "Patch position 2 (console): byte pattern not found\n");
		}
		patch3 = FindPattern(0x401000, 0xE52000, pattern3, "xxxxxxxx",0);
		if(patch3)
		{
			VirtualProtect( (void*)patch3, 0x8, PAGE_READWRITE, &dwProtect );
			BYTE* p = (BYTE *)(patch3 + 1);
			*p++ = 0;
			*p++ = 0;
			*p++ = 0;
			*p = 0;
			VirtualProtect( (void*)patch3, 0x8, dwProtect, &dwProtect );
			fprintf(Log, "Patch position 3 (certcheck): patched at 0x%x\n", patch3);	
		}
		else
		{
			fprintf(Log, "Patch position 3 (certcheck): byte pattern not found\n");
		}
		loadPlugins(Log, ".");
		loadPlugins(Log, "asi");
		fclose(Log);
		return 0;
}


BOOL WINAPI DllMain(HINSTANCE hInst,DWORD reason,LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{			
		DWORD dwThreadId, dwThrdParam = 1;
		HANDLE hThread;
		hThread = CreateThread(NULL,0, Start, &dwThrdParam, 0, &dwThreadId);
	}
	if (reason == DLL_PROCESS_DETACH)
	{
		FreeLibrary(hOriginalBink);
	}
	return 1;
}

// _BinkBufferBlit@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferBlit(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[0*4];
	}
}

// _BinkBufferCheckWinPos@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferCheckWinPos(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[1*4];
	}
}

// _BinkBufferClear@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferClear(int a1, int a2)
{
	__asm
	{
		jmp p[2*4];
	}
}

// _BinkBufferClose@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferClose(int a1)
{
	__asm
	{
		jmp p[3*4];
	}
}

// _BinkBufferGetDescription@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferGetDescription(int a1)
{
	__asm
	{
		jmp p[4*4];
	}
}

// _BinkBufferGetError@0
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferGetError()
{
	__asm
	{
		jmp p[5*4];
	}
}

// _BinkBufferLock@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferLock(int a1)
{
	__asm
	{
		jmp p[6*4];
	}
}

// _BinkBufferOpen@16
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferOpen(int a1,int a2,int a3,int a4)
{
	__asm
	{
		jmp p[7*4];
	}
}

// _BinkBufferSetDirectDraw@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferSetDirectDraw(int a1, int a2)
{
	__asm
	{
		jmp p[8*4];
	}
}

// _BinkBufferSetHWND@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferSetHWND(int a1, int a2)
{
	__asm
	{
		jmp p[9*4];
	}
}

// _BinkBufferSetOffset@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferSetOffset(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[10*4];
	}
}

// _BinkBufferSetResolution@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferSetResolution(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[11*4];
	}
}

// _BinkBufferSetScale@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferSetScale(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[12*4];
	}
}

// _BinkBufferUnlock@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkBufferUnlock(int a1)
{
	__asm
	{
		jmp p[13*4];
	}
}

// _BinkCheckCursor@20
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkCheckCursor(int a1,int a2,int a3,int a4,int a5)
{
	__asm
	{
		jmp p[14*4];
	}
}

// _BinkClose@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkClose(int a1)
{
	__asm
	{
		jmp p[15*4];
	}
}

// _BinkCloseTrack@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkCloseTrack(int a1)
{
	__asm
	{
		jmp p[16*4];
	}
}

// _BinkControlBackgroundIO@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkControlBackgroundIO(int a1, int a2)
{
	__asm
	{
		jmp p[17*4];
	}
}

// _BinkControlPlatformFeatures@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkControlPlatformFeatures(int a1, int a2)
{
	__asm
	{
		jmp p[18*4];
	}
}

// _BinkCopyToBuffer@28
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkCopyToBuffer(int a1,int a2,int a3,int a4,int a5,int a6,int a7)
{
	__asm
	{
		jmp p[19*4];
	}
}

// _BinkCopyToBufferRect@44
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkCopyToBufferRect(int a1,int a2,int a3,int a4,int a5,int a6,int a7,int a8,int a9,int a10,int a11)
{
	__asm
	{
		jmp p[20*4];
	}
}

// _BinkDDSurfaceType@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkDDSurfaceType(int a1)
{
	__asm
	{
		jmp p[21*4];
	}
}

// _BinkDX8SurfaceType@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkDX8SurfaceType(int a1)
{
	__asm
	{
		jmp p[22*4];
	}
}

// _BinkDX9SurfaceType@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkDX9SurfaceType(int a1)
{
	__asm
	{
		jmp p[23*4];
	}
}

// _BinkDoFrame@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkDoFrame(int a1)
{
	__asm
	{
		jmp p[24*4];
	}
}

// _BinkDoFrameAsync@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkDoFrameAsync(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[25*4];
	}
}

// _BinkDoFrameAsyncWait@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkDoFrameAsyncWait(int a1, int a2)
{
	__asm
	{
		jmp p[26*4];
	}
}

// _BinkDoFramePlane@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkDoFramePlane(int a1, int a2)
{
	__asm
	{
		jmp p[27*4];
	}
}

// _BinkGetError@0
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetError()
{
	__asm
	{
		jmp p[28*4];
	}
}

// _BinkGetFrameBuffersInfo@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetFrameBuffersInfo(int a1, int a2)
{
	__asm
	{
		jmp p[29*4];
	}
}

// _BinkGetKeyFrame@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetKeyFrame(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[30*4];
	}
}

// _BinkGetPalette@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetPalette(int a1)
{
	__asm
	{
		jmp p[31*4];
	}
}

// _BinkGetRealtime@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetRealtime(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[32*4];
	}
}

// _BinkGetRects@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetRects(int a1, int a2)
{
	__asm
	{
		jmp p[33*4];
	}
}

// _BinkGetSummary@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetSummary(int a1, int a2)
{
	__asm
	{
		jmp p[34*4];
	}
}

// _BinkGetTrackData@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetTrackData(int a1, int a2)
{
	__asm
	{
		jmp p[35*4];
	}
}

// _BinkGetTrackID@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetTrackID(int a1, int a2)
{
	__asm
	{
		jmp p[36*4];
	}
}

// _BinkGetTrackMaxSize@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetTrackMaxSize(int a1, int a2)
{
	__asm
	{
		jmp p[37*4];
	}
}

// _BinkGetTrackType@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGetTrackType(int a1, int a2)
{
	__asm
	{
		jmp p[38*4];
	}
}

// _BinkGoto@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkGoto(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[39*4];
	}
}

// _BinkIsSoftwareCursor@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkIsSoftwareCursor(int a1, int a2)
{
	__asm
	{
		jmp p[40*4];
	}
}

// _BinkLogoAddress@0
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkLogoAddress()
{
	__asm
	{
		jmp p[41*4];
	}
}

// _BinkNextFrame@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkNextFrame(int a1)
{
	__asm
	{
		jmp p[42*4];
	}
}

// _BinkOpen@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkOpen(int a1, int a2)
{
	__asm
	{
		jmp p[43*4];
	}
}

// _BinkOpenDirectSound@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkOpenDirectSound(int a1)
{
	__asm
	{
		jmp p[44*4];
	}
}

// _BinkOpenMiles@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkOpenMiles(int a1)
{
	__asm
	{
		jmp p[45*4];
	}
}

// _BinkOpenTrack@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkOpenTrack(int a1, int a2)
{
	__asm
	{
		jmp p[46*4];
	}
}

// _BinkOpenWaveOut@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkOpenWaveOut(int a1)
{
	__asm
	{
		jmp p[47*4];
	}
}

// _BinkPause@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkPause(int a1, int a2)
{
	__asm
	{
		jmp p[48*4];
	}
}

// _BinkRegisterFrameBuffers@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkRegisterFrameBuffers(int a1, int a2)
{
	__asm
	{
		jmp p[49*4];
	}
}

// _BinkRequestStopAsyncThread@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkRequestStopAsyncThread(int a1)
{
	__asm
	{
		jmp p[50*4];
	}
}

// _BinkRestoreCursor@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkRestoreCursor(int a1)
{
	__asm
	{
		jmp p[51*4];
	}
}

// _BinkService@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkService(int a1)
{
	__asm
	{
		jmp p[52*4];
	}
}

// _BinkSetError@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetError(int a1)
{
	__asm
	{
		jmp p[53*4];
	}
}

// _BinkSetFrameRate@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetFrameRate(int a1, int a2)
{
	__asm
	{
		jmp p[54*4];
	}
}

// _BinkSetIO@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetIO(int a1)
{
	__asm
	{
		jmp p[55*4];
	}
}

// _BinkSetIOSize@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetIOSize(int a1)
{
	__asm
	{
		jmp p[56*4];
	}
}

// _BinkSetMemory@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetMemory(int a1, int a2)
{
	__asm
	{
		jmp p[57*4];
	}
}

// _BinkSetMixBinVolumes@20
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetMixBinVolumes(int a1,int a2,int a3,int a4,int a5)
{
	__asm
	{
		jmp p[58*4];
	}
}

// _BinkSetMixBins@16
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetMixBins(int a1,int a2,int a3,int a4)
{
	__asm
	{
		jmp p[59*4];
	}
}

// _BinkSetPan@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetPan(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[60*4];
	}
}

// _BinkSetSimulate@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetSimulate(int a1)
{
	__asm
	{
		jmp p[61*4];
	}
}

// _BinkSetSoundOnOff@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetSoundOnOff(int a1, int a2)
{
	__asm
	{
		jmp p[62*4];
	}
}

// _BinkSetSoundSystem@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetSoundSystem(int a1, int a2)
{
	__asm
	{
		jmp p[63*4];
	}
}

// _BinkSetSoundTrack@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetSoundTrack(int a1, int a2)
{
	__asm
	{
		jmp p[64*4];
	}
}

// _BinkSetVideoOnOff@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetVideoOnOff(int a1, int a2)
{
	__asm
	{
		jmp p[65*4];
	}
}

// _BinkSetVolume@12
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkSetVolume(int a1, int a2, int a3)
{
	__asm
	{
		jmp p[66*4];
	}
}

// _BinkShouldSkip@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkShouldSkip(int a1)
{
	__asm
	{
		jmp p[67*4];
	}
}

// _BinkStartAsyncThread@8
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkStartAsyncThread(int a1, int a2)
{
	__asm
	{
		jmp p[68*4];
	}
}

// _BinkWait@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkWait(int a1)
{
	__asm
	{
		jmp p[69*4];
	}
}

// _BinkWaitStopAsyncThread@4
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall BinkWaitStopAsyncThread(int a1)
{
	__asm
	{
		jmp p[70*4];
	}
}

// _RADTimerRead@0
extern "C" __declspec(dllexport) __declspec(naked) void __stdcall RADTimerRead()
{
	__asm
	{
		jmp p[71*4];
	}
}