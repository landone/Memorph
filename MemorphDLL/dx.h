#pragma once

#include <glm/glm.hpp>
#include <d3d9.h>

class DX {
public:

	static bool HookEndScene(void (*callback)());
	static void UnhookEndScene();

	static bool WorldToScreen(const glm::vec4& pos, const glm::mat4& matrix, glm::vec2& output);
	static void DrawFillRect(glm::vec2 pos, glm::vec2 dim, glm::vec4 color);
	static void DrawLine(glm::vec2 pos, glm::vec2 pos2, int width, glm::vec4 color);

private:

	friend void APIENTRY preDXCallback(LPDIRECT3DDEVICE9);
	friend BOOL CALLBACK DXenumWind(HWND,LPARAM);

	typedef HRESULT(APIENTRY* tEndScene)(LPDIRECT3DDEVICE9 pDevice);
	static bool GetD3D9Device(void** pTable, size_t size);
	static HWND GetProcessWindow();

	static HWND window;
	/* Window dimensions */
	static int windowDim[2];
	static LPDIRECT3DDEVICE9 pDevice;
	static void* d3d9Device[119];
	static BYTE EndSceneByte[7];
	static tEndScene oEndScene;
	static void(*callback)();

};