#include "Includes.h"

void DrawFillRect(int x, int y, int w, int h, D3DCOLOR col) {

	D3DRECT rect = { x, y, x + w, y + h };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, col, 0, 0);

}