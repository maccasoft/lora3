
// ----------------------------------------------------------------------
// Lora BBS Professional Edition - Version 0.13
// Copyright (c) 1995 by Marco Maccaferri. All rights reserved.
//
// History:
//    05/20/95 - Initial coding.
// ----------------------------------------------------------------------

#include "_ldefs.h"
#include "cxl.h"

#if defined(__OS2__)
typedef PBYTE  PBYTE16;
#endif

#define     MAXCHARS    255

#define HORZ  0
#define VERT  1

#define ULC   bt[0]    /* upper left corner         */
#define UHL   bt[1]    /* upper horizontal line     */
#define URC   bt[2]    /* upper right corner        */
#define LVL   bt[3]    /* left vertical line        */
#define RVL   bt[4]    /* right vertical line       */
#define LLC   bt[5]    /* lower left corner         */
#define LHL   bt[6]    /* lower horizontal line     */
#define LRC   bt[7]    /* lower right corner        */
#define MJ    bt[8]    /* middle junction           */
#define LVJ   bt[9]    /* left vertical junction    */
#define RVJ   bt[10]   /* right vertical junction   */
#define UHJ   bt[11]   /* upper horizontal junction */
#define LHJ   bt[12]   /* lower horizontal junction */

struct _winfo_t _winfo = {
   NULL,       /* pointer to active window         */
   NULL,       /* pointer to last hidden window    */
   NULL,       /* pointer to head menu record      */
   NULL,       /* pointer to current menu record   */
   NULL,       /* pointer to help info record      */
   0,          /* last handle given to a window    */
   500,        /* max files allowed in wpickfile() */
   0,          /* pointer to current help category */
   W_NOERROR,  /* error num from last window func  */
   0,          /* total number of open windows     */
   0, 0,       /* system variables used in menus   */
   ON,         /* check for Esc in input funcions? */
   8,          /* window TTY output tab width      */
   ' '         /* character to fill windows with   */
};

struct _vinfo_t _vinfo = {
   0xb800,  /* video buffer segment address */
   V_CGA,   /* video adapter type           */
   25,      /* number of displayed rows     */
   80,      /* number of displayed columns  */
   8,       /* character height in pixels   */
   8,       /* character width  in pixels   */
   NO,      /* is it a monochrome adapter?  */
   NO,      /* map color attribs to mono?   */
   NO,      /* is CGA snow present?         */
   NO,      /* use BIOS for video writes?   */
   YES,     /* check for DESQview?          */
   NO       /* is DESQview present?         */
};

char *_box_table[] = {
    "ÚÄ¿³³ÀÄÙÅÃ´ÂÁ",    /* box type 0 */
    "ÉÍ»ººÈÍ¼ÎÌ¹ËÊ",    /* box type 1 */
    "ÖÄ·ººÓÄ½×Ç¶ÒÐ",    /* box type 2 */
    "ÕÍ¸³³ÔÍ¾ØÆµÑÏ",    /* box type 3 */
    "ÜÜÜÝÞßßßÝÝÝÝÝ",    /* box type 4 */
    "             "     /* box type 5 */
};

static struct _wrec_t *curr,*found;
static short crow,ccol;
static short sline=6,eline=7;

static short   bshadow_blocking(void);
static short  *calc_bshadow(struct _wrec_t *wrec);
static short  *calc_rshadow(struct _wrec_t *wrec);
static short  *calc_window(struct _wrec_t *wrec);
static short   rshadow_blocking(void);
static void  swap_contents(short *pfound,short *pcurr,short shadow);
static short   window_blocking(void);
static short disp_char(short wrow,short wcol,short attr,short btype,short ch,short direc);
static short isupvert(short btype,short c);
static short isdownvert(short btype,short c);
static short islefthorz(short btype,short c);
static short isrighthorz(short btype,short c);
static short read_char(short wrow,short wcol);

void _stdoutch(short ch)
{
#if defined(__OS2__) || defined(__NT__)
   printf ("%c", ch);
#else
    union REGS regs;

    regs.h.ah=2;
    regs.h.dl=(unsigned char)ch;
#ifdef __386__
    int386(0x21,&regs,&regs);
#else
    int86(0x21,&regs,&regs);
#endif
#endif
}

void box_ (short srow, short scol, short erow, short ecol, short boxtype, short attr)
{
   short i,j;
   short crow,ccol,wide,tall;
   extern char *_box_table[];

   /* check for monochrome adapter, adjust attribute */
   attr = mapattr (attr);

   /* calculate width and height */
   wide = (short)(ecol - scol - 1);
   tall = (short)(erow - srow - 1);

   /* display top and bottom horizontal borders */
   ccol = (short)(scol + 1);
   for (j = 0; j < wide; j++, ccol++) {
      printc (srow, ccol, attr, _box_table[boxtype][1]);
      printc (erow, ccol, attr, _box_table[boxtype][6]);
   }

   /* display left and right vertical borders */
   crow = (short)(srow + 1);
   for (i = 0; i < tall; i++, crow++) {
      printc (crow, scol, attr, _box_table[boxtype][3]);
      printc (crow, ecol, attr, _box_table[boxtype][4]);
   }

   /* display corners */
   printc (srow, scol, attr, _box_table[boxtype][0]);
   printc (srow, ecol, attr, _box_table[boxtype][2]);
   printc (erow, scol, attr, _box_table[boxtype][5]);
   printc (erow, ecol, attr, _box_table[boxtype][7]);
}

/* this function detects if a given window's bottom shadow is blocking */
/* the current window or its shadow at specified coordinates           */
static short bshadow_blocking(void)
{
    short isblocking=NO;

    if(crow==(curr->erow+1))
        if((ccol>=(curr->scol+2))&&(ccol<=(curr->ecol+2)))
            isblocking=YES;
    return(isblocking);
}

static short *calc_bshadow(struct _wrec_t *wrec)
{
    return(wrec->wsbuf+((((crow-wrec->srow-1)*2)+(ccol-wrec->scol-2))));
}

static short *calc_rshadow(struct _wrec_t *wrec)
{
    return(wrec->wsbuf+((((crow-wrec->srow-1)*2)+(ccol-wrec->ecol-1))));
}

static short *calc_window(struct _wrec_t *wrec)
{
    return(wrec->wbuf+4+((crow-wrec->srow)*(wrec->ecol-wrec->scol+1))+
           (ccol-wrec->scol));
}

void cclrscrn (short attr)
{
#if defined(__OS2__)
   short chat;

   attr = mapattr (attr);
   chat = (short)((attr << 8) | (' ' & 0x00ff));
   VioWrtNCell ((PCH)&chat, (USHORT)(80*25), 0, 0, 0);
#else
   union REGS regs;

   regs.h.bh = (unsigned char)mapattr (attr);
#ifdef __386__
   regs.w.cx = 0;
#else
   regs.x.cx = 0;
#endif
   regs.h.dh = (unsigned char)(_vinfo.numrows - 1);
   regs.h.dl = (unsigned char)(_vinfo.numcols - 1);
#ifdef __386__
   regs.w.ax = 0x0600;          /*  clear screen by scrolling it  */
#else
   regs.x.ax = 0x0600;          /*  clear screen by scrolling it  */
#endif
#ifdef __386__
    int386(0x10,&regs,&regs);
#else
    int86(0x10,&regs,&regs);
#endif
#endif
   gotoxy_ (0, 0);
}

void clrscrn (void)
{
#if defined(__OS2__)
   short attr, row, col, len;

   VioGetCurPos ((PUSHORT)&row, (PUSHORT)&col, 0);
   len = 2;
   VioReadCellStr ((PCH)&attr, (PUSHORT)len, row, col, 0);
   cclrscrn ((short)(attr >> 8));
#else
   union REGS regs;

   regs.h.bh = 0;
   regs.h.ah = 8;               /*  get character/attribute under cursor  */
#ifdef __386__
    int386(0x10,&regs,&regs);
#else
    int86(0x10,&regs,&regs);
#endif
   cclrscrn (regs.h.ah);
#endif
}

