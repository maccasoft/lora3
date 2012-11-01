
// LoraBBS Version 2.99 Free Edition
// Copyright (C) 1987-98 Marco Maccaferri
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

typedef unsigned int   bit;
typedef unsigned short word;
typedef unsigned char  byte;
typedef long           dword;

#define  TWIT        0x10
#define  DISGRACE    0x20
#define  LIMITED     0x30
#define  NORMAL      0x40
#define  WORTHY      0x50
#define  PRIVIL      0x60
#define  FAVORED     0x70
#define  EXTRA       0x80
#define  CLERK       0x90
#define  ASSTSYSOP   0xA0
#define  SYSOP       0xB0

struct _sys_msg {
   char  msg_name[70];
   short msg_num;
   char  msg_path[40];
   char  origin[56];
   bit   echomail       :1;
   bit   netmail        :1;
   bit   fpublic        :1;
   bit   fprivate       :1;
   bit   anon_ok        :1;
   bit   no_matrix      :1;
   bit   squish         :1;
   bit   kill_unlisted  :1;
   word  msg_sig;
   char  echotag[32];
   word  pip_board;
   byte  quick_board;
   byte  msg_priv;
   long  msg_flags;
   byte  write_priv;
   long  write_flags;
   byte  use_alias;
   short max_msgs;
   short max_age;
   short age_rcvd;
   char  forward1[80];
   char  forward2[80];
   char  forward3[80];
   bit   msg_restricted :1;
   bit   passthrough    :1;
   bit   internet_mail  :1;
   byte  areafix;
   char  qwk_name[14];
   long  afx_flags;
   word  gold_board;
   bit   sendonly       :1;
   bit   receiveonly    :1;
   char  filler1[26];
};

struct _sys_file {
   char  file_name[70];
   short file_num;
   char  uppath[40];
   char  filepath[40];
   char  filelist[50];
   bit   freearea  :1;
   bit   norm_req  :1;
   bit   know_req  :1;
   bit   prot_req  :1;
   bit   nonews    :1;
   bit   no_global_search :1;
   bit   no_filedate :1;
   bit   file_restricted :1;
   word  file_sig;
   byte  file_priv;
   long  file_flags;
   byte  download_priv;
   long  download_flags;
   byte  upload_priv;
   long  upload_flags;
   byte  list_priv;
   long  list_flags;
   char  filler2[10];
   char  short_name[13];
   char  filler3[8];
   char  tic_tag[32];
   char  tic_forward1[80];
   char  tic_forward2[80];
   char  tic_forward3[80];
   byte  tic_level;
   long  tic_flags;
   bit   cdrom     :1;
   char  filler4[106];
};

#define MAT_CM           0x0001
#define MAT_DYNAM        0x0002
#define MAT_BBS          0x0004
#define MAT_NOREQ        0x0008
#define MAT_OUTONLY      0x0010
#define MAT_NOOUT        0x0020
#define MAT_FORCED       0x0040
#define MAT_LOCAL        0x0080
#define MAT_SKIP         0x0100
#define MAT_NOMAIL24     0x0200
#define MAT_NOOUTREQ     0x0400
#define MAT_NOCM         0x0800
#define MAT_RESERV       0x1000
#define MAT_RESYNC       0x2000

typedef struct _event {
   short days;
   short minute;
   short length;
   short errlevel[9];
   short last_ran;
   short behavior;
   short echomail;
   short wait_time;
   short with_connect;
   short no_connect;
   short node_cost;
   char  cmd[32];
   char  month;
   char  day;
   char  err_extent[6][4];
   short extra[1];
   short res_zone;
   short res_net;
   short res_node;
   short res_point;
   char  route_tag[16];
};

#define MAXLREAD   50
#define MAXDLREAD  20
#define MAXFLAGS   4
#define MAXCOUNTER 10

struct _lastread {
   word area;
   short msg_num;
};

struct _usr {
   char  name[36];
   char  handle[36];
   char  city[26];

   struct _lastread lastread[MAXLREAD];
   struct _lastread dynlastread[MAXDLREAD];

   char  pwd[16];
   dword times;
   word  nulls;
   word  msg;

   bit   avatar    :1;
   bit   color     :1;
   bit   scanmail  :1;
   bit   use_lore  :1;
   bit   more      :1;
   bit   ansi      :1;
   bit   kludge    :1;
   bit   formfeed  :1;

   bit   hotkey    :1;
   bit   tabs      :1;
   bit   full_read :1;
   bit   badpwd    :1;
   bit   usrhidden :1;
   bit   nokill    :1;
   bit   ibmset    :1;
   bit   deleted   :1;

   byte  language;
   byte  priv;
   long  flags;
   char  ldate[20];
   short   time;
   dword upld;
   dword dnld;
   short   dnldl;
   word  n_upld;
   word  n_dnld;
   word  files;
   word  credit;
   word  dbase;
   char  signature[58];
   char  voicephone[20];
   char  dataphone[20];
   char  birthdate[10];
   char  subscrdate[10];
   char  firstdate[20];
   char  lastpwdchange[10];
   long  ptrquestion;
   byte  len;
   byte  width;
   long  id;
   short   msgposted;
   char  comment[80];
   byte  help;
   word  old_baud_rate;
   byte  counter[MAXCOUNTER];
   word  chat_minutes;

