#include <gtk/gtk.h>

#include "filters_list.h"
#include "filter.h"
#include "gui.h"	// Only needed for create_gamelist_content

struct _GMAMEUIFiltersListPrivate
{
	GtkTreeStore          *store;
	GtkTreeModel          *filter;
	
	GtkTreeSelection      *selection;
	
	GList *groups;  /* All the filter categories */

};

/* Used when searching the tree model for a group */
typedef struct {
	GtkTreeIter  iter;
	const gchar *name;
	gboolean     found;
} FindGroup;

static void gmameui_filters_list_class_init    (GMAMEUIFiltersListClass *class);
static void gmameui_filters_list_init (GMAMEUIFiltersList      *fl);

static void
filters_list_get_group (GMAMEUIFiltersList *fl,
			const gchar       *name,
			GtkTreeIter       *iter_group_to_set,
			GtkTreeIter       *iter_separator_to_set,
			gboolean          *created);
static gboolean filters_list_filter_func                     (GtkTreeModel           *model,
							      GtkTreeIter            *iter,
							      GMAMEUIFiltersList      *list);
static void
filters_list_pixbuf_cell_data_func (GtkTreeViewColumn *tree_column,
				    GtkCellRenderer   *cell,
				    GtkTreeModel      *model,
				    GtkTreeIter       *iter,
				    GMAMEUIFiltersList *list);
static void
filters_list_text_cell_data_func (GtkTreeViewColumn *tree_column,
				  GtkCellRenderer   *cell,
				  GtkTreeModel      *model,
				  GtkTreeIter       *iter,
				  GMAMEUIFiltersList *list);

static void
filters_list_expander_cell_data_func (GtkTreeViewColumn *column,
				      GtkCellRenderer   *cell,
				      GtkTreeModel      *model,
				      GtkTreeIter       *iter,
				      GMAMEUIFiltersList *list);

static void     filters_list_row_activated_cb                (GMAMEUIFiltersList      *list,
							      GtkTreePath            *path,
							      GtkTreeViewColumn      *col,
							      gpointer                user_data);

static void
filters_list_selection_changed_cb (GtkTreeSelection *selection,
				    GMAMEUIFiltersList     *fl);

void
gmameui_filters_list_select (GMAMEUIFiltersList *fl,
							 GMAMEUIFilter *filter);

G_DEFINE_TYPE (GMAMEUIFiltersList, gmameui_filters_list,
			   GTK_TYPE_TREE_VIEW);

static void
gmameui_filters_list_finalize (GObject *obj)
{
GMAMEUI_DEBUG ("Freeing filters list");
	GMAMEUIFiltersList *fl = GMAMEUI_FILTERS_LIST (obj);
	
	/* Clear the list of filters TODO - Clear each node first? */
	g_list_free (fl->priv->groups);

	gtk_tree_store_clear (GTK_TREE_STORE (fl->priv->store));
	g_object_unref (fl->priv->store);
	g_object_unref (fl->priv->filter);
	g_object_unref (fl->priv->selection);
	
	g_free (fl->priv);
	
	((GObjectClass *) gmameui_filters_list_parent_class)->finalize (obj);
GMAMEUI_DEBUG ("Freeing filters list - done");
}

static void
gmameui_filters_list_class_init (GMAMEUIFiltersListClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->finalize = (GObjectFinalizeFunc) gmameui_filters_list_finalize;
}

