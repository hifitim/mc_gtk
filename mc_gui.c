

/*  includes  */
#include "mc_gui.h"

/*  function definitions   */
static void
tree_right_click_cut(GtkWidget *menuitem, gpointer user_data)
{
    if(current_state.num_files_to_move != -1)
    {
        char **a;
        unsigned int idx = 0;
        for(a = current_state.files_to_move; idx < current_state.num_files_to_move; a++, idx++)
            free(*a);
        free(current_state.files_to_move);
    }

    unsigned int num_files_to_copy = 0;
    GtkTreeSelection *selection;
    GtkTreeIter iter;
    GtkTreeModel *model;
    struct app_side *side = user_data;
    GList *selected_list = NULL;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(side->tree_view));
    model = gtk_tree_view_get_model(side->tree_view);
    selected_list = gtk_tree_selection_get_selected_rows(selection, &model);

    puts("Cut.");

    GList *l_temp;
    current_state.files_to_move = malloc(sizeof(char*));
    for (l_temp = selected_list; l_temp != NULL; l_temp = l_temp->next)
    {
        current_state.files_to_move = realloc(current_state.files_to_move, (++num_files_to_copy)*sizeof(char*));
        GValue name = {0,};
        gtk_tree_model_get_iter(model, &iter, l_temp->data);
        gtk_tree_model_get_value(model, &iter, FILE_NAME_COLUMN, &name);
        current_state.files_to_move[num_files_to_copy-1] = malloc(sizeof(char*)*strlen(g_value_get_string(&name)));
        snprintf(current_state.files_to_move[num_files_to_copy-1], DEFAULT_STRING_LEN, "%s", g_value_get_string(&name));
        puts(current_state.files_to_move[num_files_to_copy-1]);
    }
    current_state.num_files_to_move = num_files_to_copy;
    current_state.paste_active = 1;
    current_state.move_mode = CUT_MODE;
}

static void
tree_right_click_copy(GtkWidget *menuitem, gpointer user_data)
{
    if(current_state.num_files_to_move != -1)
    {
        char **a;
        unsigned int idx = 0;
        for(a = current_state.files_to_move; idx < current_state.num_files_to_move; a++, idx++)
            free(*a);
        free(current_state.files_to_move);
    }

    unsigned int num_files_to_copy = 0;

    GtkTreeSelection *selection;
    GtkTreeIter iter;
    GtkTreeModel *model;
    struct app_side *side = user_data;
    GList *selected_list = NULL;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(side->tree_view));
    model = gtk_tree_view_get_model(side->tree_view);
    selected_list = gtk_tree_selection_get_selected_rows(selection, &model);

    GList *l_temp;
    current_state.files_to_move = malloc(sizeof(char*));
    /*char **files_to_copy = current_state.files_to_move;*/

    for (l_temp = selected_list; l_temp != NULL; l_temp = l_temp->next)
    {
        current_state.files_to_move = realloc(current_state.files_to_move, (++num_files_to_copy)*sizeof(char*));
        GValue name = {0,};
        gtk_tree_model_get_iter(model, &iter, l_temp->data);
        gtk_tree_model_get_value(model, &iter, FILE_NAME_COLUMN, &name);
        current_state.files_to_move[num_files_to_copy-1] = malloc(sizeof(char*)*strlen(g_value_get_string(&name)));
        snprintf(current_state.files_to_move[num_files_to_copy-1], DEFAULT_STRING_LEN, "%s", g_value_get_string(&name));
        puts(current_state.files_to_move[num_files_to_copy-1]);
    }
    current_state.paste_active = 1;
    current_state.move_mode = COPY_MODE;
    current_state.num_files_to_move = num_files_to_copy;
}

