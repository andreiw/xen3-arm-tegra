/*
 * =====================================================================================
 *
 *       Filename:  gtop.c
 *
 *    Description:  GTop
 *
 *        Version:  1.0
 *        Created:  2008년 04월 16일 14시 48분 06초
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Oh Inho (inho48.oh), inho48.oh@samsung.com
 *        Company:  SAMSUNG Electronics
 *
 * =====================================================================================
 */

#include "common.h"
#include <string.h>

#define MAX_DMAP 		10

#define XTOP_VIEW_STEP	6
#define DOTSPACE		21
#define LEFTPADDING		21
#define DRAWPADDING		10

/**
 * MACRO functions
 *  - da = GtkDrawingArea
 *  - gc = GtkGC
 *  - lo = PangoLayout
 */
#define SOLID(w)	\
	gdk_gc_set_line_attributes(gc, w, \
		GDK_LINE_SOLID, GDK_CAP_NOT_LAST, GDK_JOIN_MITER);

#define DASH(w)	\
	gdk_gc_set_line_attributes(gc, 1, \
		GDK_LINE_ON_OFF_DASH, GDK_CAP_NOT_LAST, GDK_JOIN_MITER);

#define BOX(x, y, w, h, f)	\
	gdk_draw_rectangle(GTK_WIDGET(da)->window, (gc), (f), \
		(x), (y), (w), (h));							

#define LINE(x, y, x2, y2) \
	gdk_draw_line(GTK_WIDGET(da)->window, (gc), \
		(x), (y), (x2), (y2));

#define COLOR(c)	\
	gdk_gc_set_rgb_fg_color(gc, &color[(c)]);

#define DRAWTEXT(x, y, str, len)	\
	pango_layout_set_text((lo), (str), strlen(str)); \
	gdk_draw_layout(GTK_WIDGET(da)->window, (gc), \
				(x), (y), (lo));

typedef struct __dmap_t {
	// Draw area widget
	GtkDrawingArea *draw;

	// pos { x, y, width, height }
	GtkAllocation pos;

	// draw function pointer
	int (*df)(int dmap_index);
} dmap_t;

dmap_t dmap[MAX_DMAP];

GtkWidget *vbox, 
		  *notebook, *btn_quit,
		  *lbl_nb1, *lbl_nb2, *lbl_nb3,
		  *vbox_nb1, *vbox_nb2, *vbox_nb3,
		  *nb1_draw,  *nb1_hbox,
		  *nb2_draw,  *nb2_hbox,
		  *nb3_hbox, *nb3_slide, *nb3_progress;
GtkObject *nb3_adj;

extern int TimerStatus;

void redraw_graph()
{
	gint i;

	for (i=0; i<MAX_DMAP; i++) {
		if (dmap[i].draw == NULL)
			continue;

		if (!GTK_IS_DRAWING_AREA(dmap[i].draw))
			continue;

		if (!GDK_IS_DRAWABLE(GTK_WIDGET(dmap[i].draw)->window))
			continue;

		if (dmap[i].df != NULL)
			dmap[i].df(i);
	}

}

void send_redraw_event()
{
	if (dmap[0].draw != NULL) {
		//debug("send signal!");
		if (G_IS_OBJECT(dmap[0].draw)) {
			//debug("gdk_window_invalidate_rect!");
			gdk_window_invalidate_rect(GTK_WIDGET(dmap[0].draw)->window, NULL, FALSE);
		} else
			debug("dmap[0].draw is not gobject");
	} 
	else {
		debug("dmap[0].draw is null");
	}
}

/**
 * Notebook page - [Domain Management] draw function
 * 
 * @param[in]	dmap_index
 */
