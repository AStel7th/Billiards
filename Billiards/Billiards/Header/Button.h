#pragma once
#include <functional>
#include "GameObject.h"
#include "Sprite.h"
#include "Direct3D11.h"
#include "InputDeviceManager.h"

using namespace std;

class Sprite;
class TextureInfo;

template<class T>
class Button : public GameObject
{
public:
	typedef void (T::*EventFunc)();
private:
	enum ButtonState
	{
		Normal,
		MouseOver,
		ButtonDown
	};

	T* pObj;
	EventFunc func;
	ButtonState nowState;
	ButtonState beforeState;
	XMFLOAT2 pos;
	Sprite* pButtonSprite;
	Sprite* pMouseOverSprite;
	Sprite* pButtonDownSprite;
public:
	Button(float x, float y, T* obj, EventFunc f, TextureInfo* tex, TextureInfo* overtex = nullptr, TextureInfo* downtex = nullptr) : 
		GameObject(), 
		pButtonSprite(nullptr), 
		pMouseOverSprite(nullptr), 
		pButtonDownSprite(nullptr)
	{
		SetTag("Button");

		nowState = Normal;
		beforeState = Normal;

		pObj = obj;
		func = f;

		pButtonSprite = NEW Sprite();
		pButtonSprite->InitCenter(tex, x, y);

		if (overtex != nullptr)
		{
			pMouseOverSprite = NEW Sprite();
			pMouseOverSprite->InitCenter(overtex, x, y);
		}
		if (downtex != nullptr)
		{
			pButtonDownSprite = NEW Sprite();
			pButtonDownSprite->InitCenter(downtex, x, y);
		}
	}

	virtual ~Button()
	{
		SAFE_DELETE(pButtonSprite);
		SAFE_DELETE(pMouseOverSprite);
		SAFE_DELETE(pButtonDownSprite);
	}

	void Update()
	{
		beforeState = nowState;

		if (!IsMouseOver())
		{
			nowState = Normal;
		}
		else
		{
			if (InputDeviceManager::Instance().GetMouseButtonDown(0) == true)
			{
				nowState = ButtonDown;
				
			}
			else
			{
				nowState = MouseOver;
				
			}
		}


		switch (nowState)
		{
		case ButtonState::Normal:

			pButtonSprite->Draw();
		
			break;

		case ButtonState::MouseOver:

			if (pMouseOverSprite != nullptr)
				pMouseOverSprite->Draw();
			else
				pButtonSprite->Draw();

			break;

		case ButtonState::ButtonDown:
			if (pButtonDownSprite != nullptr)
			{
				if(beforeState != nowState)
					(pObj->*func)();

				pButtonDownSprite->Draw();
			}
			else
				pButtonSprite->Draw();
			break;
		}
	}

	bool IsMouseOver()
	{
		POINT pos;
		GetCursorPos(&pos);

		XMFLOAT2 spPos = *pButtonSprite->GetPos();

		int width, height;
		pButtonSprite->GetSize(&width, &height);

		if ((float)pos.x > spPos.x - ((float)width / 2.0f) && (float)pos.x < spPos.x + ((float)width / 2.0f))
		{
			if ((float)pos.y > spPos.y && (float)pos.y < spPos.y + (float)height)
			{
				return true;
			}
		}

		return false;
	}
};