static void
tree_right_click_paste(GtkWidget *menuitem, gpointer user_data)
{
    GtkTreeSelection *selection;
    GtkTreeIter iter;
    GtkTreeModel *model;
    struct app_side *side = user_data;
    GList *selected_list = NULL;
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(side->tree_view));
    model = gtk_tree_view_get_model(side->tree_view);
    selected_list = gtk_tree_selection_get_selected_rows(selection, &model);

    puts("Paste.");

    GList *l_temp;
    for (l_temp = selected_list; l_temp != NULL; l_temp = l_temp->next)
    {
        GValue name = {0,};
        // do something with l->data
        gtk_tree_model_get_iter(model, &iter, l_temp->data);
        gtk_tree_model_get_value(model, &iter, FILE_NAME_COLUMN, &name);
        puts((char*) g_value_get_string(&name));
    }
}

static gboolean
location_entry_lost_focus(GtkWidget *widget,
                          GdkEvent *event,
                          gpointer user_data)
{
    return ((gboolean)FALSE);
}

static gboolean
location_entry_keypress(GtkWidget *widget,
                        GdkEventKey *event,
                        gpointer user_data)
{
    if(event->keyval == GDK_KEY_Return)
    {
        struct app_side *side = (struct app_side*)user_data;
        char entry_text[DEFAULT_STRING_LEN];
        snprintf(entry_text, DEFAULT_STRING_LEN, "%s", gtk_entry_get_text(side->entry));
        if(is_directory(entry_text))
        {
            snprintf(side->current_location, DEFAULT_STRING_LEN, "%s", entry_text);
            populate_list_store_from_folder(side, entry_text);
        }
        else
        {
            gtk_entry_set_text(side->entry, side->current_location);
            populate_list_store_from_folder(side, side->current_location);
            puts("Text entry error: not a directory!");
            /*  Move the cursor to the end of the entry to make it easy to retype, since you made a mistake   */
            gtk_editable_set_position(GTK_EDITABLE(side->entry), -1);
        }
    }
    return ((gboolean)FALSE);
}

static void
clear_list_store(GtkListStore *list_store)
{
    GtkTreeIter iter;

    if (gtk_tree_model_iter_nth_child(GTK_TREE_MODEL(list_store), &iter, NULL, 0))
    {
        gboolean iter_valid;
        do
        {
            iter_valid = gtk_list_store_remove(list_store, &iter);
        } while(iter_valid);
    }
}

static gboolean
tree_view_click(GtkWidget *treeview,
                GdkEventButton *event,
                gpointer user_data)
{
    struct app_side *side = (struct app_side*) user_data;
    if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3)
    {
        puts("Right click.");
        GtkWidget *menu, *menuitem;

        menu = gtk_menu_new();

        menuitem = gtk_menu_item_new_with_label("Cut");
        g_signal_connect(menuitem, "activate", (GCallback) tree_right_click_cut, side);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

        menuitem = gtk_menu_item_new_with_label("Copy");
        g_signal_connect(menuitem, "activate", (GCallback) tree_right_click_copy, side);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

        menuitem = gtk_menu_item_new_with_label("Paste");
        if(!current_state.paste_active)
            gtk_widget_set_sensitive(menuitem, FALSE);
        g_signal_connect(menuitem, "activate", (GCallback) tree_right_click_paste, side);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);

        gtk_widget_show_all(menu);
        /* Note: event can be NULL here when called from view_onPopupMenu;
         *  gdk_event_get_time() accepts a NULL argument */
        gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
                       (event != NULL) ? event->button : 0,
                       gdk_event_get_time((GdkEvent*)event));
    }
    else if(event->type == GDK_BUTTON_PRESS  &&  event->button == 1)
    {
        puts("Left click.");
    }
    return ((gboolean)FALSE);
}

