/*
   旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
   �  CXL is Copyright (c) 1987-1989 by Mike Smedley.                         �
   읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
*/

#if !defined(__CXL_H__)
#define __CXL_H__

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__LINUX__)
#define ERR              (-1)
#endif

#if defined(__DOS__)
#include <bios.h>
#include <conio.h>
#include <dos.h>
#endif

#if defined(__OS2__)
#define INCL_NOPMAPI
#define INCL_VIO
#define INCL_MOU
#include <os2.h>
#endif

#if defined(__TURBOC__)                     /* Turbo C */
    #if __STDC__
        #define _Cdecl
    #else
        #define _Cdecl  cdecl
    #endif
    #define _Near
#elif defined(__ZTC__)                      /* Zortech C++ */
    #define _Cdecl
    #define _Near
#elif defined(__WATCOM__) && defined(__DOS__)
    #define _Cdecl
    #define _Near
#elif defined(M_I86) && !defined(__ZTC__)   /* Microsoft C/QuickC */
    #if !defined(NO_EXT_KEYS)
        #define _Cdecl  cdecl
        #define _Near   near
    #else
        #define _Cdecl
        #define _Near
    #endif
#elif defined(__OS2__) || defined(__LINUX__) || defined(__NT__)
    #define _Cdecl
    #define _Near
#endif

/*-----------------------[ Boolean logic conditions ]------------------------*/

#define NO      0
#define YES     1
#if !defined(FALSE)
#define FALSE   0
#endif
#if !defined(TRUE)
#define TRUE    1
#endif
#define OFF     0
#define ON      1

/*-----------------[ definitions for common control codes ]------------------*/

#define NUL  0
#define BEL  7
#define BS   8
#define HT   9
#define LF   10
#define FF   12
#define CR   13

/*------------------------------[ CXLWIN type ]------------------------------*/

typedef short     CXLWIN;

/*-------------[ display adapter types returned from vidtype() ]-------------*/

//#define V_NONE      0
#define V_MDA       1
#define V_EGAMONO   2
#define V_MCGAMONO  3
#define V_VGAMONO   4
#define V_HGC       5
#define V_HGCPLUS   6
#define V_INCOLOR   7
#define V_CGA       8
#define V_EGA       9
#define V_MCGA      10
#define V_VGA       11

/*------------------------[ video parameter settings ]-----------------------*/

#define VP_DMA    0     /* direct screen writes                     */
#define VP_CGA    1     /* direct screen writes, eliminate CGA snow */
#define VP_BIOS   2     /* BIOS screen writes                       */
#define VP_MONO   3     /* monochrome attribute translation on      */
#define VP_COLOR  4     /* monochrome attribute translation off     */

/*------------------------[ video information record ]-----------------------*/

struct _vinfo_t {
    unsigned int    videoseg;   /* video buffer segment address */
    unsigned char   adapter;    /* video adapter type           */
    unsigned char   numrows;    /* number of displayed rows     */
    unsigned char   numcols;    /* number of displayed columns  */
    unsigned char   cheight;    /* character height in pixels   */
    unsigned char   cwidth;     /* character width  in pixels   */
    unsigned char   mono;       /* is it a monochrome adapter?  */
    unsigned char   mapattr;    /* map color attribs to mono?   */
    unsigned char   cgasnow;    /* is CGA snow present?         */
    unsigned char   usebios;    /* use BIOS for video writes?   */
    unsigned char   dvcheck;    /* check for DESQview?          */
    unsigned char   dvexist;    /* is DESQview present?         */
};

extern struct _vinfo_t _vinfo;

/*--------------[ attribute codes for functions that use them ]--------------*/

#if     !defined(BLACK)     /* foreground colors */
#define BLACK       0
#define BLUE        1
#define GREEN       2
#define CYAN        3
#define RED         4
#define MAGENTA     5
#define BROWN       6
#define YELLOW      14
#define WHITE       15
#endif
#if     !defined(LGREY)
#define LGREY       7       /* CXL abbreviates LIGHT as L    */
#define DGREY       8       /* and DARK as D for ease of use */
#define LBLUE       9
#define LGREEN      10
#define LCYAN       11
#define LRED        12
#define LMAGENTA    13

