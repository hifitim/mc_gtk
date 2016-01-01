//
// Created by tim on 12/5/15.
//

#ifndef MC_GUI_H
#define MC_GUI_H

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "util.h"

/*  globals   */
const int DEFAULT_STRING_LEN = 256;

/*  types   */
enum
{
    FILE_NAME_COLUMN,
    FILE_SIZE_COLUMN,
    ROW_COLOR_COLUMN,
    ROW_COLOR_COLUMN_SET,
    FILE_TYPE_COLUMN,
    N_COLUMNS
};

enum FILE_MOVE_MODE
{
    CUT_MODE,
    COPY_MODE,
    DISABLED
};

struct app_side
{
    char *current_location;
    GtkEntry *entry;
    GtkListStore *list_store;
    GtkTreeView *tree_view;
};

struct app_state
{
    int paste_active;
    char **files_to_move;
    unsigned int num_files_to_move;
    enum FILE_MOVE_MODE move_mode;
} current_state;

/*  function declarations   */
static void
        clear_list_store(GtkListStore *list_store);

static void
        open_file(GtkTreeView *treeview,
                  GtkTreePath *path,
                  GtkTreeViewColumn *col,
                  gpointer user_data);

static gboolean
        tree_view_click(GtkWidget *treeview,
                        GdkEventButton *event,
                        gpointer user_data);

static int
        populate_list_store_from_folder(struct app_side *side,
                                        char *path);

static void
        color_cell_function(GtkTreeViewColumn *col,
                            GtkCellRenderer *renderer,
                            GtkTreeModel *model,
                            GtkTreeIter *iter,
                            gpointer user_data);

static gboolean
        location_entry_keypress(GtkWidget *widget,
                                GdkEventKey *event,
                                gpointer user_data);

static gboolean
        location_entry_lost_focus(GtkWidget *widget,
                                  GdkEvent *event,
                                  gpointer user_data);

static void
        tree_right_click_copy(GtkWidget *menuitem,
                              gpointer user_data);

static void
        tree_right_click_cut(GtkWidget *menuitem,
                             gpointer user_data);

static void
        tree_right_click_paste(GtkWidget *menuitem,
                               gpointer user_data);


#endif //MC_GUI_H
