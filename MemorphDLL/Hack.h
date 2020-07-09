#pragma once

class Hack {
public:
	
	virtual void OnStart() {};
	virtual void OnEnd() {};
	virtual void OnThink() {};
	virtual void OnDraw() {};
	virtual void OnKey(unsigned char keyCode, bool pressed) {}

};