static void
open_file(GtkTreeView *treeview,
          GtkTreePath *path,
          GtkTreeViewColumn *col,
          gpointer user_data)
{
    puts("Row activated.");
    char file_path[DEFAULT_STRING_LEN];
    GtkTreeModel *model;
    GtkTreeIter iter;
    struct app_side *side = (struct app_side*)user_data;
    GtkEntry *entry = GTK_ENTRY(side->entry);
    char current_location[DEFAULT_STRING_LEN];

    snprintf(current_location, DEFAULT_STRING_LEN, "%s", side->current_location);
    model = gtk_tree_view_get_model(treeview);
    if (gtk_tree_model_get_iter(model, &iter, path))
    {
        gchar *name;
        gtk_tree_model_get(model, &iter, FILE_NAME_COLUMN, &name, -1);
        if(strcmp(name, "..") != 0)
        {
            /*  no need to add an extra '/' if that's all that's in the current_location   */
            if(strcmp(current_location, "/"))
                snprintf(file_path, DEFAULT_STRING_LEN, "%s/%s", current_location, name);
            else
                snprintf(file_path, DEFAULT_STRING_LEN, "/%s", name);

            if(is_directory(file_path))
            {
                gtk_entry_set_text(GTK_ENTRY(entry), file_path);
                snprintf(side->current_location, DEFAULT_STRING_LEN, "%s", file_path);
                populate_list_store_from_folder(side, file_path);
            }
            g_free(name);
        }
        else
        {

            strcpy(file_path, gtk_entry_get_text(entry));
            size_t path_len = strlen(file_path);
            size_t final_len = 0;
            for(size_t k = path_len - 1; k >= 0; k--)
            {
                if(file_path[k] == '/')
                {
                    final_len = k;
                    break;
                }
            }
            char new_path[DEFAULT_STRING_LEN];
            memset(new_path, '\0', DEFAULT_STRING_LEN);

            /* If we get to root, don't go further  */
            final_len = (final_len == 0) ? 1 : final_len;
            strncpy(new_path, file_path, final_len);
            gtk_entry_set_text(GTK_ENTRY(entry), new_path);
            populate_list_store_from_folder(side, new_path);
            snprintf(side->current_location, DEFAULT_STRING_LEN, "%s", new_path);
        }
    }
}

static void
color_cell_function(GtkTreeViewColumn *col,
                    GtkCellRenderer *renderer,
                    GtkTreeModel *model,
                    GtkTreeIter *iter,
                    gpointer user_data)
{
    GValue color_value = {0,}, set_value = {0,}, class_value = {0,};

    gtk_tree_model_get_value(model, iter, ROW_COLOR_COLUMN_SET, &set_value);
    if(g_value_get_boolean(&set_value))
    {
        gtk_tree_model_get_value(model, iter, ROW_COLOR_COLUMN, &color_value);
        gtk_tree_model_get_value(model, iter, FILE_TYPE_COLUMN, &class_value);
        char *color_text = (char*) g_value_get_string(&color_value);
        g_object_set(renderer, "foreground", color_text, "foreground-set", TRUE, NULL);
    }
    else
    {
        g_object_set(renderer, "foreground-set", FALSE, NULL); /* print this normal */
    }
}