void clreol_ (void)
{
#if defined(__OS2__)
   short row, col;

   VioGetCurPos ((PUSHORT)&row, (PUSHORT)&col, 0);
   spc ((short)(_vinfo.numcols - col));
#else
   union REGS regs;

   regs.h.bh = 0;
   regs.h.ah = 3;                              /*  BIOS read cursor position  */
#ifdef __386__
    int386(0x10,&regs,&regs);
#else
    int86(0x10,&regs,&regs);
#endif
   spc ((short)(_vinfo.numcols - regs.h.dl));
#endif                           
}

static short disp_char(short wrow,short wcol,short attr,short btype,short ch,short direc)
{
    char *bt;
    short col;
    short row;

    /* see if next to a border, if so, connect to it */
    if(_winfo.active->border) {

        /* abbreviate pointer */
        bt=_box_table[btype];

        /* calculate effective row and column */
        row=(short)(_winfo.active->srow+_winfo.active->border+wrow);
        col=(short)(_winfo.active->scol+_winfo.active->border+wcol);

        /* see if this is a horizontal or vertical line */
        if(direc==HORZ) {

            /* make sure that the box type characters match */
            if(LVL==_box_table[_winfo.active->btype][3]) {

                /* check left border */
                if(col==(_winfo.active->scol+1)) {
                    printc(row,_winfo.active->scol,attr,LVJ);
                    ch=UHL;
                }

                /* check right border */
                if(col==(_winfo.active->ecol-1)) {
                    printc(row,_winfo.active->ecol,attr,RVJ);
                    ch=UHL;
                }
            }
        }
        else {

            /* make sure that the box type characters match */
            if(UHL==_box_table[_winfo.active->btype][1]) {

                /* check top border */
                if(row==(_winfo.active->srow+1)) {
                    printc(_winfo.active->srow,col,attr,UHJ);
                    ch=LVL;
                }

                /* check bottom border */
                if(row==(_winfo.active->erow-1)) {
                    printc(_winfo.active->erow,col,attr,LHJ);
                    ch=LVL;
                }
            }
        }
    }

    /* display character */
    if(wprintc(wrow,wcol,attr,ch)) return(_winfo.error);

    /* return normally */
    return(0);
}

void fill_(short srow,short scol,short erow,short ecol,short ch,short attr)
{
#if defined(__OS2__)
    short crow,chat;

    /* convert attribute if monochrome adapter */
    attr=mapattr(attr);
    chat=(short)((attr<<8)|(ch&0x00ff));

    for(crow=srow;crow<=erow;crow++)
        VioWrtNCell ((PCH)&chat, (USHORT)(ecol-scol+1), crow, scol, 0);
#else
    short crow,ccol;
    short incr,chat;
    short *p;

    /* check for monochrome adapter, adjust attribute */
    attr=mapattr(attr);

    /* calculate video RAM address */
    p=(short *)MKFP(_vinfo.videoseg,(srow*_vinfo.numcols+scol)*2);
    incr=(short)(_vinfo.numcols-ecol+scol-1);
    for(crow=srow;crow<=erow;crow++) {
        for(ccol=scol;ccol<=ecol;ccol++) {
            chat=(short)((attr<<8)|(ch&0x00ff));
            *p=chat;
            p++;
        }
        p=p+incr;
    }
#endif
}

void getcursz(int *start_line,int *end_line)
{
#if defined(__OS2__)
    VIOCURSORINFO vci;

    VioGetCurType (&vci, 0);
    *start_line = (int)vci.yStart;
    *end_line = (int)vci.cEnd;
#else
    union REGS regs;

    regs.h.bh=0;
    regs.h.ah=3;                        /* BIOS get cursor scan lines */
#ifdef __386__
    int386(0x10,&regs,&regs);
#else
    int86(0x10,&regs,&regs);
#endif
    *start_line=regs.h.ch;
    *end_line=regs.h.cl;
#endif
}

void gotoxy_ (short row, short column)
{
#if defined(__OS2__)
   VioSetCurPos (row, column, 0);
#else
   union REGS regs;

   regs.h.dl = (unsigned char)column;
   regs.h.dh = (unsigned char)row;
   regs.h.bh = 0;
   regs.h.ah = 2;                    /* BIOS cursor address function */
#ifdef __386__
    int386(0x10,&regs,&regs);
#else
    int86(0x10,&regs,&regs);
#endif
#endif
}

void hidecur(void)
{
    int sl,el;

    getcursz(&sl,&el);
    if(!(sl&48)) {
        sline=(short)sl;
        eline=(short)el;
        setcursz((_vinfo.adapter>=V_HGC&&_vinfo.adapter<=V_INCOLOR)?63:48,0);
    }
}

static short isdownvert(short btype,short c)
{
    char *bt;
    char ch;

    bt=_box_table[btype];
    ch=(char)c;
    return((ch==LVL||ch==LHJ||ch==LLC||ch==LRC||ch==LVJ||ch==RVJ||ch==MJ)
           ?YES:NO);
}

static short islefthorz(short btype,short c)
{
    char *bt;
    char ch;

    bt=_box_table[btype];
    ch=(char)c;
    return((ch==UHL||ch==LVJ||ch==LLC||ch==ULC||ch==UHJ||ch==LHJ||ch==MJ)
           ?YES:NO);
}

static short isrighthorz(short btype,short c)
{
    char *bt;
    char ch;

    bt=_box_table[btype];
    ch=(char)c;
    return((ch==UHL||ch==RVJ||ch==LRC||ch==URC||ch==UHJ||ch==LHJ||ch==MJ)
           ?YES:NO);
}

static short isupvert(short btype,short c)
{
    char *bt;
    char ch;

    bt=_box_table[btype];
    ch=(char)c;
    return((ch==LVL||ch==UHJ||ch==ULC||ch==URC||ch==LVJ||ch==RVJ||ch==MJ)
        ?YES:NO);
}

short mapattr (short attr)
{
   if (_vinfo.mapattr) {             /* if monochrome mapping is on */
      switch (attr & 112) {          /* test for a light background */
         case _LGREY:
         case _GREEN:
         case _CYAN:
         case _BROWN:
            attr = (short)(attr & 240);       /* foreground = black */
            attr = (short)(attr | 112);       /* background = light grey */
            break;
         default:
            if ((attr & 15) == 8)    /* if foreground = dark grey */
               attr = (short)(attr & 247);    /* clear intensity bit */
            attr = (short)(attr | 7);         /* foreground = light grey */
            attr = (short)(attr & 143);       /* background = black */
      }
   }
   return (attr);                    /* return converted attribute */
}

void printc (short row, short col, short attr, short ch)
{
#if defined(__OS2__)
   short chat;

   attr = mapattr (attr);
   chat = (short)((attr << 8) | (ch & 0x00ff));
   VioWrtCellStr ((PCH)&chat, 2, row, col, 0);
#else
   short *p, chat;

   /* check for monochrome adapter, adjust attribute */
   attr = mapattr (attr);
   /* calculate video buffer offset and write character/attribute */
   p = (short *)MKFP (_vinfo.videoseg, ((row * _vinfo.numcols) + col) * 2);
   chat = (short)((attr << 8) | (ch & 0x00ff));
   *p = chat;
#endif
}

void prints(short row,short col,short attr,char *str)
{
#if defined(__OS2__)
    /* convert attribute if monochrome adapter */
    attr=mapattr(attr);

    /* do while more characters */
    VioWrtCharStrAtt ((PCH)str, (USHORT)strlen (str), row, col, (PBYTE)&attr, 0);
#else
    char *q;
    char *p;

    /* convert attribute if monochrome adapter */
    attr=mapattr(attr);

    /* calculate video RAM address */
    p=(char *)MKFP(_vinfo.videoseg,((row*_vinfo.numcols)+col)*2);

    /* do while more characters */
    for(q=str;*q;q++) {
        /* write character and attribute */
        *p++=*q;
        *p++=(char)attr;
    }
#endif
}