#define _BLACK      0       /* background colors */
#define _BLUE       16
#define _GREEN      32
#define _CYAN       48
#define _RED        64
#define _MAGENTA    80
#define _BROWN      96
#define _LGREY      112
#endif

#if     !defined(BLINK)
#define BLINK       128     /* blink attribute */
#endif

/*------------------------[ definition of kbuf record ]----------------------*/

struct _kbuf_t {
    struct _kbuf_t *prev;       /* previous record */
    struct _kbuf_t *next;       /* next record     */
    unsigned xch;               /* keypress        */
};

/*-----------------------[ definition of onkey record ]----------------------*/

struct _onkey_t {
    struct _onkey_t *prev;      /* poshorter to previous record      */
    struct _onkey_t *next;      /* poshorter to next record          */
    unsigned short keycode;       /* Scan/ASCII code of trap key     */
    void (*func) (void);        /* address of onkey function       */
    unsigned short pass;          /* key to pass back, 0=don't pass  */
};

/*-------------------[ definition of keyboard info record ]------------------*/

struct _kbinfo_t {
    struct _kbuf_t *kbuf;       /* poshorter to head record in key buffer      */
    struct _onkey_t *onkey;     /* poshorter to head record in onkey list      */
    void (*kbloop) (void);      /* poshorter to function to call while waiting */
    unsigned char inmenu;       /* inmenu flag used by menuing functions     */
    unsigned char source;       /* source of keypress 0=kb, 1=kbuf, 2=mouse  */
};

extern struct _kbinfo_t _kbinfo;

/*-------------[ keyboard status codes returned from kbstat() ]--------------*/

#define RSHIFT      1       /* right shift pressed   */
#define LSHIFT      2       /* left shift pressed    */
#define CTRL        4       /* [Ctrl] pressed        */
#define SCRLOCK     16      /* [Scroll Lock] toggled */
#define INS         128     /* [Ins] toggled         */

/*-------------[ error codes returned from windowing functions ]-------------*/

#define W_NOERROR   0       /* no error                          */
#define W_ESCPRESS  1       /* Escape key was pressed            */
#define W_ALLOCERR  2       /* memory allocation error           */
#define W_NOTFOUND  3       /* record not found                  */
#define W_NOACTIVE  4       /* no active window                  */
#define W_INVCOORD  5       /* invalid coordinates               */
#define W_INVFORMT  6       /* invalid format string             */
#define W_NOINPDEF  7       /* no input fields defined           */
#define W_STRLONG   8       /* string too long for window        */
#define W_INVBTYPE  9       /* invalid box type                  */
#define W_NOBORDER  10      /* no window border                  */
#define W_NOHIDDEN  11      /* no hidden windows                 */
#define W_NOTHIDD   12      /* window is not hidden              */
#define W_NOSELECT  13      /* no selectable menu items          */
#define W_NOITMDEF  14      /* no menu items defined             */
#define W_NOMNUEND  15      /* no end of menu specified          */
#define W_NOMNUDEF  16      /* no menu defined                   */
#define W_NOMNUBEG  17      /* no begin of menu specified        */
#define W_NOFRMDEF  18      /* no form defined                   */
#define W_NOFRMBEG  19      /* no begin of form specified        */
#define W_NOHLPDEF  20      /* no help record defined            */
#define W_HLPSTKOV  21      /* help stack overflow               */
#define W_HLPSTKUN  22      /* help stack underflow              */
#define W_DOSERROR  23      /* DOS error                         */
#define W_NOMATCH   24      /* no files matched input filespec   */
#define W_INVTAGID  25      /* invalid tag identifier            */

/*-----------------------[ window border identifiers ]----------------------*/

#define TP_BORD  0      /* top border    */
#define BT_BORD  1      /* bottom border */
#define LT_BORD  2      /* left border   */
#define RT_BORD  3      /* right border  */

/*--------------------------[ direction codes ]------------------------------*/

