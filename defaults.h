/* svte: simple virtual terminal emulator: minimal, tabbed, VTE-based terminal
 * Copyright 2010 mutantturkey and svte contributors.
 *
 * This file is part of svte.
 *
 * svte is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef DEFAULT_CONFIG_FILE
#define DEFAULT_CONFIG_FILE g_build_path("/", g_get_home_dir(), ".svterc", NULL);
#endif
#ifndef DEFAULT_FONT
#define DEFAULT_FONT "monospace 10"
#endif
#ifndef DEFAULT_URL_REGEX
#define DEFAULT_URL_REGEX "(ftp|http)s?://[-a-zA-Z0-9.?$%&/=_~#.,:;+]*"
#endif

// The following are not yet used.
#ifndef DEFAULT_FULLSCREEN
#define DEFAULT_FULLSCREEN TRUE
#endif
#ifndef DEFAULT_ALLOW_BOLD
#define DEFAULT_ALLOW_BOLD TRUE
#endif
#ifndef DEFAULT_NUM_SCROLLBACK_LINES
#define DEFAULT_NUM_SCROLLBACK_LINES -1
#endif
#ifndef DEFAULT_TRANSPARENT_BG
#define DEFAULT_TRANSPARENT_BG FALSE
#endif
#ifndef DEFAULT_PALETTE_SIZE
#define DEFAULT_PALETTE_SIZE 16
#endif
#define DEFAULT_FOREGROUND_COLOR "#c0c0c0"
#define DEFAULT_BACKGROUND_COLOR "#000"
