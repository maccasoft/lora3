/*
   旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
   �                                                                          �
   �  CXL is Copyright (c) 1987-1989 by Mike Smedley.                         �
   �                                                                          �
   읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
*/

/*-----------------[ definitions for common control codes ]------------------*/

#define NUL  0x00
#define BEL  0x07
#define BS   0x08
#define HT   0x09
#define LF   0x0a
#define FF   0x0c
#define CR   0x0d
#define ESC  0x1b

/*-----------------------[ Boolean logic conditions ]------------------------*/

#define NO      0
#define YES     1
#define FALSE   0
#define TRUE    1
#define OFF     0
#define ON      1

/*----------------[ machine ID codes returned from machid() ]----------------*/

#define IBMPC       255     /* IBM PC             */
#define IBMPCXT     254     /* IBM PC/XT          */
#define IBMPCJR     253     /* IBM PCjr           */
#define IBMPCAT     252     /* IBM PC/AT          */
#define IBMPCXT2    251     /* IBM PC/XT          */
#define IBMCONV     249     /* IBM PC Convertible */
#define SPERRYPC    48      /* Sperry PC          */

/*-------------[ display adapter types returned from vidtype() ]-------------*/

#define V_NONE      0
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
    unsigned short    videoseg;   /* video buffer segment address */
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

/*------------------------------[ WINDOW type ]------------------------------*/

typedef short     WINDOW;

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
    struct _item_t *prev;   /* pointer to previous record      */
    struct _item_t *next;   /* pointer to next record          */
    void *child;            /* pointer to submenu's record     */
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
    struct _menu_t *prev;       /* pointer to prev menu structure   */
    struct _menu_t *next;       /* pointer to next menu structure   */
    struct _menu_t *parent;     /* pointer to parent menu           */
    struct _item_t *item;       /* pointer to head menu item        */
    struct _item_t *citem;      /* pointer to current menu item     */
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
    short helpptr;                /* help stack pointer         */
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
    void (*open) (void);        /* pointer to open function   */
};

/*---------------------[ window input field definition ]---------------------*/

struct _field_t {
    struct _field_t *prev;      /* pointer to previous field       */
    struct _field_t *next;      /* pointer to next field           */
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
    unsigned (*getkey) (short *); /* pointer to alternate get func   */
    struct _form_t  *prev;      /* pointer to previous form record */
    struct _form_t  *next;      /* pointer to next form record     */
    struct _field_t *field;     /* pointer to head field record    */
    struct _field_t *cfield;    /* pointer to current field record */
    unsigned *termkey;          /* addr of int for terminating key */
    char *pformat;              /* format string pointer           */
    char *pbuf;                 /* buffer string pointer           */
    unsigned char cwrow;        /* current window row              */
    unsigned char cwcol;        /* current window column           */
    unsigned char decimal;      /* decimal field flag              */
    unsigned char insert;       /* insert mode flag                */
    unsigned char fieldattr;    /* field attribute                 */
    unsigned char textattr;     /* text attribute                  */
};

/*----------------------[ structure of window records ]----------------------*/

