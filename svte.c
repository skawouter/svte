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


#include "defaults.h"
#include <gdk/gdkkeysyms.h>
#include <glib/goption.h>
#include <gtk/gtk.h>
#include <vte/vte.h>
#include <unistd.h>

static struct {
  GtkWidget *win;
  GtkWidget *notebook;
  gchar *title;
} svte;


typedef struct term {
  GtkWidget *vte;
  GtkWidget *label;
  GPid *pid;
} term;


static void quit();
gboolean event_key(GtkWidget *widget, GdkEventKey *event);
gboolean event_button(GtkWidget *widget, GdkEventButton *button_event, struct term *t);
static void tab_close();
static char* tab_get_cwd(struct term* t);
static void tab_switch(gboolean b);
static void tab_title();
static void tab_geometry_hints();
static void tab_new();
static void configure_window();
static void tab_focus(GtkNotebook *notebook, GtkWidget *page,
                      guint page_num, gpointer user_data);
static void set_window_title(guint page_num, term *t);


static GQuark term_data_id = 0;
#define get_page_term( sakura, page_idx ) (struct term*)g_object_get_qdata(G_OBJECT( gtk_notebook_get_nth_page( (GtkNotebook*)svte.notebook, page_idx ) ), term_data_id);


static gchar *config_file = NULL;
static gboolean show_version = FALSE;
static GOptionEntry options[] = { 
  { "config", 'c', 0, G_OPTION_ARG_FILENAME, &config_file,
    "Path to configuration file to use.", NULL }, 
  { "version", 0, 0, G_OPTION_ARG_NONE, &show_version,
    "Print version information and exit", NULL }, 
  { NULL } 
}; 


typedef struct {
  gboolean audible_bell;
  gboolean autohide_mouse;
  gboolean allow_bold;
  gchar *font;
  gboolean fullscreen;
  gint num_scrollback_lines;
  gboolean scroll_on_keystroke;
  gboolean scroll_on_output;
  gboolean bg_transparent;
  gdouble bg_saturation;
  gchar *bg_image;
  gchar *url_regex;
  gboolean visible_bell;
  gint window_height;
  gint window_width;
  gchar *word_chars;
  GdkColor foreground;
  GdkColor background;
  GdkColor *colour_palette;
  gchar *cursor;
} Settings;
static Settings *config;


/**
 * Helper function, if i ever need to do something while closing, like query if
 * more than one tab :)
 */
static void quit() {
  gtk_main_quit();
}


gboolean event_key(GtkWidget *widget, GdkEventKey *event) {
  guint(g) = event->keyval;

  if ((event->state & (GDK_CONTROL_MASK|GDK_SHIFT_MASK)) ==
      (GDK_CONTROL_MASK|GDK_SHIFT_MASK)) {
    if (g == GDK_KEY_T) {
      tab_new();
      return TRUE;
    }
    if (g == GDK_KEY_W) {
      tab_close();
      return TRUE;
    }
  }
  if ((event->state & (GDK_MOD1_MASK) ) == (GDK_MOD1_MASK)) {
    if (g == GDK_KEY_Left) {
      tab_switch(FALSE);
      return TRUE;
    }
    if (g == GDK_KEY_Right) {
	  tab_switch(TRUE);
      return TRUE;
    }
    if (g == GDK_KEY_F11) {
      if(config->fullscreen) {
        gtk_window_unfullscreen(GTK_WINDOW(svte.win));
        config->fullscreen = FALSE;
      } else {
        gtk_window_fullscreen(GTK_WINDOW(svte.win));
        config->fullscreen = TRUE;
      }
      return TRUE;
    }
  }
  return FALSE;
}


gboolean event_button(GtkWidget *widget, GdkEventButton *button_event, struct term *t) {
	
  return FALSE;
}


static void tab_close() {
  gint page = gtk_notebook_get_current_page(GTK_NOTEBOOK(svte.notebook));
  struct term *t;
  t = get_page_term(NULL, page);
  gtk_notebook_remove_page(GTK_NOTEBOOK(svte.notebook), page);
  g_free(t);

  if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(svte.notebook)) == 1) {
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(svte.notebook), FALSE);
    gtk_widget_grab_focus(
        gtk_notebook_get_nth_page(GTK_NOTEBOOK(svte.notebook),
        gtk_notebook_get_current_page(GTK_NOTEBOOK(svte.notebook))));
  }
  if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(svte.notebook)) == 0) {
    quit();
  }
}