#define D_DOWN    0
#define D_UP      1
#define D_LEFT    2
#define D_RIGHT   3
#define SDOWN     D_DOWN    /* scroll down */
#define SUP       D_UP      /* scroll up   */

/*----------------------[ menu item record definition ]----------------------*/

struct _item_t {
    struct _item_t *prev;   /* poshorter to previous record      */
    struct _item_t *next;   /* poshorter to next record          */
    void *child;            /* poshorter to submenu's record     */
    char *str;              /* address of selection string     */
    char *desc;             /* text description of menu option */
    void (*select) (void);  /* address of selection function   */
    void (*before) (void);  /* address of "before" function    */
    void (*after)  (void);  /* address of "after"  function    */
    unsigned hotkey;        /* hot key to select function      */
    short tagid;              /* tag identifier                  */
    short help;               /* help category number            */
    short wrow;               /* start of text - window row      */
    unsigned char wcol;     /* start of text - window column   */
    unsigned char schar;    /* quick selection character       */
    unsigned char fmask;    /* special feature mask            */
    unsigned char dwrow;    /* description window row          */
    unsigned char dwcol;    /* description window column       */
    unsigned char dattr;    /* description attribute           */
    unsigned char redisp;   /* redisplay flag                  */
};

/*--------------------[ window menu record definition ]----------------------*/

struct _menu_t {
    struct _menu_t *prev;       /* poshorter to prev menu structure   */
    struct _menu_t *next;       /* poshorter to next menu structure   */
    struct _menu_t *parent;     /* poshorter to parent menu           */
    struct _item_t *item;       /* poshorter to head menu item        */
    struct _item_t *citem;      /* poshorter to current menu item     */
    void (*open) (void);        /* address of post-opening function */
    short tagcurr;                /* tag ID of item selection bar on  */
    unsigned char usecurr;      /* will menu use current window?    */
    unsigned char srow;         /* starting row of menu window      */
    unsigned char scol;         /* starting column of menu window   */
    unsigned char erow;         /* ending row of menu window        */
    unsigned char ecol;         /* ending column of menu window     */
    unsigned char btype;        /* menu window border type          */
    unsigned char battr;        /* menu window border attribute     */
    unsigned char wattr;        /* menu window attribute            */
    unsigned char menutype;     /* menu type mask                   */
    unsigned char barwidth;     /* width of menu bar or zero        */
    unsigned char textpos;      /* offset of text from start of bar */
    unsigned char textattr;     /* attribute of menu text           */
    unsigned char scharattr;    /* attribute of selection character */
    unsigned char noselattr;    /* non-selectable text attribute    */
    unsigned char barattr;      /* attribute of selection bar       */
};

/*--------------------[ window help information record ]---------------------*/

struct _help_t {
    short help[20];               /* help stack                 */
    char *file;                 /* help file name             */
    short helpptr;                /* help stack poshorter         */
    unsigned short  key;          /* help hot key               */
    unsigned char winattr;      /* help window attribute      */
    unsigned char textattr;     /* help window text attribute */
    unsigned char selattr;      /* selection text attribute   */
    unsigned char barattr;      /* selection bar attribute    */
    unsigned char srow;         /* help window start row      */
    unsigned char scol;         /* help window start column   */
    unsigned char erow;         /* help window end row        */
    unsigned char ecol;         /* help window end column     */
    unsigned char btype;        /* help window box type       */
    unsigned char title;        /* display "Help" title?      */
    void (*open) (void);        /* poshorter to open function   */
};

/*-----------------------[ window information record ]-----------------------*/

struct _winfo_t {
    struct _wrec_t  *active;    /* poshorter to active window         */
    struct _wrec_t  *hidden;    /* poshorter to head hidden window    */
    struct _menu_t  *menu;      /* poshorter to head menu record      */
    struct _menu_t  *cmenu;     /* poshorter to current menu record   */
    struct _help_t  *helptr;    /* poshorter to help info record      */
    CXLWIN handle;              /* last handle given to a window    */
    short maxfiles;               /* max files allowed in wpickfile() */
    short help;                   /* current help category            */
    short error;                  /* error num from last window func  */
    short total;                  /* total number of open windows     */
    short mlevel,ilevel;          /* system variables used in menus   */
    unsigned char esc;          /* check for Esc in input funcions? */
    unsigned char tabwidth;     /* window TTY output tab width      */
    unsigned char fillch;       /* character to fill windows with   */
};