   bit   xfer_prior:1;
   bit   quiet     :1;
   bit   nerd      :1;
   bit   donotdisturb:1;
   bit   robbed    :1;
   bit   novote    :1;
   bit   havebox   :1;
   bit   security  :1;

   char  protocol;
   char  archiver;

   struct _ovr_class_rec {
      short  max_time;
      short  max_call;
      short  max_dl;
      word ratio;
      word min_baud;
      word min_file_baud;
      word start_ratio;
   } ovr_class;

   short   msg_sig;
   word  account;
   word  f_account;
   short   votes;
   short   file_sig;

   long  baud_rate;

   bit   dhotkey    :1;

   long  alias_id;

   char  extradata[281];
};

#define LCONFIG_VERSION  3

#define MAXNL 10
#define MAXPACKER 10
#define MAX_LANG          20
#define MAX_ALIAS         20

struct class_rec {
   short  priv;
   short  max_time;
   short  max_call;
   short  max_dl;
   short  dl_300;
   short  dl_1200;
   short  dl_2400;
   short  dl_9600;
   word ratio;
   word min_baud;
   word min_file_baud;
   word start_ratio;
};

struct _alias {
   short  zone;
   short  net;
   short  node;
   short  point;
   word fakenet;
   char *domain;
};

struct _configuration {
   short version;

   char sys_path[40];

   char log_name[40];
   char log_style;

   char sched_name[40];
   char user_file[40];

   char norm_filepath[40];
   char know_filepath[40];
   char prot_filepath[40];
   char outbound[40];
   char netmail_dir[40];
   char bad_msgs[40];
   char dupes[40];
   char quick_msgpath[40];
   char pip_msgpath[40];
   char ipc_path[40];
   char net_info[40];
   char glob_text_path[40];
   char menu_path[40];
   char flag_dir[40];

   long keycode;

   char about[40];
   char files[40];
   short  norm_max_kbytes;
   short  prot_max_kbytes;
   short  know_max_kbytes;
   short  norm_max_requests;
   short  prot_max_requests;
   short  know_max_requests;
   char def_pack;

   char enterbbs[70];
   char banner[70];
   char mail_only[70];

   short  com_port;
   short  old_speed;
   char modem_busy[20];
   char dial[20];
   char init[40];
   char term_init[40];

   byte mustbezero;

   byte echomail_exit;
   byte netmail_exit;
   byte both_exit;

   long speed;
   short modem_OK_errorlevel;

   byte filler[10];

   struct _altern_dial {
      char prefix[20];
      char suffix[20];
   } altdial[10];

   bit  lock_baud   :1;
   bit  ansilogon   :2;
   bit  birthdate   :1;
   bit  voicephone  :1;
   bit  dataphone   :1;
   bit  emsi        :1;
   bit  ibmset      :1;

   bit  wazoo       :1;
   bit  msgtrack    :1;
   bit  keeptransit :1;
   bit  hslink      :1;
   bit  puma        :1;
   bit  secure      :1;
   bit  janus       :1;
   bit  terminal    :1;

   bit  fillerbug   :1;
   bit  no_direct   :1;
   bit  snooping    :1;
   bit  snow_check  :1;
   bit  unpack_norm :1;
   bit  unpack_know :1;
   bit  unpack_prot :1;

   short  blank_timer;

   struct _language {
      char txt_path[30];
      char descr[24];
      char basename[10];
   } language[MAX_LANG];

   char sysop[36];
   char system_name[50];
   char location[50];
   char phone[32];
   char flags[50];

   struct _alias alias[MAX_ALIAS];

   char newareas_create[50];
   char newareas_link[50];

   short  line_offset;
   short  min_calls;
   short  vote_limit;
   short  target_up;
   short  target_down;
   byte vote_priv;
   byte max_readpriv;

   word speed_graphics;

   byte aftercaller_exit;
   byte aftermail_exit;
   short  max_connects;
   short  max_noconnects;

   byte logon_level;
   long logon_flags;

   char areachange_key[4];
   char dateformat[20];
   char timeformat[20];
   char external_editor[50];

   struct class_rec uclass[12];

   char local_editor[50];

   char QWKDir[40];
   char BBSid[10];
   word qwk_maxmsgs;

   char galileo[30];

   char norm_okfile[40];
   char know_okfile[40];
   char prot_okfile[40];

   char reg_name[36];
   long betakey;

   struct _packers {
      char id[10];
      char packcmd[30];
      char unpackcmd[30];
   } packers[10];

   struct _nl {
      char list_name[14];
      char diff_name[14];
      char arc_name[14];
   } nl[10];

   bit  ansigraphics   :2;
   bit  avatargraphics :2;
   bit  hotkeys        :2;
   bit  screenclears   :2;

   bit  autozmodem     :1;
   bit  avatar         :1;
   bit  moreprompt     :2;
   bit  mailcheck      :2;
   bit  fullscrnedit   :2;