/* Retrieve the cwd of the specified term page.
 * Original function was from terminal-screen.c of gnome-terminal, copyright (C) 2001 Havoc Pennington
 * Adapted by Hong Jen Yee, non-linux shit removed by David GÃ³mez */
static char* tab_get_cwd(struct term* t)
{
    char *cwd = NULL;

    if (t->pid >= 0) {
        char *file;
        char buf[255+1];
        int len;

        file = g_strdup_printf ("/proc/%d/cwd", t->pid);
        len = readlink (file, buf, sizeof (buf) - 1);

        if (len > 0 && buf[0] == '/') {
            buf[len] = '\0';
            cwd = g_strdup(buf);
        }

        g_free(file);
    }

    return cwd;
}


static void tab_switch(gboolean b) {
  gint(current) = gtk_notebook_get_current_page(GTK_NOTEBOOK(svte.notebook));
  if(b) {
    if (current == gtk_notebook_get_n_pages(GTK_NOTEBOOK(svte.notebook)) -1 ) {
      current = 0;
    } else {
      current = current + 1;
    }
  } else {
    if (current == 0) {
      current = gtk_notebook_get_n_pages(GTK_NOTEBOOK(svte.notebook)) - 1;
    } else {
      current = current -1;
    }
  }
  gtk_notebook_set_current_page(GTK_NOTEBOOK(svte.notebook), current);
}


static void tab_geometry_hints(term *t) {
  // I dont need to call this every time, since the char width only changes
  // once, maybe I'll make hints and border global and reuse them
  GdkGeometry hints;
  GtkBorder *border;
  gint char_width, char_height;
  gtk_widget_style_get(GTK_WIDGET(t->vte), "inner-border", &border, NULL);

  char_width = vte_terminal_get_char_width(VTE_TERMINAL(t->vte));
  char_height = vte_terminal_get_char_height(VTE_TERMINAL(t->vte));

  hints.min_width = char_width + border->left + border->right;
  hints.min_height = char_height + border->top + border->bottom;
  hints.base_width = border->left + border->right;
  hints.base_height = border->top + border->bottom;
  hints.width_inc = char_width;
  hints.height_inc = char_height;

  gtk_window_set_geometry_hints(
      GTK_WINDOW(svte.win),
      GTK_WIDGET(t->vte),
      &hints,
      GDK_HINT_RESIZE_INC | GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE);
}


static void tab_title(GtkWidget *widget, term *t) {
  gtk_label_set_text(
      GTK_LABEL(t->label),
      vte_terminal_get_window_title(VTE_TERMINAL(t->vte)));
}


static void set_window_title(guint page_num, term *t){
  const char *title = vte_terminal_get_window_title(VTE_TERMINAL(t->vte));
  if (title == NULL) {
    title = "svte";
  }
  gtk_window_set_title(GTK_WINDOW(svte.win), title);
}


static void tab_focus(GtkNotebook *notebook, GtkWidget *page,
                      guint page_num, gpointer user_data) {
  struct term *t;
  t = get_page_term(NULL, page_num);
  set_window_title(page_num, t);
}