extern struct _winfo_t _winfo;

/*-------------------[ fmask definitions for wmenuitem() ]-------------------*/

#define M_HASPD     1   /* has pull-down menu attached   */
#define M_NOSEL     2   /* is not selectable             */
#define M_CLOSE     4   /* close menu after select func  */
#define M_CLALL     8   /* close all menus when selected */
#define M_CLOSB    16   /* close menu before select func */

/*------------------[ menutype definitions for wmenuend() ]------------------*/

#define M_HORZ      1       /* horizontal menu         */
#define M_VERT      2       /* vertical menu           */
#define M_OMNI      7       /* omnidirectional menu    */
#define M_PD        8       /* pull-down menu          */
#define M_NOQS      16      /* disable quick selection */
#define M_SAVE      32      /* save last bar position  */

/*------------------[ special return codes from wmenuget() ]-----------------*/

#define M_EXIT      32764   /* exit menu               */
#define M_EXITALL   32765   /* exit all menus          */
#define M_PREVPD    32766   /* previous pull-down menu */
#define M_NEXTPD    32767   /* next pull-down menu     */

/*---------------------[ window input field definition ]---------------------*/

struct _field_t {
    struct _field_t *prev;      /* poshorter to previous field       */
    struct _field_t *next;      /* poshorter to next field           */
    char *str;                  /* address of receiving string     */
    char *buf;                  /* address of temp receive string  */
    char *format;               /* input field format string       */
    short (*validate) (char *);   /* address of validation function  */
    void (*before) (void);      /* address of "before" function    */
    void (*after)  (void);      /* address of "after"  function    */
    short help;                   /* help category number            */
    short lenbuf;                 /* length of buffer                */
    short lenfld;                 /* length of screen input field    */
    short lenfmt;                 /* length of format string         */
    unsigned char wrow;         /* start of input - window row     */
    unsigned char wcol;         /* start of input - window column  */
    unsigned char mode;         /* 0=init, 1=update, 2=cond update */
    unsigned char decpos;       /* decimal position (numeric)      */
    unsigned char redisp;       /* redisplay flag                  */
    char fconv;                 /* field conversion character      */
};

/*-------------------[ window data entry form definition ]-------------------*/

struct _form_t {
    unsigned (*getkey) (short *); /* poshorter to alternate get func   */
    struct _form_t  *prev;      /* poshorter to previous form record */
    struct _form_t  *next;      /* poshorter to next form record     */
    struct _field_t *field;     /* poshorter to head field record    */
    struct _field_t *cfield;    /* poshorter to current field record */
    unsigned *termkey;          /* addr of short for terminating key */
    char *pformat;              /* format string poshorter           */
    char *pbuf;                 /* buffer string poshorter           */
    unsigned char cwrow;        /* current window row              */
    unsigned char cwcol;        /* current window column           */
    unsigned char decimal;      /* decimal field flag              */
    unsigned char insert;       /* insert mode flag                */
    unsigned char fieldattr;    /* field attribute                 */
    unsigned char textattr;     /* text attribute                  */
};

/*----------------------[ structure of window records ]----------------------*/

