#include <gtk/gtk.h>
#include <vte/vte.h>
#include <gdk/gdkkeysyms.h>


static void quit();
gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event);
static void tab_new();
static void tab_close();
static void tab_geometry_hints();
static void config();
/* part of the code is from sakura. :) i just want a more minimal version, sakura is to code bloat for what i want. so snippets will do*/
/* i am very thankful for this part, because it really helped me figure out how to close tabs properly (sounds easy, but i am dumb as a doorknob*/
static GQuark term_data_id = 0;
#define  get_page_term( sakura, page_idx ) (struct term*)g_object_get_qdata(G_OBJECT( gtk_notebook_get_nth_page( (GtkNotebook*)mt.notebook, page_idx ) ), term_data_id);

static char *font = "terminus 9";
static long scroll = 500;
//static char *httpregexp =  "(ftp|http)s?://[-a-zA-Z0-9.?$%&/=_~#.,:;+]*";

static struct {
	GtkWidget *win;
	GtkWidget *notebook;
	gchar *title;
} mt;

struct term {
	GtkWidget *vte;     /* Reference to VTE terminal */
	GtkWidget *scrollbar;
	GtkWidget *label;
	gchar *label_text;
};
static void quit() {
	gtk_main_quit();
}

gboolean key_press_cb (GtkWidget *widget, GdkEventKey *event) {	
if (event->state == (GDK_CONTROL_MASK|GDK_SHIFT_MASK)) {

 if (gdk_keyval_to_lower(event->keyval) == GDK_t) {	tab_new(); return TRUE;	}
 if (gdk_keyval_to_lower(event->keyval) == GDK_x) { tab_close();	return TRUE; }	
 if (gdk_keyval_to_lower(event->keyval) == GDK_Left) { gtk_notebook_prev_page(GTK_NOTEBOOK(mt.notebook)); return TRUE;  }
 if (gdk_keyval_to_lower(event->keyval) == GDK_Right) { gtk_notebook_next_page(GTK_NOTEBOOK(mt.notebook)); 	return TRUE; }
	
	}
		return FALSE;
}


static void tab_close() {
	
	
	gint page = gtk_notebook_get_current_page(GTK_NOTEBOOK(mt.notebook));
	
	/*remove t structure and widgets*/	
	struct term *t;
	t = get_page_term(NULL, page);
	gtk_notebook_remove_page(GTK_NOTEBOOK(mt.notebook), page);
	g_free(t);
	
	
	gtk_widget_grab_focus(gtk_notebook_get_nth_page(GTK_NOTEBOOK(mt.notebook), gtk_notebook_get_current_page(GTK_NOTEBOOK(mt.notebook))));


	
		if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(mt.notebook)) == 1) { gtk_notebook_set_show_tabs(GTK_NOTEBOOK(mt.notebook), FALSE); }
		if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(mt.notebook)) == 0) { quit(); }
		}
		
		
static void tab_geometry_hints(struct term *t) {
	
	/*barrowed from sakura, but using non depreacated code*/
	
	GdkGeometry hints;
	GtkBorder *border;
	gint pad_x, pad_y;
	gint char_width, char_height;
	puts("1");
	//vte_terminal_get_padding(VTE_TERMINAL(t->vte), (int *)&pad_x, (int *)&pad_y);
	gtk_widget_style_get(GTK_WIDGET(t->vte), "inner-border", &border, NULL);
	
	char_width = vte_terminal_get_char_width(VTE_TERMINAL(t->vte));
	char_height = vte_terminal_get_char_height(VTE_TERMINAL(t->vte));
   puts("1");
   hints.min_width = char_width + border->left + border->right;
   hints.min_height = char_height + border->top + border->bottom;
   hints.base_width = border->left + border->right;
   hints.base_height = border->top + border->bottom;
   hints.width_inc = char_width;
   hints.height_inc = char_height;
		gtk_window_set_geometry_hints(GTK_WINDOW (mt.win), GTK_WIDGET (t->vte), &hints, GDK_HINT_RESIZE_INC | GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE);
}



static void tab_new() {
	
	struct term *t;
	t = g_new0(struct term, 1);
	t->label = gtk_label_new("terminal");
	t->vte = vte_terminal_new();
	vte_terminal_fork_command(VTE_TERMINAL(t->vte), NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE);
	int index = gtk_notebook_append_page(GTK_NOTEBOOK(mt.notebook), t->vte, t->label);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(mt.notebook), t->vte, TRUE);
	
	if ( gtk_notebook_get_n_pages(GTK_NOTEBOOK(mt.notebook)) == 1) {
		tab_geometry_hints(t);
	}
	if (index == 0) {
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(mt.notebook), FALSE);
	} else  { gtk_notebook_set_show_tabs(GTK_NOTEBOOK(mt.notebook), TRUE); }

	g_object_set_qdata_full(G_OBJECT(gtk_notebook_get_nth_page((GtkNotebook*)mt.notebook, index)), term_data_id, t, NULL);
	g_signal_connect(t->vte, "child-exited", G_CALLBACK(tab_close), NULL);
	
	
	
	/*barrowed from sakura*/
	vte_terminal_set_scrollback_lines(VTE_TERMINAL(t->vte), scroll);
	// maybe i dont /REALLY NEED/ it. vte_terminal_match_add_gregex(VTE_TERMINAL(t->vte), httpregexp, 0);
	vte_terminal_set_mouse_autohide(VTE_TERMINAL(t->vte), TRUE);
	vte_terminal_set_font_from_string(VTE_TERMINAL(t->vte), font);
	//vte_terminal_set_
	gtk_widget_show_all(mt.notebook);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(mt.notebook), index);
	gtk_widget_grab_focus(t->vte);
}
static void config(){
	
	term_data_id = g_quark_from_static_string("mt");
    mt.notebook = gtk_notebook_new();
    gtk_notebook_set_show_border(GTK_NOTEBOOK(mt.notebook), FALSE);
    mt.win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
 	gtk_window_set_default_size(GTK_WINDOW(mt.win), 800, 800);
    gtk_container_add (GTK_CONTAINER(mt.win), mt.notebook);
	tab_new();
    gtk_widget_show_all(mt.win);
    g_signal_connect (G_OBJECT (mt.win), "destroy", G_CALLBACK (quit), NULL);    
    g_signal_connect(mt.win, "key-press-event", G_CALLBACK(key_press_cb), NULL); 
    
	
}

int main (int argc, char* argv[]) {
    gtk_init (&argc, &argv);
    
    config();
    
  gtk_main();  
  
  return 0;
};
