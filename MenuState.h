#pragma once
#include <vector>
#include "GameState.h"
#include "Bitmap.h"
#include "text2D.h"

class MenuState : public GameState
{
public:

	virtual void update(); 
	virtual void render(); 
	virtual bool onEnter(); 
	virtual bool onExit(); 
	virtual std::string getStateID() const { return s_menuID; }


private:
	
	// call back funtions for menu items
	static void s_menuToPlay(); 
	static void s_exitFromMenu(); 

	static const std::string s_menuID;

	Bitmap*		bitmap; 
	Text2D*		text; 


};