struct _wrec_t {
    struct _wrec_t *prev;   /* poshorter to previous window record */
    struct _wrec_t *next;   /* poshorter to next window record     */
    struct _form_t *form;   /* poshorter to head form record       */
    short *wbuf;            /* address of window's buffer        */
    short *wsbuf;           /* address of window shadow's buffer */
    char *title;            /* address of window's title string  */
    CXLWIN whandle;         /* window's handle                   */
    short help;               /* help category number              */
    unsigned char srow;     /* start row of window               */
    unsigned char scol;     /* start column of window            */
    unsigned char erow;     /* end row of window                 */
    unsigned char ecol;     /* end column of window              */
    unsigned char btype;    /* window's box type                 */
    unsigned char wattr;    /* window's initial text attribute   */
    unsigned char battr;    /* atrribute of window's border      */
    unsigned char border;   /* has border?  0 = no, 1 = yes      */
    unsigned char row;      /* window's current cursor row       */
    unsigned char column;   /* window's current cursor column    */
    unsigned char attr;     /* window's current text attribute   */
    unsigned char tpos;     /* position of window's title        */
    unsigned char tattr;    /* attribute of window's title       */
    unsigned char wsattr;   /* attribute of window's shadow      */
};

/*------------[ window title position definitions for wtitle() ]-------------*/

#define TLEFT       1   /* left justified  */
#define TCENTER     2   /* centered        */
#define TRIGHT      3   /* right justified */

/*------------------------[ definition of NULL ]-----------------------------*/

#if !defined(NULL)
    #if defined(__TURBOC__)                     /* Turbo C */
        #if defined(__TINY__) || defined(__SMALL__) || defined(__MEDIUM__)
            #define NULL    0
        #else
            #define NULL    0L
        #endif
    #elif defined(__ZTC__)                      /* Zortech C++ */
        #ifdef LPTR
            #define NULL    0L
        #else
            #define NULL    0
        #endif
    #elif defined(M_I86) && !defined(__ZTC__)   /* Microsoft C/QuickC */
        #if defined(M_I86SM) || defined(M_I86MM)
            #define NULL    0
        #else
            #define NULL    0L
        #endif
    #endif
#endif

