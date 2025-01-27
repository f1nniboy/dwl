/* Taken from https://github.com/djpohly/dwl/issues/466 */
#define COLOR(hex)    { ((hex >> 24) & 0xFF) / 255.0f, \
                        ((hex >> 16) & 0xFF) / 255.0f, \
                        ((hex >> 8) & 0xFF) / 255.0f, \
                        (hex & 0xFF) / 255.0f }

/* appearance */
static const int sloppyfocus				= 1;  /* focus follows mouse */
static const int bypass_surface_visibility	= 0;  /* 1 means idle inhibitors will disable idle tracking even if it's surface isn't visible  */
static const int smartborders				= 1;
static const unsigned int borderpx			= 2;  /* border pixel of windows */
static const float rootcolor[]				= COLOR(0x11111bff);
static const float bordercolor[]			= COLOR(0x181825ff);
static const float focuscolor[]				= COLOR(0xf38ba8ff);
static const float urgentcolor[]			= COLOR(0xfab387ff);
/* This conforms to the xdg-protocol. Set the alpha to zero to restore the old behavior */
static const float fullscreen_bg[]			= {0.1f, 0.1f, 0.1f, 1.0f}; /* You can also use glsl colors */

enum {
    VIEW_L = -1,
    VIEW_R = 1,
    SHIFT_L = -2,
    SHIFT_R = 2,
} RotateTags;

/* tagging - TAGCOUNT must be no greater than 31 */
#define TAGCOUNT (9)

/* logging */
static int log_level = WLR_ERROR;

/* NOTE: ALWAYS keep a rule declared even if you don't use rules (e.g leave at least one example) */
static const Rule rules[] = {
	/* app_id						title						tags mask	isfloating	monitor */
	/* examples: */
	{ "Gimp_EXAMPLE",				NULL,						0,			1,			-1 }, /* Start on currently visible tags floating, not tiled */
	{ "firefox_EXAMPLE",			NULL,						1 << 8,		0,			-1 }, /* Start on ONLY tag "9" */

	{ "xdg-desktop-portal-gtk",		NULL,						0,			1,			-1 },
	{ "file-picker",				NULL,						0,			1,			-1 },
	{ NULL,							"Steam-Einstellungen",		0,			1,			-1 },
	{ "firefox",					"Library",					0,			1,			-1 },
	{ "Pinentry-gtk",				NULL,						0,			1,			-1 },
};

/* layout(s) */
static const Layout layouts[] = {
	/* symbol		arrange function */
	{ "[]=",		tilewide },
	{ NULL,			NULL }, /* terminate */
};

/* monitors */
/* (x=-1, y=-1) is reserved as an "autoconfigure" monitor position indicator
 * WARNING: negative values other than (-1, -1) cause problems with Xwayland clients
 * https://gitlab.freedesktop.org/xorg/xserver/-/issues/899
*/
/* NOTE: ALWAYS add a fallback rule, even if you are completely sure it won't be used */
static const MonitorRule monrules[] = {
	/* name	mfact	nmaster	scale	layout			rotate/reflect				x	y */
	{ NULL,	0.7f,	1,		1,		&layouts[0],	WL_OUTPUT_TRANSFORM_NORMAL,	-1,	-1 },
};

/* keyboard */
static const struct xkb_rule_names xkb_rules = {
	.options = "caps:escape",
	.layout = "de",
};

static const int repeat_rate = 25;
static const int repeat_delay = 600;

/* Trackpad */
static const int tap_to_click = 1;
static const int tap_and_drag = 1;
static const int drag_lock = 1;
static const int natural_scrolling = 0;
static const int disable_while_typing = 1;
static const int left_handed = 0;
static const int middle_button_emulation = 0;
/* You can choose between:
LIBINPUT_CONFIG_SCROLL_NO_SCROLL
LIBINPUT_CONFIG_SCROLL_2FG
LIBINPUT_CONFIG_SCROLL_EDGE
LIBINPUT_CONFIG_SCROLL_ON_BUTTON_DOWN
*/
static const enum libinput_config_scroll_method scroll_method = LIBINPUT_CONFIG_SCROLL_2FG;

