#include <gtk/gtk.h>
#include <vte/vte.h>
#include <gdk/gdkkeysyms.h>


static void quit();
gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event);
static void new_tab();
static void close_tab();

/* part of the code is from sakura. :) i just want a more minimal version, sakura is to code bloat for what i want. so snippets will do*/
static GQuark term_data_id = 0;
#define  get_page_term(page_idx) (struct term*)g_object_get_qdata(G_OBJECT( gtk_notebook_get_nth_page( (GtkNotebook*)mt.notebook, page_idx ) ), term_data_id);


#define  set_page_term( sakura, page_idx, term )  \
    g_object_set_qdata_full( \
            G_OBJECT( gtk_notebook_get_nth_page( (GtkNotebook*)mt.notebook, page_idx) ), \
            term_data_id, term, (GDestroyNotify)g_free);

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
if (event->state == GDK_CONTROL_MASK) {

	/*if (gdk_keyval_to_lower(event->keyval) == GDK_f) {
		gtk_widget_grab_focus(GTK_WIDGET(sb.bar));	
		return TRUE;
	}
	if (gdk_keyval_to_lower(event->keyval) == GDK_l) {
		gtk_widget_grab_focus(GTK_WIDGET(sb.bar));
		return TRUE;
	}	
	if (gdk_keyval_to_lower(event->keyval) == GDK_j) {
		go_back_cb(NULL, NULL);

		return TRUE;
	}	
	if (gdk_keyval_to_lower(event->keyval) == GDK_k) {
		go_forward_cb(NULL, NULL);
		return TRUE;
	}	
	if (gdk_keyval_to_lower(event->keyval) == GDK_bracketright) {
		
		webkit_web_view_set_zoom_level(sb.view, (webkit_web_view_get_zoom_level(sb.view) + .05));
		return TRUE;
	}
		if (gdk_keyval_to_lower(event->keyval) == GDK_bracketleft) {
		
		webkit_web_view_set_zoom_level(sb.view, (webkit_web_view_get_zoom_level(sb.view) - .05));
		return TRUE;
	}
	else {
		return FALSE;
	}
}

	if (gtk_widget_has_focus(sb.bar) && gdk_keyval_to_lower(event->keyval) == GDK_Escape) {
		gtk_widget_grab_focus(GTK_WIDGET(sb.view));
		return TRUE;
	}*/}
	return FALSE;
}
static void close_tab() {
	
	
	gint page = gtk_notebook_get_current_page(GTK_NOTEBOOK(mt.notebook));
		
	struct term *t = get_page_term(page);
	
	gtk_widget_hide(t->vte);
	gtk_notebook_remove_page(GTK_NOTEBOOK(mt.notebook), page);
	g_free(t);
	if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(mt.notebook)) == 0) {
		quit();
}

}

static void new_tab() {
	
	struct term *t;
	t = g_new0(struct term, 1);
	t->label = gtk_label_new("terminal");
	t->vte = vte_terminal_new();
	vte_terminal_fork_command(VTE_TERMINAL(t->vte), NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE);
	gint index = gtk_notebook_append_page(GTK_NOTEBOOK(mt.notebook), t->vte, t->label);
	
	g_signal_connect(t->vte, "child-exited", G_CALLBACK(close_tab), NULL);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(mt.notebook), index);
	gtk_widget_has_focus(t->vte);
}

int main (int argc, char* argv[]) {
    gtk_init (&argc, &argv);
    
    mt.notebook = gtk_notebook_new();
    mt.win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (G_OBJECT (mt.win), "destroy", G_CALLBACK (quit), NULL);    
    g_signal_connect(mt.win, "key-press-event", G_CALLBACK(key_press_cb), NULL); 
    new_tab();
    new_tab();
    gtk_container_add (GTK_CONTAINER(mt.win), mt.notebook);
    gtk_window_set_default_size(GTK_WINDOW(mt.win), 800, 800);
    gtk_widget_show_all(mt.win);	
    
  gtk_main();  
};
