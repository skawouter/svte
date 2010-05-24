#include <gtk/gtk.h>
#include <vte/vte.h>
#include <gdk/gdkkeysyms.h>

static void quit();
gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event);
static void tab_close();
static void tab_title();
static void tab_geometry_hints();
static void tab_new();
static void config();
static void tab_focus(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data);
/* part of the code is from sakura. :) i just want a more minimal version, sakura is to code bloat for what i want. so snippets will do*/
/* i am very thankful for this part, because it really helped me figure out how to close tabs properly (sounds easy, but i am dumb as a doorknob*/
static GQuark term_data_id = 0;
#define  get_page_term( sakura, page_idx ) (struct term*)g_object_get_qdata(G_OBJECT( gtk_notebook_get_nth_page( (GtkNotebook*)mt.notebook, page_idx ) ), term_data_id);
static gboolean fullscreen;
static char *font = "terminus 9";
static long scroll = 250;
static char *httpregexp =  "(ftp|http)s?://[-a-zA-Z0-9.?$%&/=_~#.,:;+]*";

static struct {	GtkWidget *win;	GtkWidget *notebook; gchar *title;} mt;
typedef struct term { GtkWidget *vte; GtkWidget *label; } term;

/*helper function, if i ever need to do something while closing, like query if more than one tab :)*/
static void quit() {
	gtk_main_quit();
}

gboolean key_press_cb (GtkWidget *widget, GdkEventKey *event) {	
	guint(g) = event->keyval;
if (event->state == (GDK_CONTROL_MASK|GDK_SHIFT_MASK)) {
 if (g == GDK_t) { tab_new(); return TRUE;	}
 if (g == GDK_x) { tab_close();	return TRUE; }	
	}
if (event->state == (GDK_MOD1_MASK))  {
 if (g == GDK_Left) { gtk_notebook_prev_page(GTK_NOTEBOOK(mt.notebook)); return TRUE;  }
 if (g == GDK_Right) { gtk_notebook_next_page(GTK_NOTEBOOK(mt.notebook)); 	return TRUE; }
 if (g == GDK_F11) { if(fullscreen) { gtk_window_unfullscreen(GTK_WINDOW(mt.win)); fullscreen = FALSE; } else { gtk_window_fullscreen(GTK_WINDOW(mt.win)); fullscreen = TRUE; }   }
	}
	
 
		return FALSE;
}

gboolean button_press_cb (GtkWidget *widget, GdkEventButton *event) {
	
}
static void tab_close() {
	gint page = gtk_notebook_get_current_page(GTK_NOTEBOOK(mt.notebook));
	struct term *t;
	t = get_page_term(NULL, page);
	gtk_notebook_remove_page(GTK_NOTEBOOK(mt.notebook), page);
	g_free(t);
	
	
	
		if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(mt.notebook)) == 1) {
			 gtk_notebook_set_show_tabs(GTK_NOTEBOOK(mt.notebook), FALSE);
			 gtk_widget_grab_focus(gtk_notebook_get_nth_page(GTK_NOTEBOOK(mt.notebook), gtk_notebook_get_current_page(GTK_NOTEBOOK(mt.notebook))));
		 }
		if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(mt.notebook)) == 0) { quit(); }
		}		
static void tab_geometry_hints(term *t) {
	/*barrowed from sakura, but using non depreacated code patch by me :)*/
	/* I dont need to call this every time, since the char width only changes once, maybe i'll make hints and border global and reuse them*/
	GdkGeometry hints;
	GtkBorder *border;
	gint pad_x, pad_y;
	gint char_width, char_height;
	//vte_terminal_get_padding(VTE_TERMINAL(t->vte), (int *)&pad_x, (int *)&pad_y);
	gtk_widget_style_get(GTK_WIDGET(t->vte), "inner-border", &border, NULL);
	
	char_width = vte_terminal_get_char_width(VTE_TERMINAL(t->vte));
	char_height = vte_terminal_get_char_height(VTE_TERMINAL(t->vte));
	
   hints.min_width = char_width + border->left + border->right;
   hints.min_height = char_height + border->top + border->bottom;
   hints.base_width = border->left + border->right;
   hints.base_height = border->top + border->bottom;
   hints.width_inc = char_width;
   hints.height_inc = char_height;
   
		gtk_window_set_geometry_hints(GTK_WINDOW (mt.win), GTK_WIDGET (t->vte), &hints, GDK_HINT_RESIZE_INC | GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE);
}
static void tab_title(GtkWidget *widget, term *t) {
	gtk_label_set_text(GTK_LABEL(t->label), vte_terminal_get_window_title(VTE_TERMINAL(t->vte)));
	
}

static void tab_focus(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data)   {
		struct term *t;
		t = get_page_term(NULL, page_num);
		const char *title = vte_terminal_get_window_title(VTE_TERMINAL(t->vte));
		if (title == NULL) { title = "mt";	}
		gtk_window_set_title(GTK_WINDOW(mt.win), title);
	}
	
static void tab_new() {
	
	term *t;
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
	
	//vte_terminal_set_background_transparent(VTE_TERMINAL(t->vte), TRUE);	
	
	
	*tmp = vte_terminal_match_add_gregex(VTE_TERMINAL(t->vte), g_regex_new("(ftp|http)s?://[-a-zA-Z0-9.?$%&/=_~#.,:;+]*", G_REGEX_CASELESS, G_REGEX_MATCH_NOTEMPTY, NULL), 0);
	vte_terminal_match_set_cursor_type(VTE_TERMINAL(t->vte), *tmp, GDK_CROSSHAIR);
	g_free (tmp);
	/*barrowed from sakura*/
	vte_terminal_set_scrollback_lines(VTE_TERMINAL(t->vte), scroll);
	vte_terminal_set_mouse_autohide(VTE_TERMINAL(t->vte), TRUE);
	vte_terminal_set_font_from_string(VTE_TERMINAL(t->vte), font);
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
}
int main (int argc, char* argv[]) {
  gtk_init (&argc, &argv);
  config();
  gtk_main();  
  return 0;
};