/*---------------------------[ function prototypes ]-------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

char           *_Cdecl biosver(void);
short             _Cdecl clockcal(void);
char           *_Cdecl cxlver(void);
void            _Cdecl delay_(unsigned duration);
unsigned        _Cdecl expmem(void);
unsigned        _Cdecl extmem(void);
short             _Cdecl fcrypt(char *file,char *key);
short             _Cdecl gameport(void);
short             _Cdecl machid(void);
short             _Cdecl mathchip(void);
short             _Cdecl numflop(void);
short             _Cdecl numpar(void);
short             _Cdecl numser(void);
char           *_Cdecl randfile(void);
void            _Cdecl sound_(unsigned pitch,unsigned duration);
void            _Cdecl _stdoutch(short ch);
char           *_Cdecl sysdate(short dtype);
char           *_Cdecl systime(short ttype);
short             _Cdecl tabstop(short col,short tabwidth);

void     _Cdecl capsoff(void);
void     _Cdecl capson(void);
struct _onkey_t *_Cdecl chgonkey(struct _onkey_t *kblist);
void     _Cdecl freonkey(void);
short      _Cdecl getchf(char *valid,short defchar);
short      _Cdecl getns(char *str,short maxchars);
#if defined(__LINUX__)
short      _Cdecl kbhit(void);
#endif
unsigned _Cdecl getxch(void);
short      _Cdecl inputsf(char *str,char *fmt);
void     _Cdecl kbclear(void);
short      _Cdecl kbmhit(void);
short      _Cdecl kbput(unsigned xch);
short      _Cdecl kbputs(char *str);
unsigned short _Cdecl kbstat(void);
void     _Cdecl numoff(void);
void     _Cdecl numon(void);
short      _Cdecl scancode(short ch);
short      _Cdecl setonkey(unsigned keycode,void (_Cdecl *func) (void),
                         unsigned pass);
short      _Cdecl waitkey(void);
short      _Cdecl waitkeyt(short duration);

void       _Cdecl closevideo(void);
short      _Cdecl cvaltype(short ch,short ctype);
double   _Cdecl cvtcf(char *field,short wholesize,short fracsize);
short      _Cdecl cvtci(char *field);
void     _Cdecl cvtfc(char *field,double value,short wholesize,short fracsize);
void     _Cdecl cvtic(char *field,short value,short size);
short      _Cdecl strblank(char *str);
char    *_Cdecl strbmatch(char *str,char *strarr[]);
short      _Cdecl strchg(char *str,short oldch,short newch);
unsigned long _Cdecl strchksum(char *str);
char    *_Cdecl strcode(char *str,char *key);
char    *_Cdecl strdel(char *substr,char *str);
char    *_Cdecl strdela(char *substr,char *str);
short      _Cdecl strichg(char *str,short oldch,short newch);
unsigned long _Cdecl strichksum(char *str);
char    *_Cdecl stridel(char *substr,char *str);
char    *_Cdecl stridela(char *substr,char *str);
char    *_Cdecl striinc(char *str1,char *str2);
char    *_Cdecl strinc(char *str1,char *str2);
char    *_Cdecl strins(char *instr,char *str,short st_pos);
short      _Cdecl striocc(char *str,short ch);
char    *_Cdecl strischg(char *str,char *find,char *replace);
short      _Cdecl strisocc(char *str1,char *str2);
char    *_Cdecl strisrep(char *str,char *search,char *replace);
char    *_Cdecl strljust(char *str);
char    *_Cdecl strltrim(char *str);
short      _Cdecl strmatch(char *str1,char *str2);
char    *_Cdecl strmid(char *str,short st_pos,short num_chars);
short      _Cdecl strocc(char *str,short ch);
char    *_Cdecl strrjust(char *str);
char    *_Cdecl strrol(char *str,short count);
char    *_Cdecl strror(char *str,short count);
char    *_Cdecl strschg(char *str,char *find,char *replace);
char    *_Cdecl strsetsz(char *str,short newsize);
char    *_Cdecl strshl(char *str,short count);
char    *_Cdecl strshr(char *str,short count);
short      _Cdecl strsocc(char *str1,char *str2);
//char    *_Cdecl strsrep(char *str,char *search,char *replace);
char    *_Cdecl strtrim(char *str);
char    *_Cdecl struplow(char *str);
short      _Cdecl touplow(char *str,char *pos,short ch);

void     _Cdecl box_(short srow,short scol,short erow,short ecol,short btype,short attr);
void     _Cdecl cclrscrn(short attr);
short      _Cdecl cgardbyte(char *src);
short      _Cdecl cgardword(short *src);
void     _Cdecl cgareadn(short *src,short *dest,unsigned n);
void     _Cdecl cgawrbyte(char *dest,short chr);
void     _Cdecl cgawriten(short *src,short *dest,unsigned n);
void     _Cdecl cgawrstr(char *dest,char *string,short attr);
void     _Cdecl cgawrword(short *dest,short chratr);
void     _Cdecl clreol_(void);
void     _Cdecl clrscrn(void);
short      _Cdecl disktoscrn(char *fname);
short      _Cdecl disktowin(char *fname);
void     _Cdecl fill_(short srow,short scol,short erow,short ecol,short ch,short atr);
void     _Cdecl getcursz(short *sline,short *eline);
void     _Cdecl gotoxy_(short row,short col);
void     _Cdecl hidecur(void);
void     _Cdecl lgcursor(void);
short      _Cdecl mapattr(short attr);
void     _Cdecl mode(short mode_code);
void     _Cdecl printc(short row,short col,short attr,short ch);
void     _Cdecl prints(short row,short col,short attr,char *str);
void     _Cdecl putchat(short ch,short attr);
unsigned _Cdecl readchat(void);
void     _Cdecl readcur(short *row,short *col);
void     _Cdecl revattr(short count);
short      _Cdecl revsattr(short attr);
short      _Cdecl scrntodisk(char *fname);
void     _Cdecl setattr(short attr,short count);
void     _Cdecl setcursz(short sline,short eline);
short      _Cdecl setlines(short numlines);
short      _Cdecl setvparam(short setting);
void     _Cdecl showcur(void);
void     _Cdecl smcursor(void);
void     _Cdecl spc(short num);
void     _Cdecl srestore(short *sbuf);
short     *_Cdecl ssave(void);
short      _Cdecl videoinit(void);
void       _Cdecl videoupdate(void);
short      _Cdecl vidmode(void);
short      _Cdecl vidtype(void);
short      _Cdecl wshortodisk(short srow,short scol,short erow,short ecol,char *fname);

short      _Cdecl wactiv(CXLWIN whandle);
short      _Cdecl wborder(short btype);
short      _Cdecl wbox(short wsrow,short wscol,short werow,short wecol,short btype
                ,short attr);
short      _Cdecl wbprintc(short bord,short offs,short attr,short ch);
short      _Cdecl wcclear(short attr);
short      _Cdecl wcenters(short wrow,short attr,char *str);
short      _Cdecl wchgattr(short battr,short wattr);
short      _Cdecl wchkbox(short wsrow,short wscol,short werow,short wecol);
short      _Cdecl wchkcol(short wcol);
short      _Cdecl wchkcoord(short wrow,short wcol);
short      _Cdecl wchkrow(short wrow);
void       _Cdecl wclear(void);
short      _Cdecl wclose(void);
short      _Cdecl wcloseall(void);
short      _Cdecl wclreol(void);
short      _Cdecl wclreos(void);
CXLWIN   _Cdecl wcopy(short nsrow,short nscol);
short      _Cdecl wdelline(short wrow,short direc);
short      _Cdecl wdrag(short direction);
short      _Cdecl wdump(void);
short      _Cdecl wdupc(short ch,short count);
short      _Cdecl wdups(char *str,short count);
char    *_Cdecl werrmsg(void);
short      _Cdecl wfill(short wsrow,short wscol,short werow,short wecol,short ch,short attr);
struct   _wrec_t *_Cdecl wfindrec(CXLWIN whandle);
short      _Cdecl wgetc(void);
short      _Cdecl wgetchf(char *valid,short defchar);
short      _Cdecl wgetns(char *str,short maxchars);
short      _Cdecl wgets(char *str);
short      _Cdecl wgetyn(short cdefault);
short      _Cdecl wgotoxy(short wrow,short wcol);
CXLWIN   _Cdecl whandle(void);
short      _Cdecl whelpcat(short cat);
short      _Cdecl whelpclr(void);
short      _Cdecl whelpdef(char *file,unsigned key,short winattr,short textattr,
                         short selattr,short barattr,void (_Cdecl *open)(void));
short      _Cdecl whelpop(void);
short      _Cdecl whelpopc(void);
short      _Cdecl whelppcat(short cat);
short      _Cdecl whelpush(void);
short      _Cdecl whelpushc(short cat);
short      _Cdecl whelpwin(short srow,short scol,short erow,short ecol,short btype,
                         short title);
short      _Cdecl whide(void);
short      _Cdecl whline(short wsrow,short wscol,short count,short btype,short attr);
CXLWIN   _Cdecl windowat(short row,short col);
short      _Cdecl winpbeg(short fieldattr,short textattr);
short      _Cdecl winpdef(short wrow,short wcol,char *str,char *format,short fconv,short mode,short (*validate) (char *),short help);
short      _Cdecl winpread(void);
short      _Cdecl winputsf(char *str,char *fmt);
short      _Cdecl winsline(short wrow,short direc);
short      _Cdecl wmenubeg(short srow,short scol,short erow,short ecol,short btype,short battr,short wattr,void (*open)(void));
short      _Cdecl wmenubegc(void);
short      _Cdecl wmenuend(short taginit,short menutype,short barwidth,short textpos,
                         short textattr,short scharattr,short noselattr,short barattr);
short      _Cdecl wmenuget(void);
struct   _item_t *_Cdecl wmenuicurr(void);
short      _Cdecl wmenuidsab(short tagid);
short      _Cdecl wmenuienab(short tagid);
struct   _item_t *_Cdecl wmenuifind(short tagid);
short      _Cdecl wmenuinext(short tagid);
short      _Cdecl wmenuitem(short wrow,short wcol,char *str,short schar,short tagid,short fmask,void (*select)(void),unsigned hotkey,short help);
short      _Cdecl wmenuitxt(short wrow,short wcol,short attr,char *str);
short      _Cdecl wmessage(char *str,short border,short leftofs,short attr);
short      _Cdecl wmove(short nsrow,short nscol);
CXLWIN   _Cdecl wopen(short srow,short scol,short erow,short ecol,short btype,short battr,
                      short wattr);
short      _Cdecl wperror(char *message);
short      _Cdecl wpgotoxy(short wrow,short wcol);
char    *_Cdecl wpickfile(short srow,short scol,short erow,short ecol,short btype,
                          short bordattr,short winattr,short barattr,short title,
                          char *filespec,void (*open)(void));
short      _Cdecl wpickstr(short srow,short scol,short erow,short ecol,short btype,
                         short bordattr,short winattr,short barattr,char *strarr[],
                         short initelem,void (*open)(void));
short      _Cdecl wprintc(short wrow,short wcol,short attr,short ch);
short      _Cdecl wprintf(const char *format,...);
short      _Cdecl wprints(short wrow,short wcol,short attr,char *str);
short      _Cdecl wprintsf(short wrow,short wcol,short attr,char *format,char *str);
short      _Cdecl wputc(short ch);
short      _Cdecl wputns(char *str,short width);
short      _Cdecl wputs(char *str);
short      _Cdecl wputsw(char *str);
short      _Cdecl wreadcur(short *wrow,short *wcol);
void     _Cdecl wrestore(short *wbuf);
short      _Cdecl wrjusts(short wrow,short wjcol,short attr,char *str);
short   *_Cdecl wsave(short srow,short scol,short erow,short ecol);
short      _Cdecl wscanf(const char *format,...);
short      _Cdecl wscroll(short count,short direc);
short      _Cdecl wscrollbox(short wsrow,short wscol,short werow,short wecol,short count,
                           short direction);
short      _Cdecl wselstr(short wrow,short wcol,short attr,char *strarr[],
                        short initelem);
short      _Cdecl wshadoff(void);
short      _Cdecl wshadow(short attr);
short      _Cdecl wsize(short nerow,short necol);
short      _Cdecl wslide(short nsrow,short nscol);
short      _Cdecl wtextattr(short attr);
short      _Cdecl wtitle(char *str,short tpos,short tattr);
short      _Cdecl wunhide(CXLWIN whandle);
short      _Cdecl wunlink(CXLWIN w);
short      _Cdecl wvline(short wsrow,short wscol,short count,short btype,short attr);
short      _Cdecl wwprints(CXLWIN whandle,short wrow,short wcol,short attr,char *str);

#ifdef __cplusplus
}
#endif

/*-----------------------[ macro-function definitions ]----------------------*/