   bit  fillerbits     :2;
   bit  ask_protocol   :1;
   bit  ask_packer     :1;
   bit  put_uploader   :1;
   bit  keep_dl_count  :1;
   bit  use_areasbbs   :1;
   bit  write_areasbbs :1;

   short  rookie_calls;

   char pre_import[40];
   char after_import[40];
   char pre_export[40];
   char after_export[40];

   byte emulation;
   char dl_path[40];
   char ul_path[40];

   bit  manual_answer  :1;
   bit  limited_hours  :1;
   bit  solar          :1;
   bit  areafix        :1;
   bit  doflagfile     :1;
   bit  multitask      :1;
   bit  ask_alias      :1;
   bit  random_birth   :1;

   short  start_time;
   short  end_time;

   char boxpath[40];
   char dial_suffix[20];

   char galileo_dial[40];
   char galileo_suffix[40];
   char galileo_init[40];

   char areafix_help[40];
   char alert_nodes[50];

   char automaint[40];

   byte min_login_level;
   long min_login_flags;
   byte min_login_age;

   char resync_nodes[50];
   char bbs_batch[40];
   byte altx_errorlevel;

   char fax_response[20];
   byte fax_errorlevel;

   char dl_counter_limits[4];

   char areas_bbs[40];
   byte afx_remote_maint;
   byte afx_change_tag;

   bit  allow_rescan    :1;
   bit  check_city      :1;
   bit  check_echo_zone :1;
   bit  save_my_mail    :1;
   bit  mail_method     :2;
   bit  replace_tear    :2;

   char my_mail[40];

   bit  stripdash       :1;
   bit  use_iemsi       :1;
   bit  newmail_flash   :1;
   bit  mymail_flash    :1;
   bit  keep_empty      :1;
   bit  show_missing    :1;
   bit  random_redial   :1;

   char override_pwd[20];
   char pre_pack[40];
   char after_pack[40];

   byte modem_timeout;
   byte login_timeout;
   byte inactivity_timeout;

   struct _altern_prefix {
      char flag[6];
      char prefix[20];
   } prefixdial[10];

   char iemsi_handle[36];
   char iemsi_pwd[20];
   short  iemsi_infotime;

   bit  iemsi_on        :1;
   bit  iemsi_hotkeys   :1;
   bit  iemsi_quiet     :1;
   bit  iemsi_pausing   :1;
   bit  iemsi_editor    :1;
   bit  iemsi_news      :1;
   bit  iemsi_newmail   :1;
   bit  iemsi_newfiles  :1;

   bit  iemsi_screenclr :1;
   bit  prot_xmodem     :1;
   bit  prot_1kxmodem   :1;
   bit  prot_zmodem     :1;
   bit  prot_sealink    :1;
   bit  tic_active      :1;
   bit  tic_check_zone  :1;
   bit  filebox         :1;

   char newkey_code[30];
   char tearline[36];

   char  uucp_gatename[20];
   short uucp_zone;
   short uucp_net;
   short uucp_node;

   byte carrier_mask;
   byte dcd_timeout;

   struct {
      char  display[20];
      short offset;
      char  ident[20];
   } packid[10];

   char quote_string[5];
   char quote_header[50];

   char tic_help[40];
   char tic_alert_nodes[50];
   char tic_newareas_create[50];
   char tic_newareas_link[50];
   byte tic_remote_maint;
   byte tic_change_tag;

   short uucp_point;

   byte  dial_timeout;
   byte  dial_pause;

   bit   newfilescheck  :2;
   bit   mono_attr      :1;
   bit   force_intl     :1;
   bit   inp_dateformat :2;
   bit   single_pass    :1;

   short ul_free_space;
   char  hangup_string[40];
   char  init2[40];
   char  init3[40];

   short page_start[7];
   short page_end[7];

   char  logbuffer;

   char newareas_path[40];
   char newareas_base;

   char answer[40];

   bit   blanker_type   :3;
   bit   tcpip          :2;
   bit   export_internet:1;

   char  internet_gate;

   char  areafix_watch[50];
   char  tic_watch[50];

   byte  netmail;
   byte  echomail;
   byte  internet;
   byte  net_echo;
   byte  echo_internet;
   byte  net_internet;
   byte  echo_net_internet;

   char  upload_check[50];
   unsigned long setup_pwd;

   char filler_2 [565]; // Per arrivare a 8192 Bytes
};

typedef struct _nodeinfo {
   short  zone;
   short  net;
   short  node;
   short  point;
   short  afx_level;
   char pw_session[20];
   char pw_areafix[20];
   char pw_tic[20];
   char pw_packet[9];
   short  modem_type;
   char phone[30];
   short  packer;
   char sysop_name[36];
   char system[36];
   bit  remap4d      :1;
   bit  can_do_22pkt :1;
   bit  wazoo        :1;
   bit  emsi         :1;
   bit  janus        :1;
   bit  pkt_create   :2;
   char aka;
   short tic_level;
   long afx_flags;
   long tic_flags;
   char tic_aka;
   long baudrate;
   char pw_inbound_packet[9];
   char mailer_aka;
   long min_baud_rate;
   char filler[40];
};