int draw_nb1(int dmap_index)
{
	GtkDrawingArea *da;
	GtkAllocation pos;
	GdkGC *gc;
	PangoLayout *lo;
	PangoFontDescription *ft;
	gint x=0, y=0,
		 left_margin=25,
		 left_padding=0,
		 sx=0, ox=0,
		 d0_sy=0, d0_oy=0,
		 d1_sy=0, d1_oy=0,
		 height,
		 i;
	gchar d0_pct[16],
		  d1_pct[16],
		  text[255];

	//debug("draw - Notebook page 1 (dmap_index=%d)", dmap_index);

	da = dmap[dmap_index].draw;
	memcpy(&pos, &dmap[dmap_index].pos, sizeof(GtkAllocation));

	x			 = DRAWPADDING;
	y			 = DRAWPADDING;
	pos.width 	-= DRAWPADDING;
	pos.height	-= DRAWPADDING;
	height = 132;
	
	// used in MACRO 
	gc = (GTK_WIDGET(da)->style->bg_gc[GTK_STATE_NORMAL]);
	memset(text, 0, 255);
	lo = gtk_widget_create_pango_layout(GTK_WIDGET(da), text);
	ft = pango_font_description_new();

	// 
	pango_font_description_set_family(ft, "sans-serif");
	pango_font_description_set_size(ft, 9 * PANGO_SCALE);
	pango_layout_set_font_description(lo, ft);

	COLOR(BLACK);
	//BOX(x, y, pos.width, pos.height, TRUE);
	BOX(x, y, pos.width, pos.height, FALSE);
//	debug("(%d,%d) width=%d, height=%d", x, y, pos.width, pos.height);

	COLOR(GRAY2);
	LINE(x, y+height+1, x+pos.width-2, y+height+1);
	LINE(x+left_margin, y, x+left_margin, y+height);
	LINE(x+left_margin, y+66, x+pos.width-2, y+66);

	COLOR(GRAY3);
	DRAWTEXT(x, y+26, "75%", -1);
	DRAWTEXT(x, y+59, "50%", -1);
	DRAWTEXT(x, y+92, "25%", -1);

	COLOR(GRAY1);
	LINE(x+left_margin, y+33, x+pos.width-2, y+33);
	LINE(x+left_margin, y+99, x+pos.width-2, y+99);

	left_padding = LEFTPADDING;
	ox = x + left_margin + DOTSPACE;
	if (dmap_index == 0) {
		d0_oy = dom0_cpu[0] * height / 100;
		d1_oy = dom1_cpu[0] * height / 100;
	}
	else {
		d0_oy = dom0_mem[0] * height / 100;
		d1_oy = dom1_mem[0] * height / 100;
	}

	for (i=0; i<XTOP_MAX_STEP; i++) {

		sx = x + i * DOTSPACE + left_margin;

		COLOR(GREEN1);
		DASH(1);
		LINE(sx+left_padding, y, sx+left_padding, y+height);
		SOLID(2);

		if (dmap_index == 0) {
			if (dom0_cpu[i] != -1) {
				d0_sy = dom0_cpu[i] * height / 100;
				if (d0_sy >= height) d0_sy = height-1;

				COLOR(GREEN2);
				LINE(ox, y+height-d0_oy, sx+left_padding, y+height-d0_sy);
			} else
				d0_sy = 0;


			if (dom1_cpu[i] != -1) {
				d1_sy = dom1_cpu[i] * height / 100;
				if (d1_sy >= height) d1_sy = height-1;

				COLOR(YELLOW2);
				LINE(ox, y+height-d1_oy, sx+left_padding, y+height-d1_sy);
			} else
				d1_sy = 0;
		} 
		else {
			if (dom0_mem[i] != -1) {
				d0_sy = dom0_mem[i] * height / 100;
				if (d0_sy >= height) d0_sy = height-1;

				COLOR(GREEN2);
				LINE(ox, y+height-d0_oy, sx+left_padding, y+height-d0_sy);
			} else
				d0_sy = 0;


			if (dom1_mem[i] != -1) {
				d1_sy = dom1_mem[i] * height / 100;
				if (d1_sy >= height) d1_sy = height-1;

				COLOR(YELLOW2);
				LINE(ox, y+height-d1_oy, sx+left_padding, y+height-d1_sy);
			} else
				d1_sy = 0;
		}

		ox = sx + left_padding;
		d0_oy = d0_sy;
		d1_oy = d1_sy;
	}

	SOLID(1);
	COLOR(RED);
	BOX(sx+left_padding, y+height-d0_sy-1, 2, 2, TRUE);
	if (dmap_index == 0) {
		if (dom1_cpu[i-1] != -1) {
			BOX(sx+left_padding, y+height-d1_sy-1, 2, 2, TRUE);
		}
		sprintf(d0_pct, "Dom0 (%d%%)", dom0_cpu[i-1]);
		sprintf(d1_pct, "Dom1 (%d%%)", dom1_cpu[i-1]);

		if (dom0_cpu[i-1] == -1)
			sprintf(d0_pct, "Dom0 (0%%)");
	} 
	else {
		if (dom1_mem[i-1] != -1) {
			BOX(sx+left_padding, y+height-d1_sy-1, 2, 2, TRUE);
		}
		sprintf(d0_pct, "Dom0 (%d%%)", dom0_mem[i-1]);
		sprintf(d1_pct, "Dom1 (%d%%)", dom1_mem[i-1]);

		if (dom0_mem[i-1] == -1)
			sprintf(d0_pct, "Dom0 (0%%)");
	}


	COLOR(WHITE);
	DRAWTEXT(x+35, y+height+2, d0_pct, strlen(d0_pct));

	COLOR(GREEN2);
	BOX(x+10, y+height+6, 20, 8, TRUE);
	COLOR(GRAY1);
	BOX(x+10, y+height+6, 20, 8, FALSE);

	if (dmap_index == 0) {
		if (dom1_cpu[i-1] == -1) {
			COLOR(YELLOW1);
			BOX(x+120, y+height+6, 20, 8, TRUE);
			COLOR(GRAY1);
			BOX(x+120, y+height+6, 20, 8, FALSE);

			sprintf(d1_pct, "Dom1 ( 0%%)");
		} else {
			COLOR(YELLOW2);
			BOX(x+120, y+height+6, 20, 8, TRUE);
			COLOR(GRAY1);
			BOX(x+120, y+height+6, 20, 8, FALSE);

			COLOR(WHITE);
		}
	}
	else {
		if (dom1_mem[i-1] == -1) {
			COLOR(YELLOW1);
			BOX(x+120, y+height+6, 20, 8, TRUE);
			COLOR(GRAY1);
			BOX(x+120, y+height+6, 20, 8, FALSE);

			sprintf(d1_pct, "Dom1 ( 0%%)");
		} else {
			COLOR(YELLOW2);
			BOX(x+120, y+height+6, 20, 8, TRUE);
			COLOR(GRAY1);
			BOX(x+120, y+height+6, 20, 8, FALSE);

			COLOR(WHITE);
		}
	}
	DRAWTEXT(x+150, y+height+2, d1_pct, strlen(d1_pct));

	g_object_unref(lo);
	pango_font_description_free(ft);

	return 0;
}




