#pragma once

#include <glm/glm.hpp>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>

class DX {
public:

	struct Font {
		ID3DXFont* data;
		int height;
	};

	static bool HookEndScene(void (*callback)());
	static void UnhookEndScene();

	static bool WorldToScreen(const glm::vec3& pos, const glm::mat4& matrix, glm::vec2& output);
	static void DrawFillRect(glm::vec2 pos, glm::vec2 dim, glm::vec4 color);
	static void DrawLine(glm::vec2 pos, glm::vec2 pos2, int width, glm::vec4 color);
	static void WriteText(glm::vec2 pos, std::string text, glm::vec4 color, Font font = { nullptr, 0 });
	static Font MakeFont(int height, std::string name = "Arial", bool bold = false, bool italic = false);

	static glm::vec2 GetWindowDim() { return glm::vec2(windowDim[0], windowDim[1]); }

private:

	friend void APIENTRY preDXCallback(LPDIRECT3DDEVICE9);
	friend BOOL CALLBACK DXenumWind(HWND,LPARAM);

	typedef HRESULT(APIENTRY* tEndScene)(LPDIRECT3DDEVICE9 pDevice);
	static bool GetD3D9Device(void** pTable, size_t size);
	static HWND GetProcessWindow();

	static HWND window;
	static Font font;
	/* Window dimensions */
	static int windowDim[2];
	static LPDIRECT3DDEVICE9 pDevice;
	static void* d3d9Device[119];
	static BYTE EndSceneByte[7];
	static tEndScene oEndScene;
	static void(*callback)();

};