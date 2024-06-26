#ifndef _TUI_IO_H
#define _TUI_IO_H
/*
   ** This header contains defitions to support tuiIO.c
 */


#include <stdio.h>

extern void tuiPuts_unfiltered  (const char *, struct ui_file *);
extern unsigned int tuiGetc (void);
extern unsigned int tuiBufferGetc (void);
extern int tuiRead (int, char *, int);
extern void tuiStartNewLines (int);
extern void tui_vStartNewLines (va_list);
extern unsigned int tui_vwgetch (va_list);
extern void tuiTermSetup (int);
extern void tuiTermUnsetup (int, int);



#define m_tuiStartNewLine       tuiStartNewLines(1)
#define m_isStartSequence(ch)   (ch == 27)
#define m_isEndSequence(ch)     (ch == 126)
#define m_isBackspace(ch)       (ch == 8)
#define m_isDeleteChar(ch)      (ch == KEY_DC)
#define m_isDeleteLine(ch)      (ch == KEY_DL)
#define m_isDeleteToEol(ch)     (ch == KEY_EOL)
#define m_isNextPage(ch)        (ch == KEY_NPAGE)
#define m_isPrevPage(ch)        (ch == KEY_PPAGE)
#define m_isLeftArrow(ch)       (ch == KEY_LEFT)
#define m_isRightArrow(ch)      (ch == KEY_RIGHT)

#define m_isCommandChar(ch)     (m_isNextPage(ch) || m_isPrevPage(ch) || \
                                m_isLeftArrow(ch) || m_isRightArrow(ch) || \
                                (ch == KEY_UP) || (ch == KEY_DOWN) || \
                                (ch == KEY_SF) || (ch == KEY_SR) || \
                                (ch == (int)'\f') || m_isStartSequence(ch))

#define m_isXdbStyleCommandChar(ch)     (m_isNextPage(ch) || m_isPrevPage(ch))


#endif
/*_TUI_IO_H*/
