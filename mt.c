/* mt: A simple, tabbed, VTE based terminal
 * Copyright 2010 mutantturkey and mt contributors.
 *
 * This file is part of mt.
 *
 * mt is free software: you can redistribute it and/or modify
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


#include <gdk/gdkkeysyms.h>
#include <glib/goption.h>
#include <gtk/gtk.h>
#include <vte/vte.h>


static struct {
  GtkWidget *win;
  GtkWidget *notebook;
  gchar *title;
} mt;


typedef struct term {
  GtkWidget *vte;
  GtkWidget *label;
} term;


static void quit();
gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event);
<<<<<<< HEAD
gboolean button_press_cb (GtkWidget *widget, GdkEventButton *event);
=======
gboolean button_press_cb(GtkWidget *widget, GdkEventButton *event,
                         struct term *t);
>>>>>>> vicfryzel/master
static void tab_close();
static void tab_title();
static void tab_geometry_hints();
static void tab_new();
static void config();
static void tab_focus(GtkNotebook *notebook, GtkNotebookPage *page,
                      guint page_num, gpointer user_data);


static GQuark term_data_id = 0;
#define get_page_term( sakura, page_idx ) (struct term*)g_object_get_qdata(G_OBJECT( gtk_notebook_get_nth_page( (GtkNotebook*)mt.notebook, page_idx ) ), term_data_id);


static gchar *font = "monospace 10";
static gboolean fullscreen = FALSE;
static int scroll = -1;
static gboolean transparent = FALSE;
static gchar *url_regex = "(ftp|http)s?://[-a-zA-Z0-9.?$%&/=_~#.,:;+]*";
static gboolean version = FALSE;

static GOptionEntry options[] = { 
  { "font", 'f', 0, G_OPTION_ARG_STRING, &font,
    "Font to use for displaying text", NULL }, 
  { "fullscreen", 'l', 0, G_OPTION_ARG_NONE, &fullscreen,
    "Start in fullsreen mode", NULL }, 
  { "scroll", 's', 0, G_OPTION_ARG_INT, &scroll,
    "Number of lines to scrollback.  -1 for unlimited", NULL },
  { "transparent", 't', 0, G_OPTION_ARG_NONE, &transparent,
    "Use a transparent background", NULL }, 
  { "urlregex", 'u', 0, G_OPTION_ARG_STRING, &url_regex,
    "Regular expression to use to display URLs as clickable", NULL },
  { "version", 0, 0, G_OPTION_ARG_NONE, &version,
    "Print version information and exit", NULL }, 
  { NULL } 
}; 


/**
 * Helper function, if i ever need to do something while closing, like query if
 * more than one tab :)
 */
static void quit() {
  gtk_main_quit();
}

<<<<<<< HEAD
gboolean key_press_cb (GtkWidget *widget, GdkEventKey *event) {	
	guint(g) = event->keyval;
			 
if ( event->state  == (GDK_CONTROL_MASK|GDK_SHIFT_MASK) ) {
 if (g == GDK_T) { tab_new(); return TRUE;	}
 if (g == GDK_W) { tab_close();	return TRUE; }
	}
if ( (event->state & (GDK_MOD1_MASK) ) == (GDK_MOD1_MASK))  {
 if (g == GDK_Left) { gtk_notebook_prev_page(GTK_NOTEBOOK(mt.notebook)); return TRUE;  }
 if (g == GDK_Right) { gtk_notebook_next_page(GTK_NOTEBOOK(mt.notebook)); 	return TRUE; }
 if (g == GDK_F11) { if(fullscreen) { gtk_window_unfullscreen(GTK_WINDOW(mt.win)); fullscreen = FALSE; } else { gtk_window_fullscreen(GTK_WINDOW(mt.win)); fullscreen = TRUE; } return TRUE;  }
	}
			return FALSE;
}

gboolean button_press_cb (GtkWidget *widget, GdkEventButton *event) {
	puts("LOL");
/*	glong column, row;
	gchar *match;
	   match = vte_terminal_match_check(VTE_TERMINAL(t->vte), column, row, NULL);		
	   printf("%d , %d", column, row);
	   printf("LOL");
	   puts(match);
	   return TRUE;*/
=======

gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event) {
  guint(g) = event->keyval;
  if ((event->state &
      (GDK_CONTROL_MASK|GDK_SHIFT_MASK) == (GDK_CONTROL_MASK|GDK_SHIFT_MASK))) {
    if (g == GDK_T) {
      tab_new();
      return TRUE;
    }
    if (g == GDK_W) {
      tab_close();
      return TRUE;
    }
  }
  if ((event->state & (GDK_MOD1_MASK) ) == (GDK_MOD1_MASK)) {
    if (g == GDK_Left) {
      gtk_notebook_prev_page(GTK_NOTEBOOK(mt.notebook));
      return TRUE;
    }
    if (g == GDK_Right) {
      gtk_notebook_next_page(GTK_NOTEBOOK(mt.notebook));
      return TRUE;
    }
    if (g == GDK_F11) {
      if(fullscreen) {
        gtk_window_unfullscreen(GTK_WINDOW(mt.win));
        fullscreen = FALSE;
      } else {
        gtk_window_fullscreen(GTK_WINDOW(mt.win)); fullscreen = TRUE;
      }
      return TRUE;
    }
  }
  return FALSE;
}


