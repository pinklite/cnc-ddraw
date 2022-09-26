#ifndef DDCLIPPER_H
#define DDCLIPPER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "ddraw.h"
#include "IDirectDrawClipper.h"


HRESULT ddc_GetHWnd(IDirectDrawClipperImpl* This, HWND FAR* lphWnd);
HRESULT ddc_SetHWnd(IDirectDrawClipperImpl* This, DWORD dwFlags, HWND hWnd);
HRESULT dd_CreateClipper(DWORD dwFlags, IDirectDrawClipperImpl** lplpDDClipper, IUnknown FAR* pUnkOuter);

#endif