struct _wrec_t {
    struct _wrec_t *prev;   /* pointer to previous window record */
    struct _wrec_t *next;   /* pointer to next window record     */
    struct _form_t *form;   /* pointer to head form record       */
    short *wbuf;              /* address of window's buffer        */
    short *wsbuf;             /* address of window shadow's buffer */
    char *title;            /* address of window's title string  */
    WINDOW whandle;         /* window's handle                   */
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

/*-----------------------[ window information record ]-----------------------*/

struct _winfo_t {
    struct _wrec_t  *active;    /* pointer to active window         */
    struct _wrec_t  *hidden;    /* pointer to head hidden window    */
    struct _menu_t  *menu;      /* pointer to head menu record      */
    struct _menu_t  *cmenu;     /* pointer to current menu record   */
    struct _help_t  *helptr;    /* pointer to help info record      */
    WINDOW handle;              /* last handle given to a window    */
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

/*---------------------------[ function prototypes ]-------------------------*/

char *biosver(void);
short clockcal(void);
char *cxlver(void);
void delay_(unsigned duration);
unsigned expmem(void);
unsigned extmem(void);
short fcrypt(char *file,char *key);
short gameport(void);
short machid(void);
short mathchip(void);
short numflop(void);
short numpar(void);
short numser(void);
char *randfile(void);
void sound_(unsigned pitch,unsigned duration);
void _stdoutch(short ch);
char *sysdate(short dtype);
char *systime(short ttype);
short tabstop(short col,short tabwidth);
unsigned long timer(void);
void box_(short srow,short scol,short erow,short ecol,short btype,short attr);
void cclrscrn(short attr);
short cgardbyte(char far *src);
short cgardword(short far *src);
void cgareadn(short far *src,short *dest,unsigned n);
void cgawrbyte(char far *dest,short chr);
void cgawriten(short *src,short far *dest,unsigned n);
void cgawrstr(char far *dest,char *string,short attr);
void cgawrword(short far *dest,short chratr);
void clreol_(void);
void clrscrn(void);
short disktoscrn(char *fname);
short disktowin(char *fname);
void fill_(short srow,short scol,short erow,short ecol,short ch,short atr);
void getcursz(short *sline,short *eline);
void gotoxy_(short row,short col);
void hidecur(void);
void lgcursor(void);
short mapattr(short attr);
void mode(short mode_code);
void printc(short row,short col,short attr,short ch);
void prints(short row,short col,short attr,char *str);
void printsf(short row,short col,short attr,const char *format,...);
void putchat(short ch,short attr);
unsigned readchat(void);
void readcur(short *row,short *col);
void revattr(short count);
short revsattr(short attr);
short scrntodisk(char *fname);
void setattr(short attr,short count);
void setcursz(short sline,short eline);
short setlines(short numlines);
short setvparam(short setting);
void showcur(void);
void smcursor(void);
void spc(short num);
void srestore(short *sbuf);
short *ssave(void);
short videoinit(void);
short vidmode(void);
short vidtype(void);
short wintodisk(short srow,short scol,short erow,short ecol,char *fname);
short wactiv(WINDOW whandle);
short wborder(short btype);
short wbox(short wsrow,short wscol,short werow,short wecol,short btype,short attr);
short wbprintc(short bord,short offs,short attr,short ch);
short wcclear(short attr);
short wcenters(short wrow,short attr,char *str);
short wchgattr(short battr,short wattr);
short wchkbox(short wsrow,short wscol,short werow,short wecol);
short wchkcol(short wcol);
short wchkcoord(short wrow,short wcol);
short wchkrow(short wrow);
short wclose(void);
short wcloseall(void);
short wclreol(void);
short wclreos(void);
WINDOW wcopy(short nsrow,short nscol);
short wdelline(short wrow,short direc);
short wdrag(short direction);
short wdump(void);
short wdupc(short ch,short count);
short wdups(char *str,short count);
char *werrmsg(void);
short wfill(short wsrow,short wscol,short werow,short wecol,short ch,short attr);
struct _wrec_t *wfindrec(WINDOW whandle);
short wgetc(void);
short wgetchf(char *valid,short defchar);
short wgetns(char *str,short maxchars);
short wgets(char *str);
short wgetyn(short cdefault);
short wgotoxy(short wrow,short wcol);
WINDOW whandle(void);
short whelpcat(short cat);
short whelpclr(void);
short whelpdef(char *file,unsigned key,short winattr,short textattr,short selattr,short barattr,void (*open)(void));
short whelpop(void);
short whelpopc(void);
short whelppcat(short cat);
short whelpush(void);
short whelpushc(short cat);
short whelpwin(short srow,short scol,short erow,short ecol,short btype,short title);
short whide(void);
short whline(short wsrow,short wscol,short count,short btype,short attr);
WINDOW windowat(short row,short col);
short winpbeg(short fieldattr,short textattr);
short winpdef(short wrow,short wcol,char *str,char *format,short fconv,short mode,short (*validate) (char *),short help);
struct _field_t *winpffind(short wrow,short wcol);
short winpread(void);
short winputsf(char *str,char *fmt);
short winsline(short wrow,short direc);
short wmenubeg(short srow,short scol,short erow,short ecol,short btype,short battr,short wattr,void (*open)(void));
short wmenubegc(void);
short wmenuend(short taginit,short menutype,short barwidth,short textpos,short textattr,short scharattr,short noselattr,short barattr);
short wmenuget(void);
short wmenuidsab(short tagid);
short wmenuienab(short tagid);
struct _item_t *wmenuifind(short tagid);
short wmenuinext(short tagid);
short wmenuitem(short wrow,short wcol,char *str,short schar,short tagid,short fmask,void (*select)(void),unsigned hotkey,short help);
short wmenuitxt(short wrow,short wcol,short attr,char *str);
short wmessage(char *str,short border,short leftofs,short attr);
short wmove(short nsrow,short nscol);
WINDOW wopen(short srow,short scol,short erow,short ecol,short btype,short battr,short wattr);
short wperror(char *message);
short wpgotoxy(short wrow,short wcol);
char *wpickfile(short srow,short scol,short erow,short ecol,short btype,short bordattr,short winattr,short barattr,short title,char *filespec,void (*open)(void));
short wpickstr(short srow,short scol,short erow,short ecol,short btype,short bordattr,short winattr,short barattr,char *strarr[],short initelem,void (*open)(void));
short wprintc(short wrow,short wcol,short attr,short ch);
short wprintf(const char *format,...);
short wprints(short wrow,short wcol,short attr,char *str);
short wprintsf(short wrow,short wcol,short attr,const char *format,...);
short wputc(short ch);
short wputns(char *str,short width);
short wputs(char *str);
short wputsw(char *str);
short wreadcur(short *wrow,short *wcol);
void wrestore(short *wbuf);
short wrjusts(short wrow,short wjcol,short attr,char *str);
short *wsave(short srow,short scol,short erow,short ecol);
short wscanf(const char *format,...);
short wscroll(short count,short direc);
short wscrollbox(short wsrow,short wscol,short werow,short wecol,short count,short direction);
short wselstr(short wrow,short wcol,short attr,char *strarr[],short initelem);
short wshadoff(void);
short wshadow(short attr);
short wsize(short nerow,short necol);
short wslide(short nsrow,short nscol);
short wtextattr(short attr);
short wtitle(char *str,short tpos,short tattr);
short wunhide(WINDOW whandle);
short wunlink(WINDOW w);
short wvline(short wsrow,short wscol,short count,short btype,short attr);
short wwprints(WINDOW whandle,short wrow,short wcol,short attr,char *str);

/*------------[ window title position definitions for wtitle() ]-------------*/

#define TLEFT       1   /* left justified  */
#define TCENTER     2   /* centered        */
#define TRIGHT      3   /* right justified */

/*-----------------------[ macro-function definitions ]-----------------------*/

#if !defined (MKFP)
#ifdef __386__
#define MKFP(seg,ofs)      ((void *) (((unsigned long)(seg) << 4) | (unsigned)(ofs)))
#else
#define MKFP(seg,ofs)      ((void *) (((unsigned long)(seg) << 16) | (unsigned)(ofs)))
#endif
#endif

#define beep()              _stdoutch(BEL)
#define attrib(f,b,i,bl)    ((b<<4)|(f)|(i<<3)|(bl<<7))
#define clrwin(a,b,c,d)     gotoxy_(a,b);fill_(a,b,c,d,' ',(readchat()>>8))
#define wclear()            wcclear(_winfo.active->wattr)
#define wfillch(a)          (_winfo.fillch=a)
#define whelpundef()        whelpdef(NULL,0,0,0,0,0,NULL)
#define winpfcurr()         (_winfo.active->form->cfield)
#define wisactiv(a)         (a==_winfo.active->whandle)
#define wmenumcurr()        (_winfo.cmenu)
#define wmenuicurr()        (wmenumcurr()->citem)
#define wsetesc(a)          (_winfo.esc=a)
#define wtabwidth(a)        (_winfo.tabwidth=((a==0)?1:a))