static gboolean tbrbtn(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	debug("button = %d\n", event->button);

	return TRUE;
}

/**
 * Button clicked event handler 
 *  - Gtop quit
 */
void gtop_quit(GtkButton *btn, gpointer user_data)
{
	TimerStatus = FALSE;
	debug("program quit");
	while(1) {
		if (TimerStatus == 2)
			break;
		sleep(1);
		debug("timer wait..");
	}
	gtk_main_quit();
}


/**
 * DrawingArea Expose event handler
 *  - call DrawingArea draw function
 */
gboolean draw_bridge(GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{
	int i;

	for (i=0; i<MAX_DMAP; i++) {
		if (GTK_WIDGET(dmap[i].draw) == widget) {
			if (!GTK_IS_DRAWING_AREA(dmap[i].draw))
				continue;

			if (!GDK_IS_DRAWABLE(GTK_WIDGET(dmap[i].draw)->window))
				continue;

			if (dmap[i].df != NULL) {
				//debug("draw_bridge: dmap[%d] draw!", i);
				dmap[i].df(i);
			}
		}
	}

	return TRUE;
}


/**
 * DrawingArea size-allocate event handler
 *
 */
void sizealo(GtkWidget *widget, GtkAllocation *alo, gpointer user_data)
{
	int i;

	for (i=0; i<MAX_DMAP; i++) {
		if (GTK_WIDGET(dmap[i].draw) == widget) {
			memcpy(&(dmap[i].pos), alo, sizeof(GtkAllocation));
		}
	}
}


static void battery_change(GtkAdjustment *adj, gpointer user_data)
{
	//gchar label_text[200];
	gchar pctstr[5];
	gdouble	pct;
	gint batterylife;

	//sprintf(label_text, "<span foreground=\"white\"><big>Value %d</big></span>",(gint)adj->value);
	batterylife = (int)(adj->value);
	sprintf(pctstr, "%d%%", (int)(adj->value));
	pct = adj->value / 100;

	//gtk_label_set_markup_with_mnemonic(GTK_LABEL(nb3_label), label_text);
	
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(nb3_progress), pct);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(nb3_progress), pctstr);

	if (btm_fd >= 0) {
		debug("ioctl SET_BATTERYLIFE = %d", batterylife);
		ioctl(btm_fd, SET_BATTERYLIFE, &batterylife);
	} else {
		debug("btm_fd is not opened");
	}
}

