#include <glm/glm.hpp>
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "DX.h"
#include "Hook.h"

HWND DX::window;
int DX::windowDim[2];
LPDIRECT3DDEVICE9 DX::pDevice;
void* DX::d3d9Device[119];
BYTE DX::EndSceneByte[7];
DX::tEndScene DX::oEndScene;
void (*DX::callback)() = nullptr;

void DX::DrawFillRect(glm::vec2 pos, glm::vec2 dim, glm::vec4 color) {

	D3DRECT rect = { pos.x, pos.y, pos.x + dim.x, pos.y + dim.y };
	DX::pDevice->Clear(1, &rect, D3DCLEAR_TARGET, D3DCOLOR_RGBA((int)color.r, (int)color.g, (int)color.b, (int)color.a), 0, 0);

}

bool DX::WorldToScreen(const glm::vec4& pos, const glm::mat4& matrix, glm::vec2& output) {

	glm::vec4 product = pos * matrix;
	if (product.w < 0.1) {
		return false;
	}

	glm::vec3 NDC = glm::vec3(product) / product.w;
	output.x = (DX::windowDim[0] / 2 * NDC.x) + (NDC.x + DX::windowDim[0] / 2);
	output.y = -(DX::windowDim[1] / 2 * NDC.y) + (NDC.y + DX::windowDim[1] / 2);

	return true;

}

void APIENTRY preDXCallback(LPDIRECT3DDEVICE9 o_pDevice) {

	if (!DX::pDevice) {
		DX::pDevice = o_pDevice;
	}

	if (DX::callback) {
		DX::callback();
	}

	DX::oEndScene(DX::pDevice);

}

bool DX::HookEndScene(void (*cbk)()) {

	if (cbk == nullptr) {
		return false;
	}
	if (callback) {
		callback = cbk;
		return true;
	}
	else if (DX::GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {

		/* Grab EndScene() */
		memcpy(EndSceneByte, (char*)d3d9Device[42], 7);
		callback = cbk;
		/* Store original EndScene */
		oEndScene = (DX::tEndScene)Hook::TrampHook((char*)d3d9Device[42], (char*)preDXCallback, 7);
		return true;

	}

	return false;

}

void DX::UnhookEndScene() {

	if (callback) {
		Hook::Patch((BYTE*)d3d9Device[42], EndSceneByte, 7);
		callback = nullptr;
	}

}

BOOL CALLBACK DXenumWind(HWND handle, LPARAM lp) {

	DWORD procID;
	GetWindowThreadProcessId(handle, &procID);
	if (GetCurrentProcessId() != procID) {
		return TRUE;
	}

	DX::window = handle;
	return FALSE;

}

HWND DX::GetProcessWindow() {

	window = NULL;
	EnumWindows(DXenumWind, NULL);
	if (!window) {
		return NULL;
	}

	RECT size;
	GetWindowRect(window, &size);
	windowDim[0] = size.right - size.left;
	windowDim[1] = size.bottom - size.top;

	return window;

}

bool DX::GetD3D9Device(void** pTable, size_t size) {

	if (!pTable) {
		return false;
	}

	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (!pD3D) {
		return false;
	}

	IDirect3DDevice9* pDummyDevice = nullptr;

	D3DPRESENT_PARAMETERS d3dpp = {};
	d3dpp.Windowed = false;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetProcessWindow();

	HRESULT dummyDevCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
									d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

	if (dummyDevCreated != S_OK) {
		d3dpp.Windowed = !d3dpp.Windowed;
		HRESULT dummyDevCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);
		if (dummyDevCreated != S_OK) {
			pD3D->Release();
			return false;
		}
	}

	memcpy(pTable, *(void***)(pDummyDevice), size);
	pDummyDevice->Release();
	pD3D->Release();
	return true;

}