/* You can choose between:
LIBINPUT_CONFIG_CLICK_METHOD_NONE
LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS
LIBINPUT_CONFIG_CLICK_METHOD_CLICKFINGER
*/
static const enum libinput_config_click_method click_method = LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS;

/* You can choose between:
LIBINPUT_CONFIG_SEND_EVENTS_ENABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED
LIBINPUT_CONFIG_SEND_EVENTS_DISABLED_ON_EXTERNAL_MOUSE
*/
static const uint32_t send_events_mode = LIBINPUT_CONFIG_SEND_EVENTS_ENABLED;

/* You can choose between:
LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT
LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE
*/
static const enum libinput_config_accel_profile accel_profile = LIBINPUT_CONFIG_ACCEL_PROFILE_FLAT;
static const double accel_speed = 0.0;

/* You can choose between:
LIBINPUT_CONFIG_TAP_MAP_LRM -- 1/2/3 finger tap maps to left/right/middle
LIBINPUT_CONFIG_TAP_MAP_LMR -- 1/2/3 finger tap maps to left/middle/right
*/
static const enum libinput_config_tap_button_map button_map = LIBINPUT_CONFIG_TAP_MAP_LRM;

/* If you want to use the windows key for MODKEY, use WLR_MODIFIER_LOGO */
#ifndef MODKEY
#       define MODKEY WLR_MODIFIER_LOGO
#endif