/**
 * GUI Main
 *
 */
int start_gtop(GtkWidget *window)
{
	gint i;

	// main container
	vbox = gtk_vbox_new(FALSE, 2);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	// Tab widget
	notebook = gtk_notebook_new();

	// QUIT button
	btn_quit = gtk_button_new_with_mnemonic("Quit");

	// make tab page
	vbox_nb1 = gtk_vbox_new(FALSE, 2);
	vbox_nb2 = gtk_vbox_new(FALSE, 2);
	vbox_nb3 = gtk_vbox_new(FALSE, 2);
	gtk_container_add(GTK_CONTAINER(notebook), vbox_nb1);
	gtk_container_add(GTK_CONTAINER(notebook), vbox_nb2);
	gtk_container_add(GTK_CONTAINER(notebook), vbox_nb3);

	// set tab label
	lbl_nb1 = gtk_label_new("CPU");
	lbl_nb2 = gtk_label_new("MEM");
	lbl_nb3 = gtk_label_new("Battery");
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 0), lbl_nb1);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 1), lbl_nb2);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook), gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), 2), lbl_nb3);


	// make 'CPU' tab
	nb1_draw = gtk_drawing_area_new();
	nb1_hbox = gtk_hbox_new(TRUE, 0);
	
	gtk_box_pack_start(GTK_BOX(nb1_hbox), nb1_draw, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox_nb1), nb1_hbox, TRUE, TRUE, 0);


	// make 'MEM' tab
	nb2_hbox = gtk_hbox_new(TRUE, 0);
	nb2_draw = gtk_drawing_area_new();

	gtk_box_pack_start(GTK_BOX(nb2_hbox), nb2_draw, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox_nb2), nb2_hbox, TRUE, TRUE, 0);


	// make 'Battery' tab
	nb3_hbox 	= gtk_hbox_new(TRUE, 0);
	nb3_progress= gtk_progress_bar_new();
	// (value, lower, upper, step_increment, page_increment, page_size)
	nb3_adj  	= gtk_adjustment_new (100.0, 0.0, 100.0, 5.0, 5.0, 0);
	nb3_slide	= gtk_hscale_new(GTK_ADJUSTMENT(nb3_adj));

	// Slider
	gtk_range_set_update_policy (GTK_RANGE (nb3_slide),  GTK_UPDATE_CONTINUOUS);
	gtk_scale_set_draw_value (GTK_SCALE(nb3_slide), FALSE);
	gtk_widget_set_sensitive(nb3_slide, TRUE);

	// Progressbar
	gtk_widget_set_size_request(nb3_progress, 250, 50);
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(nb3_progress), 1.0);
	gtk_progress_bar_set_text(GTK_PROGRESS_BAR(nb3_progress), "100%");

	gtk_box_pack_start(GTK_BOX(nb3_hbox), nb3_progress, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(vbox_nb3), nb3_hbox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox_nb3), nb3_slide, TRUE, TRUE, 0);

  	g_signal_connect (G_OBJECT (nb3_adj), "value_changed",
                      G_CALLBACK (battery_change), NULL);
  

	// 
	gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), btn_quit, FALSE, TRUE, 0);


	// Mapping DrawingArea - dmap table
	dmap[0].draw = GTK_DRAWING_AREA(nb1_draw);
	dmap[1].draw = GTK_DRAWING_AREA(nb2_draw);
	dmap[0].df = draw_nb1;
	dmap[1].df = draw_nb1;

	for (i=0; i<MAX_DMAP; i++) {
		if (dmap[i].draw == NULL) 
			continue;

		g_signal_connect(G_OBJECT(dmap[i].draw), "size-allocate", G_CALLBACK(sizealo), NULL);
		g_signal_connect(G_OBJECT(dmap[i].draw), "expose-event", G_CALLBACK(draw_bridge), NULL);
	}

	g_signal_connect(G_OBJECT(btn_quit), "clicked", G_CALLBACK(gtop_quit), NULL);

	g_signal_connect(G_OBJECT(window), "button_press_event", G_CALLBACK(tbrbtn), NULL);
	g_signal_connect(G_OBJECT(window), "button_release_event", G_CALLBACK(tbrbtn), NULL);

//	gtk_widget_show_all(window);

	return 0;
}