/* Creates the UI for the filters list */
static void
gmameui_filters_list_init (GMAMEUIFiltersList *fl)
{
	GtkTreeModel *model;
	
GMAMEUI_DEBUG ("Creating filters list");

	fl->priv = g_new0 (GMAMEUIFiltersListPrivate, 1);

	/* Create the model */
	fl->priv->store = gtk_tree_store_new (GMAMEUI_FILTER_LIST_MODEL_N_COLUMNS,
					  GDK_TYPE_PIXBUF,     /* Status pixbuf */
					  G_TYPE_STRING,       /* Name */
					  G_TYPE_OBJECT,       /* Filter */
					  G_TYPE_BOOLEAN,      /* Is group */
					  G_TYPE_BOOLEAN,      /* Is separator */
					  -1);     

	/* Save normal model */
	model = GTK_TREE_MODEL (fl->priv->store);
	
	fl->priv->filter = gtk_tree_model_filter_new (model, NULL);

	gtk_tree_view_set_model (GTK_TREE_VIEW (fl), fl->priv->filter);
	
	/* Setup the view */
	GtkCellRenderer   *cell;
	GtkTreeViewColumn *col;

	g_object_set (fl,
		      "headers-visible", FALSE,
		      "reorderable", FALSE, /* Disable re-ordering (also removes ability to drag and drop filters around) */
		      "show-expanders", FALSE,
		      "search-column", -1,
		      NULL);
	
	col = gtk_tree_view_column_new ();

	/* State */
	cell = gtk_cell_renderer_pixbuf_new ();
	gtk_tree_view_column_pack_start (col, cell, FALSE);
	gtk_tree_view_column_set_cell_data_func (
		col, cell,
		(GtkTreeCellDataFunc) filters_list_pixbuf_cell_data_func,
		fl, NULL);

	g_object_set (cell,
		      "xpad", 5,
		      "ypad", 1,
		      "visible", FALSE,
		      NULL);

	/* Name */
	cell = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (col, cell, TRUE);
	gtk_tree_view_column_set_cell_data_func (
		col, cell,
		(GtkTreeCellDataFunc) filters_list_text_cell_data_func,
		fl, NULL);

	gtk_tree_view_column_add_attribute (col, cell,
					    "text", GMAMEUI_FILTER_LIST_MODEL_COLUMN_NAME);


	/* Expander */
	cell = gossip_cell_renderer_expander_new ();
	gtk_tree_view_column_pack_end (col, cell, FALSE);
	gtk_tree_view_column_set_cell_data_func (
		col, cell,
		(GtkTreeCellDataFunc) filters_list_expander_cell_data_func,
		fl, NULL);

	/* Actually add the column now we have added all cell renderers */
	gtk_tree_view_append_column (GTK_TREE_VIEW (fl), col);
	
	/* Now add the categories */
	fl->priv->groups = g_list_append (fl->priv->groups, "Available");
	fl->priv->groups = g_list_append (fl->priv->groups, "Architecture");
	fl->priv->groups = g_list_append (fl->priv->groups, "Imperfect");
	fl->priv->groups = g_list_append (fl->priv->groups, "Game Details");
	fl->priv->groups = g_list_append (fl->priv->groups, "Custom");
	fl->priv->groups = g_list_append (fl->priv->groups, "Category");
	fl->priv->groups = g_list_append (fl->priv->groups, "Version");

	fl->priv->selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (fl));
	
	/* Double-click on row */
	g_signal_connect (fl,
					  "row-activated",
					  G_CALLBACK (filters_list_row_activated_cb),
					  NULL);
	
	/* Single click on row */
	g_signal_connect (fl->priv->selection,
					  "changed",
					  G_CALLBACK (filters_list_selection_changed_cb),
					  NULL);
GMAMEUI_DEBUG ("Finished creating filters list");
}

GtkWidget *
gmameui_filters_list_new (void)
{
	return g_object_new (GMAMEUI_TYPE_FILTERS_LIST,
			     NULL);

}

void
gmameui_filters_list_add_filter (GMAMEUIFiltersList *fl,
						 GMAMEUIFilter *filter, 
						 gchar *group)
{
	GtkTreeIter iter, iter_group, iter_separator;
	GList *l;
	
	for (l = fl->priv->groups; l; l = l->next) {
		const gchar *name;
		gboolean     created;
		GtkTreePath   *path;
		
		gchar *filter_name;
		GdkPixbuf *filter_pixbuf;
	
		name = l->data;
		if (!name)
			continue;
		
		/* Get the iterators for the specific group we want; if the group
		   doesn't exist, it is automatically created as a separator */
		filters_list_get_group (fl, name, &iter_group, &iter_separator, &created);
	
		/* Don't add item to group if we haven't explicitly done so */
		if (g_ascii_strcasecmp (name, group) != 0)
			continue;

		g_object_get (filter,
		      "name", &filter_name,
		      "pixbuf", &filter_pixbuf,
		      NULL);

		/* Expand if there are child nodes (i.e. filters) in the group */
		path = gtk_tree_model_get_path (GTK_TREE_MODEL (fl->priv->store), &iter_group);
		gtk_tree_view_expand_row (GTK_TREE_VIEW (fl), path, TRUE);
		gtk_tree_path_free (path);
		
		//gtk_tree_store_insert_after (fl->priv->store, &iter, &iter_group, NULL);
		gtk_tree_store_append (fl->priv->store, &iter, &iter_group);
		gtk_tree_store_set (fl->priv->store, &iter,
						GMAMEUI_FILTER_LIST_MODEL_COLUMN_PIXBUF, filter_pixbuf,
						GMAMEUI_FILTER_LIST_MODEL_COLUMN_NAME, filter_name,
						GMAMEUI_FILTER_LIST_MODEL_COLUMN_FILTER, filter,
						GMAMEUI_FILTER_LIST_MODEL_IS_GROUP, FALSE,
						GMAMEUI_FILTER_LIST_MODEL_IS_SEPARATOR, FALSE,
						-1);

		g_free (filter_name);
		if (filter_pixbuf != NULL)
			g_object_unref (filter_pixbuf);

		if (!created)
			continue;

	}
}

