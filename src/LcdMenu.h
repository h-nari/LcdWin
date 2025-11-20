#ifndef _lcd_menu_h_
#define _lcd_menu_h_

#include <Humblesoft_ILI9341.h>

#include "LcdWin.h"
#include "LcdRoot.h"
#include "LcdMenuEntry.h"
#include "LcdButton.h"
#include "LcdMesBox.h"

class LcdMenu : public LcdWin {
 public:
  LcdMenu();
  void add(LcdMenuEntry *pEntry);
  void popup(LcdRoot *rootWin, uint16_t x, uint16_t y);
  void activated() override;
  void reactivated() override;
  bool tp_pressed(int16_t x, int16_t y) override;
  bool tp_drag(int16_t x, int16_t y) override;
  bool tp_released(int16_t x, int16_t y) override;
  void adjustSize();
  void drawAll();
  bool drawChanged(uint16_t x, uint16_t y);
  void drawRow(LcdMenuEntry *p,uint16_t x, uint16_t y, bool selected = false);
  int8_t findSelectedRow(int16_t x, int16_t y);
  int8_t getSelectedRow() {return m_selectedRow;}
  
 protected:
  LcdMenuEntry *m_lEntry;
  // uint8_t m_textSize;
  // uint8_t m_ox;
  // uint8_t m_oy;
  uint8_t m_margin;
  uint8_t m_rowHeight;
  int8_t m_selectedRow;
  bool    m_vSize;
  uint16_t m_textColor;
};

extern RomFontx font24;


#endif /* _lcd_menu_h_ */