#define TAGKEYS(KEY,SKEY,TAG) \
	{ MODKEY,                    KEY,            view,            {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL,  KEY,            toggleview,      {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_SHIFT, SKEY,           tag,             {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL|WLR_MODIFIER_SHIFT,SKEY,toggletag, {.ui = 1 << TAG} }

#define SCRIPTCMD(...) { .v = (const char*[]){ "/home/etc/scripts/" __VA_ARGS__, NULL } }
#define CMD(...) { .v = (const char*[]){ __VA_ARGS__, NULL } }

static const Key keys[] = {
	/* Note that Shift changes certain key codes: c -> C, 2 -> at, etc. */
	/* modifier					 	key				 				function			argument */
	/* starting at full screen resolution fixes the flickering issue https://codeberg.org/dwl/dwl/issues/705 */
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_Return,					spawn,				CMD("foot", "-m", "-w 3440x1440") },
	{ MODKEY,						XKB_KEY_b,						togglebar,			{0} },
	{ MODKEY,						XKB_KEY_j,						focusstack,			{.i = -1} },
	{ MODKEY,						XKB_KEY_k,						focusstack,			{.i = +1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_J,						movestack,			{.i = -1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_K,						movestack,			{.i = +1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_H,						incnmaster,			{.i = +1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_L,						incnmaster,			{.i = -1} },
	{ MODKEY,						XKB_KEY_h,						setmfact,			{.f = -0.05f} },
	{ MODKEY,						XKB_KEY_l,						setmfact,			{.f = +0.05f} },
	{ MODKEY,						XKB_KEY_space,					zoom,				{0} },
	{ MODKEY,						XKB_KEY_Tab,					view,				{0} },
	{ MODKEY,						XKB_KEY_q,						killclient,			{0} },
	{ MODKEY,						XKB_KEY_t,						nextlayout,			{0} },
	{ MODKEY,						XKB_KEY_c,						togglefloating, 	{0} },
	{ MODKEY,						XKB_KEY_f,						togglefullscreen,	{0} },
	{ MODKEY,						XKB_KEY_a,						rotatetags,			{.i = VIEW_L} },
	{ MODKEY,						XKB_KEY_d,						rotatetags,			{.i = VIEW_R} },
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_A,						rotatetags,			{.i = SHIFT_L} },
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_D,						rotatetags,			{.i = SHIFT_R} },
	{ MODKEY,						XKB_KEY_0,						view,				{.ui = ~0} },
	//{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_parenright, 			tag,				{.ui = ~0} },
	{ MODKEY,						XKB_KEY_comma,					focusmon,			{.i = WLR_DIRECTION_LEFT} },
	{ MODKEY,						XKB_KEY_period,					focusmon,			{.i = WLR_DIRECTION_RIGHT} },
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_less,					tagmon,				{.i = WLR_DIRECTION_LEFT} },
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_greater,				tagmon,				{.i = WLR_DIRECTION_RIGHT} },
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_Q,						quit,				{0} },
	{ MODKEY,						XKB_KEY_Return,					spawn,				SCRIPTCMD("keys/launcher") },
	{ 0,							XKB_KEY_XF86AudioRaiseVolume,	spawn,				SCRIPTCMD("keys/volume", "@DEFAULT_AUDIO_SINK@", "+") },
	{ 0,							XKB_KEY_XF86AudioLowerVolume,	spawn,				SCRIPTCMD("keys/volume", "@DEFAULT_AUDIO_SINK@", "-") },
	{ 0,							XKB_KEY_XF86AudioPlay,			spawn,				SCRIPTCMD("player", "play-pause") },
	{ 0,							XKB_KEY_XF86AudioNext,			spawn,				SCRIPTCMD("player", "next") },
	{ 0,							XKB_KEY_XF86AudioPrev,			spawn,				SCRIPTCMD("player", "previous") },
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_S,						spawn,				SCRIPTCMD("keys/screenshot") },
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_R,						spawn,				SCRIPTCMD("keys/save-replay") },
	{ MODKEY,						XKB_KEY_Delete,					spawn,				SCRIPTCMD("keys/power") },
	{ MODKEY,						XKB_KEY_w,						spawn,				SCRIPTCMD("keys/utils") },
	{ MODKEY,						XKB_KEY_e,						spawn,				SCRIPTCMD("keys/emoji") },
	{ MODKEY,						XKB_KEY_p,						spawn,				SCRIPTCMD("keys/clipboard", "copy") },
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_P,						spawn,				SCRIPTCMD("keys/clipboard", "delete") },
	{ MODKEY,						XKB_KEY_n,						spawn,				CMD("makoctl", "menu", "/home/etc/scripts/handle-notif") },
	{ MODKEY|WLR_MODIFIER_SHIFT,	XKB_KEY_N,						spawn,				CMD("makoctl", "dismiss") },
	TAGKEYS(		  XKB_KEY_1,	XKB_KEY_exclam,					0),
	TAGKEYS(		  XKB_KEY_2,	XKB_KEY_quotedbl,				1),
	TAGKEYS(		  XKB_KEY_3,	XKB_KEY_section,				2),
	TAGKEYS(		  XKB_KEY_4,	XKB_KEY_dollar,					3),
	TAGKEYS(		  XKB_KEY_5,	XKB_KEY_percent,				4),
	TAGKEYS(		  XKB_KEY_6,	XKB_KEY_asciicircum,			5),
	TAGKEYS(		  XKB_KEY_7,	XKB_KEY_slash,					6),
	TAGKEYS(		  XKB_KEY_8,	XKB_KEY_parenleft,				7),
	TAGKEYS(		  XKB_KEY_9,	XKB_KEY_parenright,				8),


	/* Ctrl-Alt-Backspace and Ctrl-Alt-Fx used to be handled by X server */
	{ WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_Terminate_Server, quit, {0} },
	/* Ctrl-Alt-Fx is used to switch to another VT, if you don't know what a VT is
	 * do not remove them.
	 */
#define CHVT(n) { WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_XF86Switch_VT_##n, chvt, {.ui = (n)} }
	CHVT(1), CHVT(2), CHVT(3), CHVT(4), CHVT(5), CHVT(6),
	CHVT(7), CHVT(8), CHVT(9), CHVT(10), CHVT(11), CHVT(12),
};

static const Button buttons[] = {
	{ MODKEY,	BTN_LEFT,	moveresize,		{.ui = CurMove} },
	{ MODKEY,	BTN_MIDDLE,	togglefloating,	{0} },
	{ MODKEY,	BTN_RIGHT,	moveresize,		{.ui = CurResize} },
	{ 0,		BTN_EXTRA,	spawn,			SCRIPTCMD("keys/mute", "@DEFAULT_AUDIO_SOURCE@") },
};