static gboolean
filters_list_get_group_foreach (GtkTreeModel *model,
				GtkTreePath  *path,
				GtkTreeIter  *iter,
				FindGroup    *fg)
{
	gchar    *str;
	gboolean  is_group;

	/* Groups are only at the top level. */
	if (gtk_tree_path_get_depth (path) != 1) {
		return FALSE;
	}

	gtk_tree_model_get (model, iter,
						GMAMEUI_FILTER_LIST_MODEL_COLUMN_NAME, &str,
						GMAMEUI_FILTER_LIST_MODEL_IS_GROUP, &is_group,
						-1);

	if (is_group && g_ascii_strcasecmp (str, fg->name) == 0) {
		fg->found = TRUE;
		fg->iter = *iter;
	}

	g_free (str);

	return fg->found;
}

static gboolean
filters_list_filter_func (GtkTreeModel      *model,
			  GtkTreeIter       *iter,
			  GMAMEUIFiltersList *list)
{
	gboolean               is_group;
	gboolean               is_separator;
	gchar                 *name;

	gtk_tree_model_get (model, iter,
			    GMAMEUI_FILTER_LIST_MODEL_IS_GROUP, &is_group,
			    GMAMEUI_FILTER_LIST_MODEL_IS_SEPARATOR, &is_separator,
			    GMAMEUI_FILTER_LIST_MODEL_COLUMN_NAME, &name,
			    -1);

	if (is_separator) {
		GMAMEUI_DEBUG ("<--- Filter func: ** SEPARATOR **");
		GMAMEUI_DEBUG ("---> Filter func: returning TRUE\n");
		g_free (name);
		return TRUE;
	}
/* FIXME TODO 
	if (is_group) { 
		GMAMEUI_DEBUG ("<--- Filter func: ** GROUP = '%s' **", name);
		g_free (name);
		return contact_list_filter_func_show_group (model, iter, list);
	} else {
		GMAMEUI_DEBUG ("<--- Filter func: ** FILTER = '%s' **", name);
		g_free (name);
		return contact_list_filter_func_show_contact (model, iter, list);
	}*/

	return FALSE;
}

/* Sets the background colours of the row */
static void
filters_list_cell_set_background (GMAMEUIFiltersList  *list,
				  GtkCellRenderer    *cell,
				  gboolean            is_group,
				  gboolean            is_active)
{
	GdkColor  color;
	GtkStyle *style;

	g_return_if_fail (list != NULL);
	g_return_if_fail (cell != NULL);

	style = gtk_widget_get_style (GTK_WIDGET (list));

	if (!is_group) {
		if (is_active) {
			color = style->bg[GTK_STATE_SELECTED];

			/* Here we take the current theme colour and add it to
			 * the colour for white and average the two. This
			 * gives a colour which is inline with the theme but
			 * slightly whiter.
			 */
			color.red = (color.red + (style->white).red) / 2;
			color.green = (color.green + (style->white).green) / 2;
			color.blue = (color.blue + (style->white).blue) / 2;

			g_object_set (cell,
						  "cell-background-gdk", &color,
						  NULL);
		} else {
			g_object_set (cell,
						  "cell-background-gdk", NULL,
						  NULL);
		}
	} else {
		color = style->text_aa[GTK_STATE_INSENSITIVE];

		color.red = (color.red + (style->white).red) / 2;
		color.green = (color.green + (style->white).green) / 2;
		color.blue = (color.blue + (style->white).blue) / 2;

		g_object_set (cell,
					  "cell-background-gdk", &color,
					  NULL);
	}
}