void printsf(short row,short col,short attr,const char *format,...)
{
#if defined(__OS2__)
    va_list argptr;
    char buf[MAXCHARS];

    /* format string using specified parameters into buffer */
    va_start(argptr,format);            /* access argument list */
    vsprintf(buf,format,argptr);        /* create string using argument list */
    va_end(argptr);                     /* end access of argument list */

    /* convert attribute if monochrome adapter */
    attr=mapattr(attr);

    /* do while more characters */
    VioWrtCharStrAtt ((PCH)buf, (USHORT)strlen (buf), row, col, (PBYTE)&attr, 0);
#else
    va_list argptr;
    char buf[MAXCHARS],*q,*p;

    /* format string using specified parameters into buffer */
    va_start(argptr,format);            /* access argument list */
    vsprintf(buf,format,argptr);        /* create string using argument list */
    va_end(argptr);                     /* end access of argument list */

    /* convert attribute if monochrome adapter */
    attr=mapattr(attr);

    /* calculate video RAM address */
    p=(char *)MKFP(_vinfo.videoseg,((row*_vinfo.numcols)+col)*2);

    /* do while more characters */
    for(q=buf;*q;q++) {
        /* write character and attribute */
        *p++=*q;
        *p++=(char)attr;
    }
#endif
}

static char *process_esc(char *str)
{
    short attr;
    char *p;
    short ch,wrow,wcol;

    attr=_winfo.active->attr;
    for(p=str;*p==ESC;p++) {
        switch(*++p) {
            case '+':   /* increase text attribute */
                wtextattr(++attr);
                break;
            case '-':   /* decrease text attribute */
                wtextattr(--attr);
                break;
            case 'A':   /* change attribute */
                wtextattr(*++p);
                break;
            case 'F':   /* change foreground attribute */
                wtextattr((short)((*++p&7)|(attr&248)));
                break;
            case 'B':   /* change background attribute */
                wtextattr((short)((*++p&112)|(attr&143)));
                break;
            case 'I':   /* toggle intensity bit */
                wtextattr((short)((attr&8)?(attr&247):(attr|8)));
                break;
            case 'L':   /* toggle blinking bit */
                wtextattr((short)((attr&128)?(attr&127):(attr|128)));
                break;
            case 'X':   /* reverse attribute */
                wtextattr(revsattr(attr));
                break;
            case 'R':   /* set cursor row */
                wreadcur(&wrow,&wcol);
                wgotoxy(*++p,wcol);
                break;
            case 'C':   /* set cursor column */
                wreadcur(&wrow,&wcol);
                wgotoxy(wrow,*++p);
                break;
            case 'E':   /* erase */
                switch(*++p) {
                    case 'W':   /* erase window */
                        wclear();
                        break;
                    case 'S':   /* erase to end of window */
                        wclreos();
                        break;
                    case 'L':   /* erase to end of window's line */
                        wclreol();
                        break;
                }
                break;
            case 'D':   /* duplicate character */
                ch=*++p;
                wdupc(ch,*++p);
                break;
            default:
                p--;
        }
    }
    return(--p);
}

static short read_char(short wrow,short wcol)
{
    short ch;
#if defined(__OS2__)
    short len;

    len = 2;
    VioReadCellStr ((PCH)&ch, (PUSHORT)&len, (USHORT)(_winfo.active->srow+wrow+_winfo.active->border), (USHORT)(_winfo.active->scol+wcol+_winfo.active->border), 0);
#else
    char *p;

    /* calculate video RAM address */
    p=(char *)MKFP(_vinfo.videoseg,(((_winfo.active->srow+wrow+_winfo.active->border)*_vinfo.numcols)+(_winfo.active->scol+wcol+_winfo.active->border))*2);
    ch=*p;
#endif
    return((short)(ch&0x00ff));
}

void readcur (short *row, short *column)
{
#if defined(__OS2__)
    VioGetCurPos ((PUSHORT)row, (PUSHORT)column, 0);
#else
    union REGS regs;

    regs.h.bh = 0;
    regs.h.ah = 3;                        /* BIOS get cursor coordinates */
#ifdef __386__
    int386(0x10,&regs,&regs);
#else
    int86(0x10,&regs,&regs);
#endif
    *row = regs.h.dh;
    *column = regs.h.dl;
#endif
}

short revsattr(short attr)
{
    return((short)(((attr>>4)&0x07)|((attr<<4)&0x70)|(attr&0x80)|(attr&0x08)));
}

/* this function detects if a given window's right shadow is blocking */
/* the current window or its shadow at specified coordinates          */
static short rshadow_blocking(void)
{
    short isblocking=NO;

    if(ccol==(curr->ecol+1)||ccol==(curr->ecol+2))
        if((crow>=(curr->srow+1))&&(crow<=curr->erow))
            isblocking=YES;
    return(isblocking);
}

void setcursz(short sline,short eline)
{
#if defined(__OS2__)
    VIOCURSORINFO vci;

    VioGetCurType (&vci, 0);
    vci.yStart = sline;
    vci.cEnd = eline;
    VioGetCurType (&vci, 0);
#else
    union REGS regs;

    regs.h.ch=(unsigned char)sline;
    regs.h.cl=(unsigned char)eline;
    regs.h.ah=1;          /*  set cursor type  */
#ifdef __386__
    int386(0x10,&regs,&regs);
#else
    int86(0x10,&regs,&regs);
#endif
#endif
}

void showcur(void)
{
    int sl,el;

    getcursz(&sl,&el);
    if(sl&48) setcursz(sline,eline);
}

void spc (short count)
{
#if defined(__OS2__)
    short attr, row, col, len, chat;

    VioGetCurPos ((PUSHORT)&row, (PUSHORT)&col, 0);
    len = 2;
    VioReadCellStr ((PCH)&attr, (PUSHORT)len, row, col, 0);

    chat = (short)((attr & 0xFF00U) | (' ' & 0x00ff));
    VioWrtNCell ((PCH)&chat, count, row, col, 0);
#else
    union REGS regs;

    regs.h.bh = 0;
    regs.h.ah = 8;                  /* get character/attribute under cursor */
#ifdef __386__
    int386(0x10,&regs,&regs);
#else
    int86(0x10,&regs,&regs);
#endif
    regs.h.bl = regs.h.ah;
#ifdef __386__
    regs.w.cx = count;
#else
    regs.x.cx = count;
#endif
    regs.h.bh = 0;
    regs.h.al = ' ';
    regs.h.ah = 9;                  /* display character and attribute */
#ifdef __386__
    int386(0x10,&regs,&regs);
#else
    int86(0x10,&regs,&regs);
#endif
#endif
}

/* this function will exchange the contents of the applicable buffers */
static void swap_contents(short *pfound,short *pcurr,short shadow)
{
    struct _wrec_t *wptr;
    short temp;
    short chat;
    short *pscreen;
#if defined(__OS2__)
    PBYTE16 pbLVB;
    USHORT cbLVB;
    VIOMODEINFO viomi;

    viomi.cb = sizeof (viomi);
    VioGetMode (&viomi, 0);
    cbLVB = (USHORT)(viomi.row * viomi.col * 2);

    VioGetBuf ((PULONG)&pbLVB, &cbLVB, 0);
#endif

    /* display character from current position in window to */
    /* activate on the screen.  if character is part of a   */
    /* shadow, reflect the character on the screen.         */
#if defined(__OS2__)
    pscreen=(short *)(pbLVB+(crow*_vinfo.numcols+ccol)*2);
#else
    pscreen=(short *)MKFP(_vinfo.videoseg,(crow*_vinfo.numcols+ccol)*2);
#endif
    temp=*pscreen;
    if(shadow&2) *pcurr=(short)((*pcurr&0xff00)|(temp&0x00ff));
    chat=(short)(((temp&0x8000&&shadow)?(*pcurr|0x8000):*pcurr));
    *pscreen=chat;

    /* let window position directly above position  */
    /* to activate have the character that it holds */
    *pcurr=*pfound;

    /* if current character position to activate will */
    /* activate over a shadow in another window       */
    if(shadow&1) {

        /* resolve all shadows upwards */
        wptr=curr;
        chat=(short)((curr->wsattr<<8)|(*pfound&0x00ff));
        for(curr=curr->next;curr!=NULL;curr=curr->next) {
            if(window_blocking()) {
                *(calc_window(curr))=chat;
                chat=temp;
                break;
            }
            else {
                if(bshadow_blocking())
                    *(calc_bshadow(curr))=chat;
                else
                    if(rshadow_blocking())
                        *(calc_rshadow(curr))=chat;
            }
        }
        temp=chat;
        curr=wptr;
    }

    /* let character position activated hold character */
    /* that was on the screen in the same position     */
    *pfound=temp;
#if defined(__OS2__)
    VioShowBuf (0, cbLVB, 0);
#endif
}

