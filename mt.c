#include <gtk/gtk.h>
#include <vte/vte.h>
#include <gdk/gdkkeysyms.h>


static void quit();
gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event);
static void tab_new();
static void tab_close();

/* part of the code is from sakura. :) i just want a more minimal version, sakura is to code bloat for what i want. so snippets will do*/
/* i am very thankful for this part, because it really helped me figure out how to close tabs properly (sounds easy, but i am dumb as a doorknob*/
static GQuark term_data_id = 0;
#define  get_page_term( sakura, page_idx ) \
    (struct term*)g_object_get_qdata(G_OBJECT( gtk_notebook_get_nth_page( (GtkNotebook*)mt.notebook, page_idx ) ), term_data_id);


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

	if (gdk_keyval_to_lower(event->keyval) == GDK_t) {
	tab_new();
		return TRUE;
	}
	if (gdk_keyval_to_lower(event->keyval) == GDK_x) {
		tab_close();
		return TRUE;
	}
	
	}
		return FALSE;
}
static void tab_close() {
	
	
	gint page = gtk_notebook_get_current_page(GTK_NOTEBOOK(mt.notebook));
		
	struct term *t;
	t = get_page_term(NULL, page);
	
	gtk_widget_hide(t->vte);
	gtk_notebook_remove_page(GTK_NOTEBOOK(mt.notebook), page);
	page = gtk_notebook_get_current_page(GTK_NOTEBOOK(mt.notebook));
	g_free(t);
		
		if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(mt.notebook)) == 0) { quit(); }
		
	gtk_widget_grab_focus(gtk_notebook_get_nth_page(GTK_NOTEBOOK(mt.notebook), page));

}

static void tab_new() {
	
	struct term *t;
	t = g_new0(struct term, 1);
	t->label = gtk_label_new("terminal");
	t->vte = vte_terminal_new();
	vte_terminal_fork_command(VTE_TERMINAL(t->vte), NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE);
	int index = gtk_notebook_append_page(GTK_NOTEBOOK(mt.notebook), t->vte, t->label);
	//sakura_set_page_term(NULL, index, t);
	
	g_object_set_qdata_full(G_OBJECT(gtk_notebook_get_nth_page((GtkNotebook*)mt.notebook, index)), term_data_id, t, NULL);
	g_signal_connect(t->vte, "child-exited", G_CALLBACK(tab_close), NULL);
	
	gtk_widget_show_all(mt.notebook);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(mt.notebook), index);
	gtk_widget_grab_focus(t->vte);
}

int main (int argc, char* argv[]) {
    gtk_init (&argc, &argv);
    term_data_id = g_quark_from_static_string("mt");
    mt.notebook = gtk_notebook_new();
    mt.win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
 	gtk_window_set_default_size(GTK_WINDOW(mt.win), 800, 800);
    tab_new();
    gtk_container_add (GTK_CONTAINER(mt.win), mt.notebook);

    gtk_widget_show_all(mt.win);
    
    g_signal_connect (G_OBJECT (mt.win), "destroy", G_CALLBACK (quit), NULL);    
    g_signal_connect(mt.win, "key-press-event", G_CALLBACK(key_press_cb), NULL); 
    
  gtk_main();  
};