static void
filters_list_pixbuf_cell_data_func (GtkTreeViewColumn *tree_column,
				    GtkCellRenderer   *cell,
				    GtkTreeModel      *model,
				    GtkTreeIter       *iter,
				    GMAMEUIFiltersList *list)
{
	GdkPixbuf *pixbuf;
	gboolean   is_group;
	gboolean   is_active;

	gtk_tree_model_get (model, iter,
						GMAMEUI_FILTER_LIST_MODEL_IS_GROUP, &is_group,
						GMAMEUI_FILTER_LIST_MODEL_COLUMN_PIXBUF, &pixbuf,
						-1);

	g_object_set (cell,
		      "visible", !is_group,
		      "pixbuf", pixbuf,
		      NULL);

	if (pixbuf) {
		g_object_unref (pixbuf);
	}
	
	is_active = FALSE;

	filters_list_cell_set_background (list, cell, is_group, is_active);
}

static void
filters_list_text_cell_data_func (GtkTreeViewColumn *tree_column,
				  GtkCellRenderer   *cell,
				  GtkTreeModel      *model,
				  GtkTreeIter       *iter,
				  GMAMEUIFiltersList *list)
{
	gboolean is_group;
	gboolean is_active;

	gtk_tree_model_get (model, iter,
						GMAMEUI_FILTER_LIST_MODEL_IS_GROUP, &is_group,
						-1);

	is_active = FALSE;
	filters_list_cell_set_background (list, cell, is_group, is_active);
}

static void
filters_list_expander_cell_data_func (GtkTreeViewColumn *column,
				      GtkCellRenderer   *cell,
				      GtkTreeModel      *model,
				      GtkTreeIter       *iter,
				      GMAMEUIFiltersList *list)
{
	gboolean is_group;
	gboolean is_active;

	gtk_tree_model_get (model, iter,
			    GMAMEUI_FILTER_LIST_MODEL_IS_GROUP, &is_group,
			    -1);

	if (gtk_tree_model_iter_has_child (model, iter)) {
		GtkTreePath *path;
		gboolean     row_expanded;

		path = gtk_tree_model_get_path (model, iter);
		row_expanded = gtk_tree_view_row_expanded (GTK_TREE_VIEW (column->tree_view), path);
		gtk_tree_path_free (path);

		g_object_set (cell,
					  "visible", TRUE,
					  "expander-style", row_expanded ? GTK_EXPANDER_EXPANDED : GTK_EXPANDER_COLLAPSED,
					  NULL);
	} else {
		g_object_set (cell, "visible", FALSE, NULL);
	}

	is_active = FALSE;
	filters_list_cell_set_background (list, cell, is_group, is_active);
}

/* Sets the iters to the row in the filters list containing the
   specified group name. If no group is found by that name, the
   group is created */
static void
filters_list_get_group (GMAMEUIFiltersList *fl,
			const gchar       *name,
			GtkTreeIter       *iter_group_to_set,
			GtkTreeIter       *iter_separator_to_set,
			gboolean          *created)
{
	GtkTreeModel          *model;
	GtkTreeIter            iter_group, iter_separator;
	FindGroup              fg;
	
	memset (&fg, 0, sizeof (fg));
	fg.name = name;
	
	model = GTK_TREE_MODEL (fl->priv->store);
	gtk_tree_model_foreach (model,
				(GtkTreeModelForeachFunc) filters_list_get_group_foreach,
				&fg);

	if (!fg.found) {
		/*GMAMEUI_DEBUG (" Adding group %s to model", name);*/

		if (created) {
			*created = TRUE;
		}

		/* Add the name string only, since this will be a group,
		   not a filter; all other fields are NULL */
		gtk_tree_store_append (fl->priv->store, &iter_group, NULL);
		gtk_tree_store_set (fl->priv->store, &iter_group,
				    GMAMEUI_FILTER_LIST_MODEL_COLUMN_PIXBUF, NULL,
				    GMAMEUI_FILTER_LIST_MODEL_COLUMN_NAME, name,
					GMAMEUI_FILTER_LIST_MODEL_COLUMN_FILTER, NULL,
				    GMAMEUI_FILTER_LIST_MODEL_IS_GROUP, TRUE,
				    GMAMEUI_FILTER_LIST_MODEL_IS_SEPARATOR, FALSE,
				    -1);
	
		if (iter_group_to_set) {
			*iter_group_to_set = iter_group;
		}

		/* The following lines add a blank line at the end of each group.
		   Commented out for now
		gtk_tree_store_append (fl->priv->store,
				       &iter_separator, 
				       &iter_group);
		gtk_tree_store_set (fl->priv->store, &iter_separator,
				    GMAMEUI_FILTER_LIST_MODEL_IS_SEPARATOR, TRUE,
				    -1);*/

		if (iter_separator_to_set) {
			*iter_separator_to_set = iter_separator;
		}
	} else {
		/*GMAMEUI_DEBUG (" Using existing group:'%s' from model", name);*/

		if (created) {
			*created = FALSE;
		}

		if (iter_group_to_set) {
			*iter_group_to_set = fg.iter;
		}

		iter_separator = fg.iter;

		if (gtk_tree_model_iter_next (model, &iter_separator)) {
			gboolean is_separator;

			gtk_tree_model_get (model, &iter_separator,
					    GMAMEUI_FILTER_LIST_MODEL_IS_SEPARATOR, &is_separator,
					    -1);

			if (is_separator && iter_separator_to_set) {
				*iter_separator_to_set = iter_separator;
			}
		}
	}
}

