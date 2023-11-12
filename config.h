/* See LICENSE file for copyright and license details. */

#include "mpdcontrol.c"

// #include <X11/XF86keysym.h> >> dwm.c
/*  key definitions */
#define XK_PrtSc           0x0000ff61 // PrtSc
#define XK_Delete          0x0000ffff // Delete
#define XK_Esc             0x0000ff1b // Esc
#define XK_IncSound        0x1008ff13 // Fn0 Sound+
#define XK_DecSound        0x1008ff11 // Fn0 Sound-
#define XK_ToggleSound     0x1008ff12 // Fn0 Mute
#define XK_IncLight        0x1008ff02 // Fn Light+
#define XK_DecLight        0x1008ff03 // Fn Light-
#define XK_Home            0x0000ff50 // Home
#define XK_Bluetooth       0x1008ff94 // Bluetooth 
// F1-12 conversions
//#define XK_F1              XK_ToggleSound
//#define XK_F2              XK_DecSound


/* appearance */
static const unsigned int borderpx  = 4;        /* border pixel of windows */
static const unsigned int snap      = 16;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int user_bh            = 0;        /* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
static const char *fonts[]          = { 
    "Hack Nerd Font Mono:pixelsize=28:antialias=true:autohint=true",
    "JoyPixels:pixelsize=20:antialias=true:autohint=true",    
};
static const char dmenufont[]       = "Hack Nerd Font Mono:pixelsize=28:antialias=true;autohind=true";
static const char col_gray1[]       = "#cb0000";
static const char col_gray2[]       = "#333333";
static const char col_gray3[]       = "#cccccc";
static const char col_gray4[]       = "#cb0000";
static const char col_cyan[]        = "#cb0000";
static const char col_white[]       = "#ffffff";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_white, col_gray2, col_gray2 },
	[SchemeSel]  = { col_white, col_gray1,  col_gray2  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/usr/local/st", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *firefox[]  = { "firejail", "--private=/opt/firefox", "firefox", NULL };
static const char *obs[]  = { "obs", NULL };
static const char *obsidian[]  = { "obsidian", NULL };
static const char *thunderbird[]  = { "thunderbird", NULL };
static const char *gimp[]  = { "gimp", NULL };
static const char *torbrowser[]  = { "firejail", "tor-browser", NULL };
static const char *qbittorrent[]  = { "qbittorrent", NULL };
static const char *vlc[]  = { "vlc", NULL };
static const char *mpd[]  = { "/home/rodfer/.xscripts/mpd.sh", NULL };
static const char *lmms[]  = { "lmms", NULL };
static const char *ardour[]  = { "ardour7", NULL };
static const char *keepassxc[]  = { "keepassxc", NULL };
static const char *burpsuite[]  = { "/home/rodfer/library/burpsuite/BurpSuiteCommunity", NULL };
static const char *kvm[]  = { "virt-manager", NULL };
static const char *screenshot[]  = { "/bin/sh", "-c", "/home/rodfer/.xscripts/screenshot.sh", "/home/rodfer/screenshots", NULL };
static const char *inclight[]  = { "xbacklight", "-inc", "5", NULL };
static const char *declight[]  = { "xbacklight", "-dec", "5", NULL };
static const char *togglesound[]  = { "amixer", "-D", "pulse", "set", "Master", "1+" ,"toggle", NULL };
static const char *incsound[]  = { "amixer", "-D", "pulse", "sset", "Master", "2%+", NULL };
static const char *decsound[]  = { "amixer", "-D", "pulse", "sset", "Master", "2%-", NULL };
static const char *shutdown[]  = { "shutdown", "now", NULL };
//static const char *hibernate[]  = { "systemctl", "hibernate", NULL };
static const char *suspend[]  = { "systemctl", "suspend", NULL };
static const char *nextbg[]  = { "/bin/python3", "/home/rodfer/.xscripts/next_bg.py", NULL };
static const char *bluetooth[] = { "/bin/sh", "home/rodfer/.xscripts/bluetooth.sh", NULL };
static const char *cinema[] = { "kodi", NULL };


static Key keys[] = {
	/* modifier                     key             function        argument */
	{ MODKEY,                       XK_p,           spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return,      spawn,          {.v = termcmd } },
    { MODKEY|ShiftMask,             XK_f,           spawn,          {.v = firefox } },
    { MODKEY|ShiftMask,             XK_p,           spawn,          {.v = qbittorrent } },
    { MODKEY|ShiftMask,             XK_e,           spawn,          {.v = thunderbird } },
    { MODKEY|ShiftMask,             XK_g,           spawn,          {.v = gimp } },
    { MODKEY|ShiftMask,             XK_a,           spawn,          {.v = ardour } },
    { MODKEY|ShiftMask,             XK_t,           spawn,          {.v = torbrowser } },
    { MODKEY|ShiftMask,             XK_o,           spawn,          {.v = obsidian } },
    { MODKEY|ShiftMask,             XK_l,           spawn,          {.v = obs } },
    { MODKEY|ShiftMask,             XK_v,           spawn,          {.v = vlc } },
    { MODKEY|ShiftMask,             XK_m,           spawn,          {.v = mpd} },
    { MODKEY|ShiftMask,             XK_w,           spawn,          {.v = burpsuite} },
    { MODKEY|ShiftMask,             XK_b,           spawn,          {.v = bluetooth } },
    { MODKEY|ShiftMask,             XK_x,           spawn,          {.v = kvm } },
    { MODKEY|ShiftMask,             XK_z,           spawn,          {.v = keepassxc } },
    { MODKEY|ShiftMask,             XK_k,           spawn,          {.v = cinema } },
    { 0,                            XK_Home,        spawn,          {.v = nextbg } },
	{ MODKEY|ShiftMask,             XK_Esc,         spawn,          {.v = shutdown } },
    { MODKEY,                       XK_Esc,         spawn,          {.v = suspend } },
    { 0,                            XK_IncLight,    spawn,          {.v = inclight } },
    { 0,                            XK_DecLight,    spawn,          {.v = declight } },
    { 0,                            XK_ToggleSound, spawn,          {.v = togglesound } },
    { 0,                            XK_DecSound,    spawn,          {.v = decsound } },
    { 0,                            XK_IncSound,    spawn,          {.v = incsound } },
    { 0,                            XK_Bluetooth,   spawn,          {.v = bluetooth } },
	{ MODKEY,                       XK_DecSound,    mpdchange,      {.i = -1} },
	{ MODKEY,                       XK_IncSound,    mpdchange,      {.i = +1} },
	{ MODKEY,                       XK_ToggleSound, mpdcontrol,     {0} },
    { 0,                            XK_PrtSc,       spawn,          {.v = screenshot } },
    { MODKEY,                       XK_p,           togglebar,      {0} },
	{ MODKEY,                       XK_j,           focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,           focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,           incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,           incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_r,           setmfact,       {.f = -0.01} },
	{ MODKEY,                       XK_l,           setmfact,       {.f = +0.01} },
	{ MODKEY,                       XK_Return,      zoom,           {0} },
	{ MODKEY,                       XK_Tab,         view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,           killclient,     {0} },
	{ MODKEY,                       XK_t,           setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_o,           setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_s,           setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,       setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,       togglefloating, {0} },
	{ MODKEY,                       XK_0,           view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,           tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,       focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period,      focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,       tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period,      tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                           0)
	TAGKEYS(                        XK_2,                           1)
	TAGKEYS(                        XK_3,                           2)
	TAGKEYS(                        XK_4,                           3)
	TAGKEYS(                        XK_5,                           4)
	TAGKEYS(                        XK_6,                           5)
	TAGKEYS(                        XK_7,                           6)
	TAGKEYS(                        XK_8,                           7)
	TAGKEYS(                        XK_9,                           8)
	{ MODKEY|ShiftMask,             XK_q,           quit,           {0} },
	{ MODKEY,                       XK_F1,          mpdchange,      {.i = -1} },
	{ MODKEY,                       XK_F2,          mpdchange,      {.i = +1} },
	{ MODKEY,                       XK_Escape,      mpdcontrol,     {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