gboolean button_press_cb(GtkWidget *widget, GdkEventButton *event, struct term *t) {
  puts("LOL");
/*
  glong column, row;
  gchar *match;
  match = vte_terminal_match_check(VTE_TERMINAL(t->vte), column, row, NULL);
  printf("%d , %d", column, row);
  printf("LOL");
  puts(match);
  return TRUE;
*/
>>>>>>> vicfryzel/master
}


static void tab_close() {
  gint page = gtk_notebook_get_current_page(GTK_NOTEBOOK(mt.notebook));
  struct term *t;
  t = get_page_term(NULL, page);
  gtk_notebook_remove_page(GTK_NOTEBOOK(mt.notebook), page);
  g_free(t);

  if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(mt.notebook)) == 1) {
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(mt.notebook), FALSE);
    gtk_widget_grab_focus(
        gtk_notebook_get_nth_page(GTK_NOTEBOOK(mt.notebook),
        gtk_notebook_get_current_page(GTK_NOTEBOOK(mt.notebook))));
  }
  if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(mt.notebook)) == 0) {
    quit();
  }
}


static void tab_geometry_hints(term *t) {
  // borrowed from sakura, but using non depreacated code patch by me :)
  // I dont need to call this every time, since the char width only changes
  // once, maybe I'll make hints and border global and reuse them
  GdkGeometry hints;
  GtkBorder *border;
  gint pad_x, pad_y;
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
      GTK_WINDOW(mt.win),
      GTK_WIDGET(t->vte),
      &hints,
      GDK_HINT_RESIZE_INC | GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE);
}


static void tab_title(GtkWidget *widget, term *t) {
  gtk_label_set_text(
      GTK_LABEL(t->label),
      vte_terminal_get_window_title(VTE_TERMINAL(t->vte)));
}


static void tab_focus(GtkNotebook *notebook, GtkNotebookPage *page,
                      guint page_num, gpointer user_data) {
  struct term *t;
  t = get_page_term(NULL, page_num);
  const char *title = vte_terminal_get_window_title(VTE_TERMINAL(t->vte));
  if (title == NULL) {
    title = "mt";
  }
  gtk_window_set_title(GTK_WINDOW(mt.win), title);
}


static void tab_new() {
<<<<<<< HEAD
	
	term *t, previous;
	int *tmp;
	t = g_new0(term, 1);
	t->label = gtk_label_new("");
	t->vte = vte_terminal_new();
	
	int index = gtk_notebook_append_page(GTK_NOTEBOOK(mt.notebook), t->vte, t->label);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(mt.notebook), t->vte, TRUE);
	
	if ( gtk_notebook_get_n_pages(GTK_NOTEBOOK(mt.notebook)) == 1) { tab_geometry_hints(t);	}
	if (index == 0) {
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(mt.notebook), FALSE);
	} else  { gtk_notebook_set_show_tabs(GTK_NOTEBOOK(mt.notebook), TRUE); }


	
	vte_terminal_fork_command(VTE_TERMINAL(t->vte), NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE);
	g_object_set_qdata_full(G_OBJECT(gtk_notebook_get_nth_page((GtkNotebook*)mt.notebook, index)), term_data_id, t, NULL);
	g_signal_connect(t->vte, "child-exited", G_CALLBACK(tab_close), NULL);
	g_signal_connect(t->vte, "window-title-changed", G_CALLBACK(tab_title), t);
	*tmp = vte_terminal_match_add_gregex(VTE_TERMINAL(t->vte), g_regex_new("(ftp|http)s?://[-a-zA-Z0-9.?$%&/=_~#.,:;+]*", G_REGEX_CASELESS, G_REGEX_MATCH_NOTEMPTY, NULL), 0);
	vte_terminal_match_set_cursor_type(VTE_TERMINAL(t->vte), *tmp, GDK_CROSSHAIR);
	g_free (tmp);
	vte_terminal_set_scrollback_lines(VTE_TERMINAL(t->vte), -1);
	vte_terminal_set_mouse_autohide(VTE_TERMINAL(t->vte), TRUE);
	vte_terminal_set_font_from_string(VTE_TERMINAL(t->vte), FONT);
	gtk_window_set_title(GTK_WINDOW(mt.win), vte_terminal_get_window_title(VTE_TERMINAL(t->vte)));
	gtk_widget_show_all(mt.notebook);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(mt.notebook), index);
	gtk_widget_grab_focus(t->vte);
	}