typedef struct _FilterPath {
	GMAMEUIFilter    *filter;
	GtkTreePath      *path;
} FilterPath;

/* Returns TRUE if the selected filter is at the current row in the filters list */
static gboolean
match_source_to_iter (GtkTreeModel *model,
		      GtkTreePath  *path,
		      GtkTreeIter  *iter,
		      FilterPath   *fp)
{
	GMAMEUIFilter *target = NULL;
	gboolean  res;

	gtk_tree_model_get (model, iter, GMAMEUI_FILTER_LIST_MODEL_COLUMN_FILTER, &target, -1);

	res = FALSE;
	if (target == fp->filter) {
		fp->path = gtk_tree_path_copy (path);
		res = TRUE;
	}

	if (target != NULL) {
		g_object_unref (target);
	}

	return res;
}

/* Sets iter to the row of the filters list containing the specified filter */
static gboolean
gmameui_filters_list_source_to_iter (GMAMEUIFiltersList *fl,
									 GMAMEUIFilter *filter,
									 GtkTreeIter  *iter)
{
	FilterPath *fp;
	gboolean    ret;

	ret = FALSE;
	fp = g_new0 (FilterPath, 1);
	fp->filter = filter;

	gtk_tree_model_foreach (fl->priv->filter, (GtkTreeModelForeachFunc) match_source_to_iter, fp);

	if (fp->path) {
		ret = gtk_tree_model_get_iter (fl->priv->filter, iter, fp->path);
	}

	gtk_tree_path_free (fp->path);
	g_free (fp);
	fp = NULL;

	return ret;
}

/* This function takes care of selecting the specified filter in
   the filters list, navigating to the row and highlighting it */
void
gmameui_filters_list_select (GMAMEUIFiltersList *fl,
							 GMAMEUIFilter *filter)
{
	GtkTreeIter iter;

	g_assert (gmameui_filters_list_source_to_iter (fl, filter, &iter));
	gtk_tree_selection_select_iter (fl->priv->selection, &iter);
}

/* FIXME This callback doesn't do anything useful */
static void
filters_list_row_activated_cb (GMAMEUIFiltersList *list,
			       GtkTreePath       *path,
			       GtkTreeViewColumn *col,
			       gpointer           user_data)
{
	GtkTreeView   *view;
	GtkTreeModel  *model;
	GtkTreeIter    iter;
	gchar *name;

	view = GTK_TREE_VIEW (list);
	model = gtk_tree_view_get_model (view);

	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_model_get (model, &iter,
						GMAMEUI_FILTER_LIST_MODEL_COLUMN_NAME, &name, -1);

	
}

static void
filters_list_selection_changed_cb (GtkTreeSelection *selection,
				    GMAMEUIFiltersList     *fl)
{
	GtkTreeIter   iter;
	GtkTreeModel *model;
	GMAMEUIFilter *filter;
	gchar *name;
	gint folderid;

	if (!gtk_tree_selection_get_selected (selection,
										  &model, &iter))
		return;

	gtk_tree_model_get (model, &iter,
						GMAMEUI_FILTER_LIST_MODEL_COLUMN_FILTER, &filter,
						-1);

	g_return_if_fail (filter != NULL);  /* This will fail if the selected row is a group */
	g_return_if_fail (GMAMEUI_IS_FILTER (filter));
	
	g_object_get (filter,
				  "name", &name,
				  "folderid", &folderid,
				  NULL);

	gui_prefs.FolderID = folderid;
	selected_filter = filter;   /* Set global variable for use when re-creating the gamelist content */

	GMAMEUI_DEBUG ("About to recreate gamelist after filter selected");
	create_gamelist_content ();
	GMAMEUI_DEBUG ("Done recreating gamelist after filter selected");
	
	g_free (name);
	g_object_unref (filter);
}
