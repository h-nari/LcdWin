#include <Arduino.h>
#include "LcdMenu.h"
#include <LcdTouchscreen.h>

LcdRoot::LcdRoot(LcdDrv *lcd, LcdTouchscreen *tp)
{
  m_pLcd = lcd;
  m_pTp  = tp;
  m_lWin = NULL;
  m_bPressed0 = false;
  m_bg = Humblesoft_ILI9341::rgb("BLACK");
}

void LcdRoot::begin(void)
{
  if(m_lWin) m_lWin->activated();
}

void LcdRoot::update(void)
{
  int16_t x, y;
  bool bPressed;

  bPressed =  m_pTp->getPoint(&x, &y);

  if(bPressed){
    if(!m_bPressed0){
      if(m_lWin)
	m_lWin->tp_pressed(x, y);
    }
    else if(x != m_x0 || y != m_y0){
      if(m_lWin)
	m_lWin->tp_drag(x,y);
    }
  }
  else {
    if(m_bPressed0){
      if(m_lWin)
	m_lWin->tp_released(m_x0,m_y0);
    }
  }

  m_bPressed0 = bPressed;
  m_x0 = x;
  m_y0 = y;

  m_lWin->update();
}

void LcdRoot::bindWin(LcdWin *pWin)
{
  pWin->m_root = this;
  pWin->m_pLcd = m_pLcd;
}

void LcdRoot::pushWin(LcdWin *pWin)
{
  pWin->m_root = this;
  pWin->m_pNext = m_lWin;
  pWin->m_pLcd = m_pLcd;
  if(m_lWin)
    m_lWin->deactivated();
  m_lWin = pWin;

  pWin->activated();
}

void LcdRoot::popWin(LcdWin *pWin)
{
  if(m_lWin == pWin || !pWin){
    if(!pWin) pWin = m_lWin;
    m_lWin = pWin->m_pNext;
    pWin->deactivated();
    if(m_lWin)
      m_lWin->reactivated();
#if 0
    else
      redisplay();
#endif
  }
  else 
    Serial.printf("popWin Error, bad win\n");
}

void LcdRoot::popWinAll()
{
  while(m_lWin){
    m_lWin->deactivated();
    m_lWin = m_lWin->m_pNext;
  }
}

void LcdRoot::redisplay(void)
{
  m_pLcd->fillScreen(m_bg);
}