static int
populate_list_store_from_folder(struct app_side *side,
                                char *path)
{
    clear_list_store(side->list_store);
    GtkTreeIter iter;
    struct dirent *ep;
    GtkListStore *list_store = side->list_store;
    /* setup the .. directory to go back up one level
       wanted this to be first in the directory listing.
    */
    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set(list_store, &iter,
                       FILE_NAME_COLUMN, "..",
                       FILE_SIZE_COLUMN, "dir",
                       ROW_COLOR_COLUMN, "blue",
                       ROW_COLOR_COLUMN_SET, FALSE,
                       FILE_TYPE_COLUMN, "directory",
                       -1);
    DIR *dp = opendir(path);
    if (dp != NULL)
    {
        while ((ep = readdir(dp)))
        {
            if(ep->d_name[0] != '.' &&
               ep->d_name[strlen(ep->d_name)-1] != '~')
            {
                gtk_list_store_append (list_store, &iter);

                char full_path[DEFAULT_STRING_LEN];
                char class[DEFAULT_STRING_LEN];
                char file_size[DEFAULT_STRING_LEN];
                char text_color[DEFAULT_STRING_LEN];
                gboolean color_set = FALSE;

                snprintf(full_path, DEFAULT_STRING_LEN, "%s/%s", path, ep->d_name);

                struct stat buffer;
                if(lstat(full_path, &buffer) != 0)
                {
                    perror("Can't get file size!");
                    return -1;
                }

                sprintf(file_size, "%llu", ((unsigned long long)buffer.st_size));
                if(is_directory(full_path))
                {
                    snprintf(file_size, DEFAULT_STRING_LEN, "dir");
                    snprintf(class, DEFAULT_STRING_LEN, "directory");
                    snprintf(text_color, DEFAULT_STRING_LEN, "blue");
                    color_set = TRUE;
                }
                else if(is_executable(full_path))
                {
                    snprintf(class, DEFAULT_STRING_LEN, "executable");
                    snprintf(text_color, DEFAULT_STRING_LEN, "green");
                    color_set = TRUE;
                }
                else
                {
                    snprintf(class, DEFAULT_STRING_LEN, "file");
                    snprintf(text_color, DEFAULT_STRING_LEN, "black");
                    color_set = FALSE;
                }

                gtk_list_store_set(list_store, &iter,
                                   FILE_NAME_COLUMN, ep->d_name,
                                   FILE_SIZE_COLUMN, file_size,
                                   ROW_COLOR_COLUMN, text_color,
                                   ROW_COLOR_COLUMN_SET, color_set,
                                   FILE_TYPE_COLUMN, class,
                                   -1);
            }
        }
        closedir (dp);
    }
    else
    {
        perror ("Couldn't open the directory");
        return -1;
    }
    return 0;
}

