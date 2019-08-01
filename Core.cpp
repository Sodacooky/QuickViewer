#include "Core.h"
Core::Core()
{
	try
	{
		m_nWinWidth = 800;
		m_nWinHeight = 600;
		m_nPicWidth = 0;
		m_nPicHeight = 0;
		m_rtShow = { 0,0,0,0 };
		m_pTexNow = nullptr;
		m_pThr = nullptr;
		m_nZoomInLevel = 0;
		m_ptPrevMousePos = { 0,0 };

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			throw("Core::Core() SDL_Init() failed!");
		}
		m_pWin = SDL_CreateWindow(u8"QuickView",
								  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
								  m_nWinWidth, m_nWinHeight,
								  SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
		if (m_pWin == nullptr)
		{
			throw("Core::Core() SDL_CreateWindow() failed!");
		}
		m_pRen = SDL_CreateRenderer(m_pWin, -1, SDL_RENDERER_ACCELERATED);
		if (m_pRen == nullptr)
		{
			throw("Core::Core() SDL_CreateRenderer() failed!");
		}
	}
	catch (std::string e)
	{
		std::cout << e << std::endl;
		exit(-1);
	}
	m_bFailed = false;
	SDL_PollEvent(&m_msg);
}

int Core::Run(const char* filename)
{
	SDL_Surface* pSurPic = nullptr;
	if (filename != nullptr)
	{
		pSurPic = IMG_Load(filename);
	}
	if (pSurPic == nullptr)
	{
		m_bFailed = true;
		SDL_FreeSurface(pSurPic);
		pSurPic = SDL_LoadBMP("filefailed.bmp");
		SDL_SetColorKey(pSurPic, true, SDL_MapRGB(pSurPic->format, 0, 255, 0));
	}
	m_nPicWidth = pSurPic->w;
	m_nPicHeight = pSurPic->h;
	m_pTexNow = SDL_CreateTextureFromSurface(m_pRen, pSurPic);
	SDL_FreeSurface(pSurPic);

	__ResizeShowRect();

	while (!m_bFailed)
	{
		SDL_WaitEvent(&m_msg);
		__ProcessEvent();
		SDL_RenderClear(m_pRen);
		SDL_RenderCopy(m_pRen, m_pTexNow, nullptr, &m_rtShow);
		SDL_RenderPresent(m_pRen);
	}

	//辣鸡回收
	return 0;
}

void Core::__ResizeShowRect(void)
{
	double rate = 1.0;
	if (m_nPicHeight >= m_nPicWidth)
	{
		m_rtShow.h = m_nWinHeight;
		rate = (double)m_rtShow.h / (double)m_nPicHeight;
		m_rtShow.w = (int)(m_nPicWidth * rate);
		m_rtShow.y = 0;
		m_rtShow.x = static_cast<int>(m_nWinWidth / 2) - static_cast<int>(m_rtShow.w / 2);
	}
	else
	{
		m_rtShow.w = m_nWinWidth;
		rate = (double)m_rtShow.w / (double)m_nPicWidth;
		m_rtShow.h = (int)(m_nPicHeight * rate);
		m_rtShow.x = 0;
		m_rtShow.y = static_cast<int>(m_nWinHeight / 2) - static_cast<int>(m_rtShow.h / 2);
	}
	std::cout << std::endl
		<< "实际大小：" << m_nPicWidth << " x " << m_nPicHeight << std::endl
		<< "某轴缩放:" << rate << std::endl
		<< "初始显示大小：" << m_rtShow.w << " x " << m_rtShow.h << std::endl;
}

void Core::__ProcessEvent(void)
{
	switch (m_msg.type)
	{
	case SDL_MOUSEMOTION:
		m_ptPrevMousePos = { m_msg.button.x,m_msg.button.y };
		break;
	case SDL_MOUSEWHEEL:
		if (m_msg.wheel.y > 0)
		{
			//放大
			__ZoomIn();
		}
		else if (m_msg.wheel.y < 0)
		{
			//缩小
			__ZoomOut();
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (m_msg.button.button == SDL_BUTTON_LEFT)
		{
			__DragMouse();
		}
		break;
	case SDL_WINDOWEVENT:
		if (m_msg.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			m_nWinWidth = m_msg.window.data1;
			m_nWinHeight = m_msg.window.data2;
			__ResizeShowRect();
		}
		break;
	case SDL_QUIT:
		m_bFailed = true;
		break;
	default:
		break;
	}
}

void Core::__ZoomIn(void)
{
	if (m_nZoomInLevel >= (int)(m_nPicHeight * m_nPicWidth / 100000))
	{
		return;
	}
	++m_nZoomInLevel;
	m_rtShow.w *= 2;
	m_rtShow.h *= 2;
	m_rtShow.x = (-m_rtShow.w / 2) + (m_nWinWidth / 2);
	m_rtShow.y = (-m_rtShow.h / 2) + (m_nWinHeight / 2);//暂时就这样吧
}

void Core::__ZoomOut(void)
{
	__ResizeShowRect();//暂时这样吧
	m_nZoomInLevel = 0;
}

void Core::__DragMouse(void)
{
	bool bHolding = true;
	while (bHolding)
	{
		SDL_WaitEvent(&m_msg);
		if (m_msg.type == SDL_MOUSEBUTTONUP)
		{
			bHolding = false;
		}
		else if (m_msg.type == SDL_MOUSEMOTION)
		{
			int deltaX = m_msg.button.x - m_ptPrevMousePos.x;
			int deltaY = m_msg.button.y - m_ptPrevMousePos.y;
			m_rtShow.x += deltaX;
			m_rtShow.y += deltaY;
			SDL_RenderClear(m_pRen);
			SDL_RenderCopy(m_pRen, m_pTexNow, nullptr, &m_rtShow);
			SDL_RenderPresent(m_pRen);
			m_ptPrevMousePos = { m_msg.button.x ,m_msg.button.y };
		}
	}
}