static void config(){
	
	term_data_id = g_quark_from_static_string("mt");
    mt.notebook = gtk_notebook_new();
    gtk_notebook_set_show_border(GTK_NOTEBOOK(mt.notebook), FALSE);
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(mt.notebook), TRUE);
    mt.win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
 	gtk_window_set_default_size(GTK_WINDOW(mt.win), 500, 350);
    gtk_container_add (GTK_CONTAINER(mt.win), mt.notebook);
	tab_new();
    gtk_widget_show_all(mt.win);
    g_signal_connect (G_OBJECT (mt.win), "destroy", G_CALLBACK (quit), NULL);    
    g_signal_connect(mt.win, "key-press-event", G_CALLBACK(key_press_cb), NULL); 
	g_signal_connect(mt.win, "button-press-event", G_CALLBACK(button_press_cb), NULL);
	g_signal_connect (G_OBJECT(mt.notebook), "switch-page", G_CALLBACK(tab_focus), NULL);
=======
  term *t, previous;
  int *tmp;
  t = g_new0(term, 1);
  t->label = gtk_label_new("");
  t->vte = vte_terminal_new();

  int index = gtk_notebook_append_page(GTK_NOTEBOOK(mt.notebook), t->vte, t->label);
  gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(mt.notebook), t->vte, TRUE);

  if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(mt.notebook)) == 1) {
    tab_geometry_hints(t);
  }
  if (index == 0) {
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(mt.notebook), FALSE);
  } else {
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(mt.notebook), TRUE);
  }

  vte_terminal_fork_command(VTE_TERMINAL(t->vte), NULL, NULL, NULL, NULL,
                            FALSE, FALSE, FALSE);
  g_object_set_qdata_full(G_OBJECT(gtk_notebook_get_nth_page(
      (GtkNotebook*)mt.notebook, index)), term_data_id, t, NULL);
  g_signal_connect(t->vte, "child-exited", G_CALLBACK(tab_close), NULL);
  g_signal_connect(t->vte, "window-title-changed", G_CALLBACK(tab_title), t);
  g_signal_connect(mt.win, "button-press-event", G_CALLBACK(button_press_cb),
                   t);
  vte_terminal_set_background_transparent(VTE_TERMINAL(t->vte), transparent);

  *tmp = vte_terminal_match_add_gregex(
      VTE_TERMINAL(t->vte),
      g_regex_new(url_regex, G_REGEX_CASELESS, G_REGEX_MATCH_NOTEMPTY, NULL),
                  0);
  vte_terminal_match_set_cursor_type(VTE_TERMINAL(t->vte), *tmp,
                                     GDK_CROSSHAIR);
  g_free(tmp);
  // borrowed from sakura
  vte_terminal_set_scrollback_lines(VTE_TERMINAL(t->vte), scroll);
  vte_terminal_set_mouse_autohide(VTE_TERMINAL(t->vte), TRUE);
  vte_terminal_set_font_from_string(VTE_TERMINAL(t->vte), font);
  gtk_window_set_title(GTK_WINDOW(mt.win),
                       vte_terminal_get_window_title(VTE_TERMINAL(t->vte)));
  gtk_widget_show_all(mt.notebook);
  gtk_notebook_set_current_page(GTK_NOTEBOOK(mt.notebook), index);
  gtk_widget_grab_focus(t->vte);
>>>>>>> vicfryzel/master
}


static void config() {
  term_data_id = g_quark_from_static_string("mt");
  mt.notebook = gtk_notebook_new();
  gtk_notebook_set_show_border(GTK_NOTEBOOK(mt.notebook), FALSE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(mt.notebook), TRUE);
  mt.win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  if (fullscreen) {
    gtk_window_fullscreen(GTK_WINDOW(mt.win));
  }
  gtk_window_set_default_size(GTK_WINDOW(mt.win), 500, 350);
  gtk_container_add(GTK_CONTAINER(mt.win), mt.notebook);
  tab_new();
  gtk_widget_show_all(mt.win);
  g_signal_connect(G_OBJECT(mt.win), "destroy", G_CALLBACK(quit), NULL);
  g_signal_connect(mt.win, "key-press-event", G_CALLBACK(key_press_cb), NULL);
  g_signal_connect(G_OBJECT(mt.notebook), "switch-page", G_CALLBACK(tab_focus),
                   NULL);
} 


int main(int argc, char* argv[]) {
  gtk_init(&argc, &argv);
  GError *error = NULL;
  GOptionContext *context;
  context = g_option_context_new("- a simple, tabbed, VTE based terminal");
  g_option_context_add_main_entries(context, options, NULL);
  g_option_context_add_group(context, gtk_get_option_group(TRUE));
  if (!g_option_context_parse(context, &argc, &argv, &error)) {
    g_print("option parsing failed: %s\n", error->message);
    return 1;
  }
  if (version) {
    printf(VERSION);
    return 0;
  }
  config();
  gtk_main();
  return 0;
};