short tabstop(short column,short tabwidth)
{
    short sum;

    sum=(short)(column+tabwidth);
    return((short)(sum-(sum%tabwidth)));
}

short DLL_EXPORT wactiv(WINDOW whandle)
{
    short startcol,stopcol;
    struct _wrec_t *prev,*next;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* if window is already active, ignore request */
    if(whandle==_winfo.active->whandle) return(_winfo.error=W_NOERROR);

    /* find address of window's record */
    found=wfindrec(whandle);
    if(found==NULL) return(_winfo.error=W_NOTFOUND);

    /* check every character position in window to activate */
    for(crow=found->srow;crow<=found->erow;crow++) {
        for(ccol=found->scol;ccol<=found->ecol;ccol++) {

            /* check all window records "above" window to activate */
            for(curr=found->next;curr!=NULL;curr=curr->next) {

                /* see if current position in window to activate */
                /* is blocked by same position in test window    */
                if(window_blocking()) {

                    /* calculate buffer addresses and swap contents */
                    swap_contents(calc_window(found),calc_window(curr),0);
                    break;
                }

                /* see if test window has a shadow */
                if(curr->wsbuf!=NULL) {

                    /* see if shadow to the right of test window is        */
                    /* blocking the current position of window to activate */
                    if(rshadow_blocking()) {
                        swap_contents(calc_window(found),calc_rshadow(curr),1);
                        break;
                    }

                    /* see if shadow to the bottom of test window is       */
                    /* blocking the current position of window to activate */
                    if(bshadow_blocking()) {
                        swap_contents(calc_window(found),calc_bshadow(curr),1);
                        break;
                    }
                }
            }
        }
    }

    /* if window to activate has a shadow, then check      */
    /* every character position in the shadow to see       */
    /* if it is blocked by another window or window shadow */
    if(found->wsbuf!=NULL) {

        /* search the right shadow of window to activiate */
        startcol=(short)(found->ecol+1);
        stopcol=(short)(startcol+1);
        for(crow=(short)(found->srow+1);crow<=found->erow;crow++) {
            for(ccol=startcol;ccol<=stopcol;ccol++) {

                /* check all window records "above" shadow to activate */
                for(curr=found->next;curr!=NULL;curr=curr->next) {

                    /* see if current position in shadow to activate */
                    /* is blocked by same position in current window */
                    if(window_blocking()) {

                        /* calculate buffer addresses and swap contents */
                        swap_contents(calc_rshadow(found),calc_window(curr),2);
                        break;
                    }

                    /* see if test window has a shadow */
                    if(curr->wsbuf!=NULL) {

                        /* see if current position of window to activate is */
                        /* blocked by the right shadow of the test window   */
                        if(rshadow_blocking()) {
                            swap_contents(calc_rshadow(found),
                                          calc_rshadow(curr),3);
                            break;
                        }

                        /* see if current position of window to activate is */
                        /* blocked by the bottom shadow of the test window  */
                        if(bshadow_blocking()) {

                            swap_contents(calc_rshadow(found),
                                          calc_bshadow(curr),3);
                            break;
                        }
                    }
                }
            }
        }

        /* search bottom shadow */
        startcol=(short)(found->scol+2);
        stopcol=(short)(found->ecol+2);
        crow=(short)(found->erow+1);
        for(ccol=startcol;ccol<=stopcol;ccol++) {

            /* check all window records "above" shadow to activate */
            for(curr=found->next;curr!=NULL;curr=curr->next) {

                /* see if current position in shadow to activate */
                /* is blocked by same position in test window    */
                if(window_blocking()) {

                    /* calculate buffer addresses and swap contents */
                    swap_contents(calc_bshadow(found),calc_window(curr),2);
                    break;
                }

                /* see if test window has a shadow */
                if(curr->wsbuf!=NULL) {
                    if(rshadow_blocking()) {
                        swap_contents(calc_bshadow(found),calc_rshadow(curr),
                                      3);
                        break;
                    }
                    if(bshadow_blocking()) {
                        swap_contents(calc_bshadow(found),calc_bshadow(curr),
                                      3);
                        break;
                    }
                }
            }
        }
    }

    /* re-link pointer to window record to be activated */
    prev=found->prev;
    next=found->next;
    if(prev!=NULL) prev->next=next;
    next->prev=prev;
    _winfo.active->next=found;
    found->prev=_winfo.active;
    found->next=NULL;
    _winfo.active=found;

    /* update help category */
    if(_winfo.active->help) _winfo.help=_winfo.active->help;

    /* reset cursor position */
    gotoxy_(_winfo.active->row,_winfo.active->column);

    /* return normally */
    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wcclear(short attr)
{
    short border;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* check for window border */
    border=_winfo.active->border;

    fill_((short)(_winfo.active->srow+border),(short)(_winfo.active->scol+border),
          (short)(_winfo.active->erow-border),(short)(_winfo.active->ecol-border),
          _winfo.fillch,attr);

    wgotoxy(0,0);                       /* home the cursor */

    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wcenters(short wrow,short attr,char *str)
{
    short window_width,string_length;
    short start_column,border;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* check for valid row */
    if(wchkcoord(wrow,0)) return(_winfo.error=W_INVCOORD);

    /* check for window border */
    border=_winfo.active->border;

    /* calculate start column & window width */
    start_column=(short)(_winfo.active->scol+border);
    window_width=(short)((_winfo.active->ecol-border)-start_column+1);

    /* check length of input string */
    string_length=(short)strlen(str);
    if(string_length>window_width) return(_winfo.error=W_STRLONG);

    /* display the string */
    prints((short)(_winfo.active->srow+wrow+border),
          (short)(((window_width/2)+start_column)-(string_length/2)),attr,str);

    /* return normally */
    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wclose(void)
{
    struct _wrec_t *wrec;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* if window has a shadow, close shadow first */
    if(_winfo.active->wsbuf!=NULL) wshadoff();

    /* restore contents of and free memory held by window */
    wrestore(_winfo.active->wbuf);

    /* decrement total number of open windows */
    _winfo.total--;

    /* free memory held by window's record and update linked list */
    wrec=_winfo.active->prev;
    free(_winfo.active);
    _winfo.active=wrec;
    if(_winfo.active!=NULL) _winfo.active->next=NULL;

    /* update cursor location and help category */
    if(_winfo.active!=NULL) {
        gotoxy_(_winfo.active->row,_winfo.active->column);
        if(_winfo.active->help) _winfo.help=_winfo.active->help;
    }

    /* return normally */
    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wcloseall(void)
{
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    while(_winfo.total) {
        if(wclose()) return(_winfo.error);
    }
    return(_winfo.error=W_NOERROR);
}

short wchkbox(short wsrow,short wscol,short werow,short wecol)
{
    return( (
      wchkcoord(wsrow,wscol)||wchkcoord(werow,wecol)||
      (wsrow>werow)||(wscol>wecol) ) ? 1 : 0 );
}

short wchkcol(short wcol)
{
    return((short)((wcol<0||(wcol>((_winfo.active->ecol-_winfo.active->border)-
      (_winfo.active->scol+_winfo.active->border))))?1:0));
}

short wchkcoord(short wrow,short wcol)
{
    return((wchkrow(wrow)||wchkcol(wcol))?1:0);
}

short wchkrow(short wrow)
{
    return((short)((wrow<0||(wrow>((_winfo.active->erow-_winfo.active->border)-
           (_winfo.active->srow+_winfo.active->border))))?1:0));
}

short DLL_EXPORT wclreol(void)
{
    short col,stopcol;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* calculate stop column */
    stopcol=(short)(_winfo.active->ecol-_winfo.active->border);

    /* clear to end of window's line */
    for(col=_winfo.active->column;col<=stopcol;col++)
        printc(_winfo.active->row,col,_winfo.active->attr,_winfo.fillch);

    /* return normally */
    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wclreos(void)
{
    short wrow,werow,wr,wc;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* save current window row and column */
    wreadcur(&wr,&wc);

    wrow=wr;
    werow=(short)(_winfo.active->erow - _winfo.active->srow - _winfo.active->border);
    wclreol();
    wrow++;
    while(wrow<=werow) {
        wgotoxy(wrow,0);
        wclreol();
        wrow++;
    }

    /* restore window row and column */
    wgotoxy(wr,wc);

    /* return normally */
    return(_winfo.error=W_NOERROR);
}

short wdupc(short ch,short count)
{
    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* display ch for count times */
    while(count--) wputc(ch);

    /* return with _winfo.error set by wputc() */
    return(_winfo.error);
}

short DLL_EXPORT wfill(short wsrow,short wscol,short werow,short wecol,short ch,short attr)
{
    register int border;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* check for valid coordinates */
    if(wchkbox(wsrow,wscol,werow,wecol)) return(_winfo.error=W_INVCOORD);

    /* check for window border */
    border=_winfo.active->border;

    /* fill in specified region */
    fill_((short)(_winfo.active->srow+wsrow+border),(short)(_winfo.active->scol+wscol+border),
          (short)(_winfo.active->srow+werow+border),(short)(_winfo.active->scol+wecol+border),
          ch,attr);

    /* return with no error */
    return(_winfo.error=W_NOERROR);
}

struct _wrec_t *wfindrec(short whandle)
{
    struct _wrec_t *wrec;

    /* scan through linked list for record belonging to requested handle */
    wrec=_winfo.active;
    while(wrec!=NULL) {
        if(whandle==wrec->whandle) break;
        wrec=wrec->prev;
    }

    /* return address of found record */
    return(wrec);
}

short DLL_EXPORT wgotoxy(short wrow,short wcol)
{
    short row,col;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* check for valid cursor coordinates */
    if(wchkcoord(wrow,wcol)) return(_winfo.error=W_INVCOORD);

    /* calculate effective cursor coordinates */
    row=(short)(_winfo.active->srow+wrow+_winfo.active->border);
    col=(short)(_winfo.active->scol+wcol+_winfo.active->border);

    /* update window record */
    _winfo.active->row=(unsigned char)row;
    _winfo.active->column=(unsigned char)col;

    /* set cursor location */
    gotoxy_(row,col);

    /* return with no error */
    return(_winfo.error=W_NOERROR);
}

short whandle (void)
{
    /* test for active window */
    if(!_winfo.total) {
        _winfo.error=W_NOACTIVE;
        return(0);
    }

    /* return normally */
    _winfo.error=W_NOERROR;
    return(_winfo.active->whandle);
}

short DLL_EXPORT whline(short wsrow,short wscol,short count,short btype,short attr)
{
    char *bt;
    short col;
    short row,up,down;
    char ch;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* check for valid box type */
    if(btype<0||btype>5) return(_winfo.error=W_INVBTYPE);
    row=wsrow; col=wscol;

    /* abbreviate pointer */
    bt=_box_table[btype];

    if(count) {

        /* see if a left junction or corner is needed */
        up=isupvert(btype,read_char((short)(row-1),col));
        down=isdownvert(btype,read_char((short)(row+1),col));
        if(up&&down)
            ch=LVJ;
        else if(up)
            ch=LLC;
        else if(down)
            ch=ULC;
        else
            ch=UHL;

        /* display leftmost character */
        if(disp_char(row,col,attr,btype,ch,HORZ)) return(_winfo.error);
        col++;
        count--;
    }

    /* do while not last character */
    while(count>1) {

        /* see if a middle junction is needed */
        up=isupvert(btype,read_char((short)(row-1),col));
        down=isdownvert(btype,read_char((short)(row+1),col));
        if(up&&down)
            ch=MJ;
        else if(up)
            ch=LHJ;
        else if(down)
            ch=UHJ;
        else
            ch=UHL;

        /* display middle character */
        if(disp_char(row,col,attr,btype,ch,HORZ)) return(_winfo.error);
        col++;
        count--;
    }

    if(count) {

        /* see if a right junction or corner is needed */
        up=isupvert(btype,read_char((short)(row-1),col));
        down=isdownvert(btype,read_char((short)(row+1),col));
        if(up&&down)
            ch=RVJ;
        else if(up)
            ch=LRC;
        else if(down)
            ch=URC;
        else
            ch=UHL;

        /* display rightmost character */
        if(disp_char(row,col,attr,btype,ch,HORZ)) return(_winfo.error);
    }

    /* return normally */
    return(_winfo.error=W_NOERROR);
}

/* this function dectects if given window is blocking the */
/* current window or its shadow at specified coordinates  */
static short window_blocking(void)
{
    short isblocking=NO;

    if(crow>=curr->srow&&crow<=curr->erow&&ccol>=curr->scol&&ccol<=curr->ecol)
        isblocking=YES;
    return(isblocking);
}

WINDOW DLL_EXPORT wopen(short srow,short scol,short erow,short ecol,short btype,short battr,short wattr)
{
    short *wbuf;
    struct _wrec_t *wrec;
    short border;

    /* check for valid box type */
    if(btype<0 || btype>5) {
        _winfo.error=W_INVBTYPE;
        return(0);
    }

    /* see if window is to have a border */
    border=((btype==5)?NO:YES);

    /* check for valid coordinates */
    if(srow>(erow-border) || scol>(ecol-border)) {
        _winfo.error=W_INVCOORD;
        return(0);
    }

    /* check for monochrome adapter, adjust attributes */
    battr=mapattr(battr);
    wattr=mapattr(wattr);

    /* allocate memory for new record */
    wrec=(_wrec_t *)malloc(sizeof(struct _wrec_t));
    if(wrec==NULL) {
        _winfo.error=W_ALLOCERR;
        return(0);
    }

    /* save affected area of screen */
    if((wbuf=wsave(srow,scol,erow,ecol))==NULL) {
        free(wrec);
        _winfo.error=W_ALLOCERR;
        return(0);
    }

    /* add new record to linked list */
    if(_winfo.active!=NULL) _winfo.active->next=wrec;
    wrec->prev=_winfo.active;
    wrec->next=NULL;
    _winfo.active=wrec;

    /* draw and fill text box on screen */
    if(border) box_(srow,scol,erow,ecol,btype,battr);
    fill_((short)(srow+border),(short)(scol+border),(short)(erow-border),(short)(ecol-border),_winfo.fillch,wattr);

    /* increment window handle counter */
    _winfo.handle++;

    /* save window info in window record */
    _winfo.active->wbuf=wbuf;
    _winfo.active->whandle=_winfo.handle;
    _winfo.active->srow=(unsigned char)srow;
    _winfo.active->scol=(unsigned char)scol;
    _winfo.active->erow=(unsigned char)erow;
    _winfo.active->ecol=(unsigned char)ecol;
    _winfo.active->btype=(unsigned char)btype;
    _winfo.active->wattr=(unsigned char)wattr;
    _winfo.active->battr=(unsigned char)battr;
    _winfo.active->border=(unsigned char)border;
    _winfo.active->row=(unsigned char)(srow+border);
    _winfo.active->column=(unsigned char)(scol+border);
    _winfo.active->attr=(unsigned char)wattr;
    _winfo.active->title=NULL;
    _winfo.active->tpos=0;
    _winfo.active->help=0;
    _winfo.active->form=NULL;
    _winfo.active->wsbuf=NULL;

    /* increment total number of open windows */
    _winfo.total++;

    /* initialize cursor location to window row 0 column 0 */
    wgotoxy(0,0);

    /* return normally */
    _winfo.error=W_NOERROR;
    return(_winfo.handle);
}

short DLL_EXPORT wprintc(short wrow,short wcol,short attr,short ch)
{
#if !defined(__OS2__)
    short *p;
#endif
    short chat,col;
    short row;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* check for valid coordinates */
    if(wchkcoord(wrow,wcol)) return(_winfo.error=W_INVCOORD);

    /* calculate effective coordinates */
    row=(short)(wrow+_winfo.active->srow+_winfo.active->border);
    col=(short)(wcol+_winfo.active->scol+_winfo.active->border);

    /* check for monochrome adapter, adjust attribute */
    attr=mapattr(attr);

#if defined(__OS2__)
   chat = (short)((attr << 8) | (ch & 0x00ff));
   VioWrtCellStr ((PCH)&chat, 2, row, col, 0);
#else
    /* calculate video RAM address */
    p=(short *)MKFP(_vinfo.videoseg,((row*_vinfo.numcols)+col)*2);

    chat=(short)((attr<<8)|(ch&0x00ff));
    *p=chat;
    p++;
#endif

    /* return normally */
    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wprintf(const char *format,...)
{
    va_list argptr;
    char buf[MAXCHARS];

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* format string using specified parameters into buffer */
    va_start(argptr,format);            /* access argument list */
    vsprintf(buf,format,argptr);        /* create string using argument list */
    va_end(argptr);                     /* end access of argument list */

    /* display the created string */
    wputs(buf);

    /* return w/return code from wputs() */
    return(_winfo.error);
}

short DLL_EXPORT wprints(short wrow,short wcol,short attr,char *str)
{
    short col,max;
    short row,border;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* check for valid coordinates */
    if(wchkcoord(wrow,wcol)) return(_winfo.error=W_INVCOORD);

    /* see if window has border */
    border=_winfo.active->border;

    /* calculate effective coordinates */
    row=(short)(_winfo.active->srow+wrow+border);
    col=(short)(_winfo.active->scol+wcol+border);
    max=(short)((_winfo.active->ecol-border)-col+1);

    /* see if wraparound is needed - if not, use faster prints() function */
    if(strlen(str)<=max) {
        prints(row,col,attr,str);
        return(_winfo.error=W_NOERROR);
    }

    /* display as much of string as possible */
    while(*str&&max--) printc(row,col++,attr,*str++);
    return(_winfo.error=W_STRLONG);
}

short DLL_EXPORT wprintsf(short wrow,short wcol,short attr,const char *format,...)
{
    va_list argptr;
    short col,max;
    short row,border;
    char buf[MAXCHARS],*str;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* check for valid coordinates */
    if(wchkcoord(wrow,wcol)) return(_winfo.error=W_INVCOORD);

    /* format string using specified parameters into buffer */
    va_start(argptr,format);            /* access argument list */
    vsprintf(buf,format,argptr);        /* create string using argument list */
    va_end(argptr);                     /* end access of argument list */

    /* see if window has border */
    border=_winfo.active->border;

    /* calculate effective coordinates */
    row=(short)(_winfo.active->srow+wrow+border);
    col=(short)(_winfo.active->scol+wcol+border);
    max=(short)((_winfo.active->ecol-border)-col+1);

    /* see if wraparound is needed - if not, use faster prints() function */
    if(strlen(buf)<=max) {
        prints(row,col,attr,buf);
        return(_winfo.error=W_NOERROR);
    }

    /* display as much of string as possible */
    str=buf;
    while(*str&&max--) printc(row,col++,attr,*str++);
    return(_winfo.error=W_STRLONG);
}

short DLL_EXPORT wputc(short ch)
{
    short chat,ccol;
    short crow,border,cwcol,scol;
#if !defined(__OS2__)
    short *p;
#endif

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* get coordinates from window's record */
    crow=_winfo.active->row;
    ccol=_winfo.active->column;
    scol=_winfo.active->scol;
    border=_winfo.active->border;

    /* test the input character for control characters */
    switch(ch) {
        case LF:
            crow++;
        case CR:
            ccol=(short)(scol+border);
            break;
        case BS:
            if(ccol==(scol+border)) {
                ccol=(short)(_winfo.active->ecol-border);
                crow--;
                if(crow<(_winfo.active->srow+border)) crow++;
            }
            else {
                ccol--;
            }
            break;
        case HT:
            cwcol=(short)(ccol-border-scol);
            ccol+=(tabstop(cwcol,_winfo.tabwidth)-cwcol);
            break;
        case BEL:
            beep();
            break;
        default:
            chat=(short)(((short)_winfo.active->attr<<8)|(ch&0x00ff));
#if defined(__OS2__)
            VioWrtCellStr ((PCH)&chat, 2, crow, ccol, 0);
#else
            /* calculate address in video RAM */
            p=(short *)MKFP(_vinfo.videoseg,((crow*_vinfo.numcols)+ccol)*2);
            *p=chat;
            p++;
#endif
            ccol++;
    }

    /* see if wrap-around is needed */
    if(ccol>(_winfo.active->ecol-border)) {
        ccol=(short)(scol+border);
        crow++;
    }

    /* see if scroll is needed */
    if(crow>(_winfo.active->erow-border)) {
        wscroll(1,SUP);
        crow--;
    }

    /* update window's record */
    _winfo.active->row=(unsigned char)crow;
    _winfo.active->column=(unsigned char)ccol;

    /* reset cursor position */
    gotoxy_(crow,ccol);

    /* return normally */
    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wputs(char *str)
{
    unsigned char *ccol;
    short chat;
    unsigned char *crow,cwcol,border,scol;
    char *q;
#if !defined(__OS2__)
    short *p;
#endif

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* get effective coordinates from window's record */
    crow  =&(_winfo.active->row);
    ccol  =&(_winfo.active->column);
    scol  =_winfo.active->scol;
    border=_winfo.active->border;

    /* do while not end of string */
    for(q=str;*q;q++) {

        /* test the input character for control characters */
        switch(*q) {
            case LF:
                (*crow)++;
            case CR:
                *ccol=(unsigned char)(scol+border);
                break;
            case BS:
                if(*ccol==(scol+border)) {
                    *ccol=(unsigned char)(_winfo.active->ecol-border);
                    (*crow)--;
                    if(*crow<(_winfo.active->srow+border)) (*crow)++;
                }
                else {
                    (*ccol)--;
                }
                break;
            case HT:
                cwcol=(unsigned char)((*ccol)-border-scol);
                (*ccol)+=(tabstop(cwcol,_winfo.tabwidth)-cwcol);
                break;
            case BEL:
                beep();
                break;
            case ESC:
                q=process_esc(q);
                break;
            default:
                /* calculate address in video RAM */
                chat=(short)((((short)_winfo.active->attr<<8)|(*q&0x00ff)));
#if defined(__OS2__)
                VioWrtCellStr ((PCH)&chat, 2, *crow, *ccol, 0);
#else
                p=(short *)MKFP(_vinfo.videoseg,(((*crow)*_vinfo.numcols)+*ccol)*2);
                *p=chat;
                p++;
#endif
                (*ccol)++;
        }

        /* see if wrap-around is needed */
        if( (*ccol) > (_winfo.active->ecol-border) ) {
            *ccol=(unsigned char)(scol+border);
            (*crow)++;
        }

        /* see if scroll is needed */
        if( (*crow) > (_winfo.active->erow-border) ) {
            wscroll(1,SUP);
            (*crow)--;
        }
    }

    /* reset cursor position */
    gotoxy_(*crow,*ccol);

    /* return normally */
    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wreadcur(short *wrow,short *wcol)
{
    short row,col;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* read effective cursor coordinates */
    readcur(&row,&col);                 

    /* calculate window cursor coordinates */
    *wrow=(short)(row-_winfo.active->srow-_winfo.active->border);
    *wcol=(short)(col-_winfo.active->scol-_winfo.active->border);

    /* return normally */
    return(_winfo.error=W_NOERROR);
}

void DLL_EXPORT wrestore(short *wbuf)
{
    short crow;
    short *q,srow,scol,erow,ecol,bytes,doffs,width,oneline;
    short *p;

    /* get coordinates from window's buffer */
    q=wbuf;                             
    srow=*q++;
    scol=*q++;
    erow=*q++;
    ecol=*q++;

    /* calculate info on window */
    width=(short)(ecol-scol+1);
    bytes=(short)(width*2);
    doffs=(short)((srow*_vinfo.numcols+scol)*2);
    oneline=(short)(_vinfo.numcols*2);

    /* do for all rows in window */
    for(crow=srow;crow<=erow;crow++) {
        p=(short *)q;
#if defined(__OS2__)
        VioWrtCellStr ((PCH)p, bytes, crow, scol, 0);
#else
        memcpy (MKFP(_vinfo.videoseg,doffs),p,bytes);
#endif
        doffs+=oneline;
        q+=width;
    }

    /* free window's buffer */
    free(wbuf);                         
}

short wrjusts(short wrow,short wjcol,short attr,char *str)
{
    short row,col,jcol;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* check for valid coordinates */
    if(wchkcoord(wrow,wjcol)) return(_winfo.error=W_INVCOORD);

    /* calculate effective coordinates */
    row=(short)(_winfo.active->srow+wrow+_winfo.active->border);
    jcol=(short)(_winfo.active->scol + wjcol + _winfo.active->border);
    col=(short)(jcol - strlen(str) + 1);

    /* make sure left side of string fits in window */
    if( col < (_winfo.active->scol+_winfo.active->border) )
        return(_winfo.error=W_STRLONG);

    /* display the string */
    prints(row,col,attr,str);

    /* return normally */
    return(_winfo.error=W_NOERROR);
}

short *DLL_EXPORT wsave(short srow,short scol,short erow,short ecol)
{
    short crow;
    short *q,*wbuf,bytes,soffs,width,oneline;
    short *p;

    /* allocate buffer to hold window's contents */
    if((wbuf=(short *)malloc(((erow-srow+1)*(ecol-scol+1)+4)*2))!=NULL) {

        /* calculate info on window */
        width=(short)(ecol-scol+1);
        bytes=(short)(width*2);
        soffs=(short)((srow*_vinfo.numcols+scol)*2);
        oneline=(short)(_vinfo.numcols*2);

        /* write window's coordinates to allocated buffer */
        q=wbuf;
        *q++=srow;
        *q++=scol;
        *q++=erow;
        *q++=ecol;

        /* do for all rows in window */
        for(crow=srow;crow<=erow;crow++) {
            p=(short *)q;
#if defined(__OS2__)
            VioReadCellStr ((PCH)p, (PUSHORT)&bytes, crow, scol, 0);
#else
            memcpy(p,MKFP(_vinfo.videoseg,soffs),bytes);
#endif
            soffs+=oneline;
            q+=width;
        }
    }

    /* return address of window's buffer */
    return(wbuf);                       
}

short DLL_EXPORT wscroll(short count,short direction)
{
    short numrows,border;
#if !defined(__OS2__)
    union REGS regs;
#else
    short chat;
#endif

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* check for window border */
    border=_winfo.active->border;

    /* check scroll count against number of rows in scroll boundary */
    numrows= (short)(_winfo.active->erow-border - _winfo.active->srow+border + 1);
    if(count > numrows) count=numrows;

#if defined(__OS2__)
    chat = (short)((_winfo.active->wattr << 8) | (' ' & 0x00ff));
    if (direction == SUP)
        VioScrollUp ((USHORT)(_winfo.active->srow+border), (USHORT)(_winfo.active->scol+border), (USHORT)(_winfo.active->erow-border), (USHORT)(_winfo.active->ecol-border), (USHORT)count, (PCH)&chat, 0L);
    else
        VioScrollDn ((USHORT)(_winfo.active->srow+border), (USHORT)(_winfo.active->scol+border), (USHORT)(_winfo.active->erow-border), (USHORT)(_winfo.active->ecol-border), (USHORT)count, (PCH)&chat, 0L);
#else
    /* use BIOS function call 6 (up) or 7 (down) to scroll window */
    regs.h.bh=_winfo.active->wattr;
    regs.h.ch=(unsigned char)(_winfo.active->srow+border);
    regs.h.cl=(unsigned char)(_winfo.active->scol+border);
    regs.h.dh=(unsigned char)(_winfo.active->erow-border);
    regs.h.dl=(unsigned char)(_winfo.active->ecol-border);
    regs.h.al=(unsigned char)count;
    regs.h.ah=((direction==SDOWN)?7:6);
#ifdef __386__
    int386(0x10,&regs,&regs);
#else
    int86(0x10,&regs,&regs);
#endif
#endif

    /* return with no error */
    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wscrollbox(short wsrow,short wscol,short werow,short wecol,short count,short direction)
{
    short numrows,border,ssrow,serow;
#if !defined(__OS2__)
    union REGS regs;
#else
    short chat;
#endif

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* check for valid coordinates */
    if(wchkbox(wsrow,wscol,werow,wecol)) return(_winfo.error=W_INVCOORD);

    /* check for window border */
    border=_winfo.active->border;

    /* check scroll count against number of rows in scroll boundary */
    ssrow   = (short)(_winfo.active->srow+wsrow+border);
    serow   = (short)(_winfo.active->srow+werow+border);
    numrows = (short)(serow - ssrow + 1);
    if(count > numrows) count=numrows;

#if defined(__OS2__)
    chat = (short)((_winfo.active->wattr << 8) | (' ' & 0x00ff));
    if (direction == SUP)
        VioScrollUp ((USHORT)ssrow, (USHORT)(_winfo.active->scol+wscol+border), (USHORT)serow, (USHORT)(_winfo.active->scol+wecol+border), (USHORT)count, (PCH)&chat, 0L);
    else
        VioScrollDn ((USHORT)ssrow, (USHORT)(_winfo.active->scol+wscol+border), (USHORT)serow, (USHORT)(_winfo.active->scol+wecol+border), (USHORT)count, (PCH)&chat, 0L);
#else
    /* use BIOS function call 6 (up) or 7 (down) to scroll window */
    regs.h.bh=_winfo.active->wattr;
    regs.h.ch=(unsigned char)ssrow;
    regs.h.cl=(unsigned char)(_winfo.active->scol+wscol+border);
    regs.h.dh=(unsigned char)serow;
    regs.h.dl=(unsigned char)(_winfo.active->scol+wecol+border);
    regs.h.al=(unsigned char)count;
    regs.h.ah=(unsigned char)((direction==SDOWN)?7:6);
#ifdef __386__
    int386(0x10,&regs,&regs);
#else
    int86(0x10,&regs,&regs);
#endif
#endif

    /* return with no error */
    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wshadoff(void)
{
    short *q,ccol,crow,srow,scol,erow,ecol,stop,chat;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* if window doesn't have a shadow, ignore request */
    if(_winfo.active->wsbuf==NULL) return(_winfo.error=W_NOERROR);

    /* get window coordinates from the window's record */
    srow=_winfo.active->srow;
    scol=_winfo.active->scol;
    erow=_winfo.active->erow;
    ecol=_winfo.active->ecol;

    /* start at upper right corner of shadow and work down */
    crow=(short)(srow+1);
    ccol=(short)(ecol+1);
    q=_winfo.active->wsbuf;

    /* delete shadow to right of window */
    while(crow<=erow) {
        chat=*q++;
        printc(crow,ccol,(short)(chat>>8),chat);
        chat=*q++;
        printc(crow++,(short)(ccol+1),(short)(chat>>8),chat);
    }

    /* start at lower left corner of shadow and work right */
    crow=(short)(erow+1);
    ccol=(short)(scol+2);
    stop=(short)(ecol+2);

    /* delete bottom shadow */
    while(ccol<=stop) {
        chat=*q++;
        printc(crow,ccol++,(short)(chat>>8),chat);
    }

    /* free memory held by shadow */
    free(_winfo.active->wsbuf);

    /* update window's record */
    _winfo.active->wsbuf=NULL;
    _winfo.active->wsattr=0xff;

    /* return with no error */
    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wshadow(short attr)
{
    short *wsbuf,*q;
    short *p;
    short ccol,crow,incr,srow,scol,erow,ecol,stop,chat1,chat2;
#if defined(__OS2__)
    PBYTE16 pbLVB;
    USHORT cbLVB;
    VIOMODEINFO viomi;

    viomi.cb = sizeof (viomi);
    VioGetMode (&viomi, 0);
    cbLVB = (USHORT)(viomi.row * viomi.col * 2);

    VioGetBuf ((PULONG)&pbLVB, &cbLVB, 0);
#endif

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* see if window already has a shadow */
    if(_winfo.active->wsbuf!=NULL) return(_winfo.error=W_NOERROR);

    /* get window coordinates from the window's record */
    srow=_winfo.active->srow;
    scol=_winfo.active->scol;
    erow=_winfo.active->erow;
    ecol=_winfo.active->ecol;

    /* allocate buffer to hold shadow's contents */
    if((wsbuf=(short *)malloc((((erow-srow)*2)+ecol-scol+1)*2))==NULL)
        return(_winfo.error=W_ALLOCERR);

    /* check for/adjust to monochrome attributes */
    attr=mapattr(attr);

    /* start at upper right corner of shadow and work down */
    crow=(short)(srow+1);
    ccol=(short)(ecol+1);
    q=wsbuf;

    /* if not using BIOS video writes, calculate screen offset */
    if(!_vinfo.usebios) {
#if defined(__OS2__)
        p=(short *)(pbLVB+((crow*_vinfo.numcols)+ccol)*2);
#else
        p=(short *)MKFP(_vinfo.videoseg,((crow*_vinfo.numcols)+ccol)*2);
#endif
        incr=(short)(_vinfo.numcols-1);
    }

    /* draw shadow to right of window */
    while(crow<=erow) {

        /* read current screen characters/attributes */
        chat1=*p;
        p++;
        chat2=*p;
        p+=incr;

        /* save in shadow's buffer */
        *q++=chat1;
        *q++=chat2;

        /* write characters back to screen using shadow's attribute */
        printc(crow,ccol,(short)((chat1&0x8000)?(attr|BLINK):attr),chat1);
        printc(crow++,(short)(ccol+1),(short)((chat2&0x8000)?(attr|BLINK):attr),chat2);
    }

    /* start at lower left corner of shadow and work right */
    crow=(short)(erow+1);
    ccol=(short)(scol+2);
    stop=(short)(ecol+2);

    if(!_vinfo.usebios)
#if defined(__OS2__)
        p=(short *)(pbLVB+((crow*_vinfo.numcols)+ccol)*2);
#else
        p=(short *)MKFP(_vinfo.videoseg,((crow*_vinfo.numcols)+ccol)*2);
#endif

    /* draw bottom shadow */
    while(ccol<=stop) {

        /* read current screen character/attribute */
        chat1=*p;
        p++;

        /* save in shadow's buffer */
        *q++=chat1;

        /* write character back to screen using shadow's attribute */
        printc(crow,ccol++,(short)((chat1&0x8000)?(attr|BLINK):attr),chat1);
    }

    /* save info in window's record */
    _winfo.active->wsbuf  = wsbuf;
    _winfo.active->wsattr = (unsigned char)attr;

    /* return with no error */
    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wtextattr(short attr)
{
    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* update window's record */
    _winfo.active->attr=(unsigned char)mapattr(attr);

    /* return normally */
    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wtitle(char *str,short tpos,short tattr)
{
    short left,right,start,len,width,offs;
    char *p;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* check for monochrome adapter, adjust title attribute */
    tattr=mapattr(tattr);

    /* redraw box if deleting or moving title */
    if(str==NULL||_winfo.active->title!=NULL) {
        if(_winfo.active->border)
            box_(_winfo.active->srow,_winfo.active->scol,_winfo.active->erow,
                 _winfo.active->ecol,_winfo.active->btype,
                 _winfo.active->battr);
    }

    /* if not deleting the title, calculate position and display it */
    if(str!=NULL) {
        left=(short)(_winfo.active->scol+1);
        right=(short)(_winfo.active->ecol-1);
        width=(short)(right-left+1);
        len=(short)strlen(str);

        /* don't display title if window is borderless */
        if(_winfo.active->border) {

            switch(tpos) {
                case TLEFT:
                    start=(short)((len>(width-3))?left:(left+1));
                    break;
                case TCENTER:
                    start=(short)((len>(width-2))?left:(((width/2)+left)-(len/2)));
                    break;
                default:        /* default is TRIGHT */
                    offs=(short)(width-len);
                    if(offs>2) offs--;
                    start=(short)((len>width)?left:(left+offs));
            }

            /* allocate space for window title string, and copy it there */
            if((p=(char *)malloc(((width>len)?width:len)+1))==NULL)
                return(_winfo.error=W_ALLOCERR);
            strcpy(p,str);
            *(p+width)='\0';

            /* display title string */
            prints(_winfo.active->srow,start,tattr,p);

            /* free allocated space */
            free(p);
        }
    }

    /* update window's record */
    _winfo.active->title=str;
    _winfo.active->tpos=(unsigned char)tpos;
    _winfo.active->tattr=(unsigned char)tattr;

    /* return normally */
    return(_winfo.error=W_NOERROR);
}

short DLL_EXPORT wvline(short wsrow,short wscol,short count,short btype,short attr)
{
    char *bt;
    short col;
    short row,left,right;
    char ch;

    /* check for active window */
    if(!_winfo.total) return(_winfo.error=W_NOACTIVE);

    /* check for valid box type */
    if(btype<0||btype>5) return(_winfo.error=W_INVBTYPE);
    row=wsrow; col=wscol;

    /* abbreviate pointer */
    bt=_box_table[btype];

    if(count) {

        /* see if a top junction or corner is needed */
        left=islefthorz(btype,read_char(row,(short)(col-1)));
        right=isrighthorz(btype,read_char(row,(short)(col+1)));
        if(left&&right)
            ch=UHJ;
        else if(left)
            ch=URC;
        else if(right)
            ch=ULC;
        else
            ch=LVL;

        /* display uppermost character */
        if(disp_char(row,col,attr,btype,ch,VERT)) return(_winfo.error);
        row++;
        count--;
    }

    /* do while not last character */
    while(count>1) {
        left=islefthorz(btype,read_char(row,(short)(col-1)));
        right=isrighthorz(btype,read_char(row,(short)(col+1)));
        if(left&&right)
            ch=MJ;
        else if(left)
            ch=RVJ;
        else if(right)
            ch=LVJ;
        else
            ch=LVL;

        /* display middle character */
        if(disp_char(row,col,attr,btype,ch,VERT)) return(_winfo.error);
        row++;
        count--;
    }

    if(count) {

        /* see if a bottom junction or corner is needed */
        left=islefthorz(btype,read_char(row,(short)(col-1)));
        right=isrighthorz(btype,read_char(row,(short)(col+1)));
        if(left&&right)
            ch=LHJ;
        else if(left)
            ch=LRC;
        else if(right)
            ch=LLC;
        else
            ch=LVL;

        /* display bottommost character */
        if(disp_char(row,col,attr,btype,ch,VERT)) return(_winfo.error);
    }

    /* return normally */
    return(_winfo.error=W_NOERROR);
}