static void tab_new() {
  term *t;
  int *tmp;
  

  char **args = 0;
  const gchar *shell = g_getenv("SHELL");
  if (!shell) {
    shell = "sh";
  }
  g_shell_parse_argv(shell, 0, &args, 0);

  t = g_new0(term, 1);
  t->label = gtk_label_new("");
  t->vte = vte_terminal_new();
  int index = gtk_notebook_append_page(GTK_NOTEBOOK(svte.notebook), t->vte,
                                       t->label);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(svte.notebook), t->vte, TRUE);

  if (index == 0) {
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(svte.notebook), FALSE);
    vte_terminal_fork_command_full(VTE_TERMINAL(t->vte), 
                VTE_PTY_DEFAULT, NULL, 
                args, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, t->pid, NULL);
    tab_geometry_hints(t);
  } else {
    struct term *previous = get_page_term(NULL, 
      gtk_notebook_get_current_page(GTK_NOTEBOOK(svte.notebook)));

    vte_terminal_fork_command_full(VTE_TERMINAL(t->vte), 
                VTE_PTY_DEFAULT, tab_get_cwd(previous), 
                args, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, t->pid, NULL);
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(svte.notebook), TRUE);
  }
   
  g_object_set_qdata_full(G_OBJECT(gtk_notebook_get_nth_page(
      (GtkNotebook*)svte.notebook, index)), term_data_id, t, NULL);
  g_signal_connect(G_OBJECT(t->vte), "child-exited", G_CALLBACK(tab_close), NULL);
  g_signal_connect(G_OBJECT(t->vte), "window-title-changed", G_CALLBACK(tab_title), t);
  g_signal_connect(G_OBJECT(t->vte), "button-press-event", G_CALLBACK(event_button), t);
  vte_terminal_set_allow_bold(VTE_TERMINAL(t->vte), config->allow_bold);
  vte_terminal_set_audible_bell(VTE_TERMINAL(t->vte), config->audible_bell);
  vte_terminal_set_background_transparent(VTE_TERMINAL(t->vte),
                                          config->bg_transparent);
  vte_terminal_set_background_saturation(VTE_TERMINAL(t->vte), config->bg_saturation);
  vte_terminal_set_background_image_file(VTE_TERMINAL(t->vte), config->bg_image);
  vte_terminal_set_font_from_string(VTE_TERMINAL(t->vte), config->font);
  vte_terminal_set_mouse_autohide(VTE_TERMINAL(t->vte),
                                  config->autohide_mouse);
  vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(t->vte),
                                       config->scroll_on_keystroke);
  vte_terminal_set_scroll_on_output(VTE_TERMINAL(t->vte),
                                    config->scroll_on_output);
  vte_terminal_set_scrollback_lines(VTE_TERMINAL(t->vte),
                                    config->num_scrollback_lines);
  vte_terminal_set_visible_bell(VTE_TERMINAL(t->vte), config->visible_bell);
  vte_terminal_set_word_chars(VTE_TERMINAL(t->vte), config->word_chars);
  vte_terminal_set_colors(VTE_TERMINAL(t->vte), &config->foreground,
                            &config->background, config->colour_palette, DEFAULT_PALETTE_SIZE);

  *tmp = vte_terminal_match_add_gregex(
      VTE_TERMINAL(t->vte),
      g_regex_new(config->url_regex, G_REGEX_CASELESS, G_REGEX_MATCH_NOTEMPTY,
                  NULL),
      0);
  vte_terminal_match_set_cursor_type(VTE_TERMINAL(t->vte), *tmp,
                                     GDK_HAND2);
  g_free(tmp);
  set_window_title(index, t);
  gtk_widget_show_all(svte.notebook);
  gtk_notebook_set_current_page(GTK_NOTEBOOK(svte.notebook), index);
  gtk_widget_grab_focus(t->vte);
}


static void configure_window() {
  term_data_id = g_quark_from_static_string("svte");
  svte.notebook = gtk_notebook_new();
  gtk_notebook_set_show_border(GTK_NOTEBOOK(svte.notebook), FALSE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(svte.notebook), TRUE);
  svte.win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  if (config->fullscreen) {
    gtk_window_fullscreen(GTK_WINDOW(svte.win));
  }
  gtk_window_set_default_size(GTK_WINDOW(svte.win),
                              config->window_width,
                              config->window_height);
  gtk_container_add(GTK_CONTAINER(svte.win), svte.notebook);
  tab_new();
  gtk_widget_show_all(svte.win);
  g_signal_connect(G_OBJECT(svte.win), "destroy", G_CALLBACK(quit), NULL);
  g_signal_connect(svte.win, "key-press-event", G_CALLBACK(event_key), NULL);
  g_signal_connect(G_OBJECT(svte.notebook), "switch-page", G_CALLBACK(tab_focus),
                   NULL);
} 


static gboolean parse_command_line_options(int argc, char* argv[]) {
  gboolean retval = TRUE;
  GError *error = NULL;
  GOptionContext *context;
  context = g_option_context_new("- a simple, tabbed, VTE based terminal");
  g_option_context_add_main_entries(context, options, NULL);
  g_option_context_add_group(context, gtk_get_option_group(TRUE));
  if (!g_option_context_parse(context, &argc, &argv, &error)) {
    g_error("Error parsing command-line options: %s\n", error->message);
    retval = FALSE;
  }
  return retval;
}


