#pragma once

#include <iostream>
#include <string>

#include "SDL/SDL_image.h"
#include "SDL/SDL.h"

class Core
{
public:
	//初始化SDL，创建窗口
	Core();
	//开始读取图片进入显示
	int Run(const char* filename);
private:
	static unsigned int sm_unInstanceCount;
	SDL_Window* m_pWin;
	SDL_Renderer* m_pRen;
	int m_nWinWidth, m_nWinHeight;
	int m_nPicWidth, m_nPicHeight;

	SDL_Texture* m_pTexNow;
	SDL_Rect m_rtShow;//初始显示

	//用于文件是否打开成功的标志
	//成功打开文件后是是否软件停止的标志
	bool m_bFailed;
	SDL_Event m_msg;
	SDL_Thread* m_pThr;
	SDL_Point m_ptPrevMousePos;

	int m_nZoomInLevel;

private:
	void __ResizeShowRect(void);
	void __ProcessEvent(void);
	void __ZoomIn(void);
	void __ZoomOut(void);
	void __DragMouse(void);
};