#undef MK_FP
#if defined(__DOS__)
#if defined (__386__)
#define MK_FP(seg,ofs)      ((void *) (((unsigned long)(seg) << 4) | (unsigned)(ofs)))
#else
#define MK_FP(seg,ofs)      ((void *) (((unsigned long)(seg) << 16) | (unsigned)(ofs)))
#endif
#elif defined(__LINUX__) || defined(__OS2__) || defined(__NT__)
#define MK_FP(seg,ofs)      ((void *) (seg + ofs))
#endif
#define attrib(f,b,i,bl)    ((b<<4)|(f)|(i<<3)|(bl<<7))
#define clrwin(a,b,c,d)     gotoxy_(a,b);fill_(a,b,c,d,' ',(readchat()>>8))

#define strbtrim(st)        strtrim(strltrim(st))
#define strleft(st,nu)      strmid(st,0,nu)
#define strright(st,nu)     strmid(st,strlen(st)-nu,nu)
#define strrtrim(st)        strtrim(st)

#define wfillch(a)          (_winfo.fillch=a)
#define whelpundef()        whelpdef(NULL,0,0,0,0,0,NULL)
#define winpfcurr()         (_winfo.active->form->cfield)
#define wisactiv(a)         (a==_winfo.active->whandle)
#define wmenumcurr()        (_winfo.cmenu)
#define wsetesc(a)          (_winfo.esc=a)
#define wtabwidth(a)        (_winfo.tabwidth=((a==0)?1:a))

#endif