static void parse_config_file(gchar *config_file) {
  GKeyFile *keyfile;
  GKeyFileFlags flags;
  GError *error = NULL;
  gsize length;
  gchar *addid; 
  addid = (gchar *) g_malloc (3);

  keyfile = g_key_file_new();
  if (!g_key_file_load_from_file(keyfile, config_file, flags, &error)) {
    g_warning("Error parsing config file %s: %s\n",
              config_file,
              error->message);
  }

  config = g_slice_new(Settings);
  config->audible_bell = g_key_file_get_boolean(
      keyfile, "ui", "audible_bell", NULL);
  config->autohide_mouse = g_key_file_get_boolean(
      keyfile, "ui", "autohide_mouse", NULL);
  config->allow_bold = g_key_file_get_boolean(
      keyfile, "ui", "allow_bold", NULL);
  config->font = g_key_file_get_string(
      keyfile, "ui", "font", NULL);
  config->fullscreen = g_key_file_get_boolean(
      keyfile, "ui", "fullscreen", NULL);
  config->num_scrollback_lines = g_key_file_get_integer(
      keyfile, "ui", "num_scrollback_lines", NULL);
  config->scroll_on_keystroke = g_key_file_get_boolean(
      keyfile, "ui", "scroll_on_keystroke", NULL);
  config->scroll_on_output = g_key_file_get_boolean(
      keyfile, "ui", "scroll_on_output", NULL);
  config->bg_transparent = g_key_file_get_boolean(
      keyfile, "ui", "bg_transparent", NULL);
  config->bg_image = g_key_file_get_string(
      keyfile, "ui", "bg_image", NULL);
  config->bg_saturation = g_key_file_get_double(
      keyfile, "ui", "bg_saturation", NULL);
  config->url_regex = g_key_file_get_string(
      keyfile, "ui", "url_regex", NULL);
  config->visible_bell = g_key_file_get_boolean(
      keyfile, "ui", "visible_bell", NULL);
  config->window_height = g_key_file_get_integer(
      keyfile, "ui", "window_height", NULL);
  config->window_width = g_key_file_get_integer(
      keyfile, "ui", "window_width", NULL);
  config->word_chars = g_key_file_get_string(
      keyfile, "ui", "word_chars", NULL);
  config->cursor = g_key_file_get_string(
      keyfile, "colour scheme", "cursor", NULL);

  config->colour_palette = (GdkColor *) g_malloc(sizeof(GdkColor) * DEFAULT_PALETTE_SIZE);
  for (int i=0; i < DEFAULT_PALETTE_SIZE; i++){
    g_snprintf(addid, 3, "%d", i);
    gdk_color_parse(g_key_file_get_string(keyfile, "colour scheme", 
                                            addid , NULL), &config->colour_palette[i]);
  }

  if (!gdk_color_parse(g_key_file_get_string(
          keyfile, "colour scheme", "foreground", NULL), &config->foreground)){
      gdk_color_parse(DEFAULT_FOREGROUND_COLOR, &config->foreground);
      g_warning("Using default foreground color");
  }
  
  if (!gdk_color_parse(g_key_file_get_string(
          keyfile, "colour scheme", "background", NULL), &config->background)){
      gdk_color_parse(DEFAULT_BACKGROUND_COLOR, &config->background);
      g_warning("Using default background color");
  }

  if (NULL == config->font) {
    config->font = DEFAULT_FONT;
  }

  if (NULL == config->url_regex) {
    config->url_regex = DEFAULT_URL_REGEX;
  }

  g_free(addid);
}


int main(int argc, char* argv[]) {
  gtk_init(&argc, &argv);

  if(!parse_command_line_options(argc, argv)) {
    return 1;
  }

  if (show_version) {
    printf(VERSION);
    return 0;
  }

  if (config_file == NULL) {
    config_file = DEFAULT_CONFIG_FILE;
  }
  parse_config_file(config_file);

  configure_window();
  gtk_main();
  return 0;
}
