#define __FILEVERSION_STRING        1,3,0,8
#define __VERSION_STRING            "1.3.0.8"
#define __VERSION_DWORD             0x01030008

#if defined (_WIN64)
#define __PLUGIN_NAME "MenuItemEx RM (x64) plugin for Miranda IM"
#define __PLUGIN_ID 0
#define __PLUGIN_ID_STR "0"
#elif  (_UNICODE)
#define __PLUGIN_NAME "MenuItemEx RM (Unicode) plugin for Miranda IM"
#define __PLUGIN_ID 0
#define __PLUGIN_ID_STR "0"
#else
#define __PLUGIN_NAME "MenuItemEx RM plugin for Miranda IM"
#define __PLUGIN_ID 0
#define __PLUGIN_ID_STR "0"
#endif

#define __WEB "http://addons.miranda-im.org/details.php?action=viewfile&id="

#define __DESC "Adds some useful options to a contacts menu."
#define __AUTHORS "Heiko Schillinger, Baloo, Billy_Bons, Robert Posel"
#define __EMAIL "micron@nexgo.de, baloo@bk.ru, tatarinov.sergey@gmail.com,robyer@seznam.cz"
#define __COPYRIGHTS "© 2001-03 Heiko Schillinger, © 2006-08 Baloo, © 2009-10 Billy_Bons, © 2011 Robert Posel"
#ifdef _UNICODE
// {BEC1DD5C-69C3-438f-91F0-2F8DAA6210DC}
#define MIID_MENUEX		{0xbec1dd5c, 0x69c3, 0x438f, { 0x91, 0xf0, 0x2f, 0x8d, 0xaa, 0x62, 0x10, 0xdc }}
#else
// {E59D33F8-DB5B-461b-9EE7-B4FA930AA583}
#define MIID_MENUEX		{0xe59d33f8, 0xdb5b, 0x461b, { 0x9e, 0xe7, 0xb4, 0xfa, 0x93, 0xa, 0xa5, 0x83 }}
#endif