static void
activate (GtkApplication *app,
          gpointer user_data)
{
    char left_location_text[DEFAULT_STRING_LEN];
    char right_location_text[DEFAULT_STRING_LEN];

    /*  Need this off the heap so that it persists after the function is over  */
    char *left_current_location = malloc(DEFAULT_STRING_LEN);
    char *right_current_location = malloc(DEFAULT_STRING_LEN);

    snprintf(left_location_text, DEFAULT_STRING_LEN, "%s", getenv("HOME"));
    snprintf(right_location_text, DEFAULT_STRING_LEN, "%s", getenv("HOME"));
    snprintf(left_current_location, DEFAULT_STRING_LEN, "%s", left_location_text);
    snprintf(right_current_location, DEFAULT_STRING_LEN, "%s", right_location_text);

    GtkWidget *window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    gtk_window_set_default_size (GTK_WINDOW (window), 750, 750);

    GtkPaned *panes = GTK_PANED(gtk_paned_new(GTK_ORIENTATION_HORIZONTAL));
    GtkGrid *grid = GTK_GRID(gtk_grid_new());
    GtkEntry *left_location_entry = GTK_ENTRY(gtk_entry_new());
    GtkEntry *right_location_entry = GTK_ENTRY(gtk_entry_new());
    GtkScrolledWindow *left_scroll_box = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    GtkScrolledWindow *right_scroll_box = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    GtkListStore *left_list_store = GTK_LIST_STORE(gtk_list_store_new(N_COLUMNS,
                                                                      G_TYPE_STRING,
                                                                      G_TYPE_STRING,
                                                                      G_TYPE_STRING,
                                                                      G_TYPE_BOOLEAN,
                                                                      G_TYPE_STRING));
    GtkListStore *right_list_store = GTK_LIST_STORE(gtk_list_store_new(N_COLUMNS,
                                                                       G_TYPE_STRING,
                                                                       G_TYPE_STRING,
                                                                       G_TYPE_STRING,
                                                                       G_TYPE_BOOLEAN,
                                                                       G_TYPE_STRING));
    GtkTreeView *right_tree = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(right_list_store)));
    GtkTreeView *left_tree = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL (left_list_store)));

    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("File Name",
                                                      renderer,
                                                      "text", FILE_NAME_COLUMN,
                                                      NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(right_tree), column);
    gtk_tree_view_column_set_cell_data_func(column, renderer, color_cell_function, NULL, NULL);
    column = gtk_tree_view_column_new_with_attributes("File Size",
                                                      renderer,
                                                      "text", FILE_SIZE_COLUMN,
                                                      NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(right_tree), column);

    column = gtk_tree_view_column_new_with_attributes("File Name",
                                                      renderer,
                                                      "text", FILE_NAME_COLUMN,
                                                      NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(left_tree), column);
    gtk_tree_view_column_set_cell_data_func(column, renderer, color_cell_function, NULL, NULL);
    column = gtk_tree_view_column_new_with_attributes("File Size",
                                                      renderer,
                                                      "text", FILE_SIZE_COLUMN,
                                                      NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(left_tree), column);

    current_state.paste_active = 0;
    current_state.files_to_move = 0;
    current_state.num_files_to_move = -1;

    struct app_side *right_side = malloc(sizeof(struct app_side));
    right_side->entry = right_location_entry;
    right_side->list_store = right_list_store;
    right_side->tree_view = right_tree;
    right_side->current_location = right_current_location;

    struct app_side *left_side = malloc(sizeof(struct app_side));
    left_side->entry = left_location_entry;
    left_side->list_store = left_list_store;
    left_side->tree_view = left_tree;
    left_side->current_location = left_current_location;

    g_signal_connect(right_tree, "row-activated", (GCallback) open_file, right_side);
    g_signal_connect(left_tree, "row-activated", (GCallback) open_file, left_side);
    g_signal_connect(right_tree, "button-press-event", (GCallback) tree_view_click, right_side);
    g_signal_connect(left_tree, "button-press-event", (GCallback) tree_view_click, left_side);

    g_signal_connect(left_location_entry, "key-press-event", (GCallback) location_entry_keypress, left_side);
    g_signal_connect(right_location_entry, "key-press-event", (GCallback) location_entry_keypress, right_side);
    g_signal_connect(left_location_entry, "focus-out-event", (GCallback) location_entry_lost_focus, left_side);
    g_signal_connect(right_location_entry, "focus-out-event", (GCallback) location_entry_lost_focus, right_side);

    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(right_tree), GTK_SELECTION_MULTIPLE);
    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(left_tree), GTK_SELECTION_MULTIPLE);

    populate_list_store_from_folder(left_side, left_location_text);
    populate_list_store_from_folder(right_side, right_location_text);

    gtk_entry_set_text(left_location_entry, left_location_text);
    gtk_entry_set_text(right_location_entry, right_location_text);

    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(grid));

    gtk_grid_attach(grid, GTK_WIDGET(panes), 0, 0, 2, 1);
    gtk_grid_attach(grid, GTK_WIDGET(left_location_entry), 0, 2, 1, 1);
    gtk_grid_attach(grid, GTK_WIDGET(right_location_entry), 1, 2, 1, 1);

    gtk_container_add(GTK_CONTAINER(left_scroll_box), GTK_WIDGET(left_tree));
    gtk_container_add(GTK_CONTAINER(right_scroll_box), GTK_WIDGET(right_tree));

    gtk_paned_pack1(GTK_PANED(panes), GTK_WIDGET(left_scroll_box), TRUE, FALSE);
    gtk_paned_pack2(GTK_PANED(panes), GTK_WIDGET(right_scroll_box), TRUE, FALSE);

    gtk_widget_set_hexpand(GTK_WIDGET(grid), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(grid), TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(panes), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(left_scroll_box), TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(left_scroll_box), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(right_scroll_box), TRUE);
    gtk_widget_set_hexpand(GTK_WIDGET(right_scroll_box), TRUE);

    gtk_widget_show_all(window);
}

int
main (int argc,
      char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}



