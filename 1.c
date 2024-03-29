#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/iup.h"

/********************************** Utilities *****************************************/

int str_compare(const char *l, const char *r, int casesensitive)
{
    if (!l || !r)
        return 0;

    while (*l && *r)
    {
        int diff;
        char l_char = *l,
             r_char = *r;

        /* compute the difference of both characters */
        if (casesensitive)
            diff = l_char - r_char;
        else
            diff = tolower((int)l_char) - tolower((int)r_char);

        /* if they differ we have a result */
        if (diff != 0)
            return 0;

        /* otherwise process the next characters */
        ++l;
        ++r;
    }

    /* check also for terminator */
    if (*l == *r)
        return 1;

    if (*r == 0)
        return 1; /* if second string is at terminator, then it is partially equal */

    return 0;
}

int str_find(const char *str, const char *str_to_find, int casesensitive)
{
    int i, str_len, str_to_find_len, count;

    if (!str || str[0] == 0 || !str_to_find || str_to_find[0] == 0)
        return -1;

    str_len = (int)strlen(str);
    str_to_find_len = (int)strlen(str_to_find);
    count = str_len - str_to_find_len;
    if (count < 0)
        return -1;

    count++;

    for (i = 0; i < count; i++)
    {
        if (str_compare(str, str_to_find, casesensitive))
            return i;

        str++;
    }

    return -1;
}

char *read_file(const char *filename)
{
    int size;
    char *str;
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        IupMessagef("Error", "Can't open file: %s", filename);
        return NULL;
    }

    /* calculate file size */
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    /* allocate memory for the file contents + nul terminator */
    str = malloc(size + 1);
    /* read all data at once */
    fread(str, size, 1, file);
    /* set the nul terminator */
    str[size] = 0;

    if (ferror(file))
        IupMessagef("Error", "Fail when reading from file: %s", filename);

    fclose(file);
    return str;
}

void write_file(const char *filename, const char *str, int count)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        IupMessagef("Error", "Can't open file: %s", filename);
        return;
    }

    fwrite(str, 1, count, file);

    if (ferror(file))
        IupMessagef("Error", "Fail when writing to file: %s", filename);

    fclose(file);
}

/********************************** Callbacks *****************************************/

int item_open_action_cb(Ihandle *item_open)
{
    Ihandle *multitext = IupGetDialogChild(item_open, "MULTITEXT");
    Ihandle *filedlg = IupFileDlg();
    IupSetAttribute(filedlg, "DIALOGTYPE", "OPEN");
    IupSetAttribute(filedlg, "EXTFILTER", "Text Files|*.txt|All Files|*.*|");
    IupSetAttributeHandle(filedlg, "PARENTDIALOG", IupGetDialog(item_open));

    IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

    if (IupGetInt(filedlg, "STATUS") != -1)
    {
        char *filename = IupGetAttribute(filedlg, "VALUE");
        char *str = read_file(filename);
        if (str)
        {
            IupSetStrAttribute(multitext, "VALUE", str);
            free(str);
        }
    }

    IupDestroy(filedlg);
    return IUP_DEFAULT;
}

int item_saveas_action_cb(Ihandle *item_saveas)
{
    Ihandle *multitext = IupGetDialogChild(item_saveas, "MULTITEXT");
    Ihandle *filedlg = IupFileDlg();
    IupSetAttribute(filedlg, "DIALOGTYPE", "SAVE");
    IupSetAttribute(filedlg, "EXTFILTER", "Text Files|*.txt|All Files|*.*|");
    IupSetAttributeHandle(filedlg, "PARENTDIALOG", IupGetDialog(item_saveas));

    IupPopup(filedlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

    if (IupGetInt(filedlg, "STATUS") != -1)
    {
        char *filename = IupGetAttribute(filedlg, "VALUE");
        char *str = IupGetAttribute(multitext, "VALUE");
        int count = IupGetInt(multitext, "COUNT");
        write_file(filename, str, count);
    }

    IupDestroy(filedlg);
    return IUP_DEFAULT;
}

int item_exit_action_cb(void)
{
    return IUP_CLOSE;
}

int goto_ok_action_cb(Ihandle *bt_ok)
{
    int line_count = IupGetInt(bt_ok, "TEXT_LINECOUNT");
    Ihandle *txt = IupGetDialogChild(bt_ok, "LINE_TEXT");
    int line = IupGetInt(txt, "VALUE");
    if (line < 1 || line >= line_count)
    {
        IupMessage("Error", "Invalid line number.");
        return IUP_DEFAULT;
    }

    IupSetAttribute(IupGetDialog(bt_ok), "STATUS", "1");
    return IUP_CLOSE;
}

int goto_cancel_action_cb(Ihandle *bt_ok)
{
    IupSetAttribute(IupGetDialog(bt_ok), "STATUS", "0");
    return IUP_CLOSE;
}

int item_goto_action_cb(Ihandle *item_goto)
{
    Ihandle *multitext = IupGetDialogChild(item_goto, "MULTITEXT");
    Ihandle *dlg, *box, *bt_ok, *bt_cancel, *txt, *lbl;

    int line_count = IupGetInt(multitext, "LINECOUNT");

    lbl = IupLabel(NULL);
    IupSetfAttribute(lbl, "TITLE", "Line Number [1-%d]:", line_count);
    txt = IupText(NULL);
    IupSetAttribute(txt, "MASK", IUP_MASK_UINT); /* unsigned integer numbers only */
    IupSetAttribute(txt, "NAME", "LINE_TEXT");
    IupSetAttribute(txt, "VISIBLECOLUMNS", "20");
    bt_ok = IupButton("OK", NULL);
    IupSetInt(bt_ok, "TEXT_LINECOUNT", line_count);
    IupSetAttribute(bt_ok, "PADDING", "10x2");
    IupSetCallback(bt_ok, "ACTION", (Icallback)goto_ok_action_cb);
    bt_cancel = IupButton("Cancel", NULL);
    IupSetCallback(bt_cancel, "ACTION", (Icallback)goto_cancel_action_cb);
    IupSetAttribute(bt_cancel, "PADDING", "10x2");

    box = IupVbox(
        lbl,
        txt,
        IupSetAttributes(IupHbox(
                             IupFill(),
                             bt_ok,
                             bt_cancel,
                             NULL),
                         "NORMALIZESIZE=HORIZONTAL"),
        NULL);
    IupSetAttribute(box, "MARGIN", "10x10");
    IupSetAttribute(box, "GAP", "5");

    dlg = IupDialog(box);
    IupSetAttribute(dlg, "TITLE", "Go To Line");
    IupSetAttribute(dlg, "DIALOGFRAME", "Yes");
    IupSetAttributeHandle(dlg, "DEFAULTENTER", bt_ok);
    IupSetAttributeHandle(dlg, "DEFAULTESC", bt_cancel);
    IupSetAttributeHandle(dlg, "PARENTDIALOG", IupGetDialog(item_goto));

    IupPopup(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

    if (IupGetInt(dlg, "STATUS") == 1)
    {
        int line = IupGetInt(txt, "VALUE");
        int pos;
        IupTextConvertLinColToPos(multitext, line, 0, &pos);
        IupSetInt(multitext, "CARETPOS", pos);
        IupSetInt(multitext, "SCROLLTOPOS", pos);
    }

    IupDestroy(dlg);

    return IUP_DEFAULT;
}

int find_next_action_cb(Ihandle *bt_next)
{
    Ihandle *multitext = (Ihandle *)IupGetAttribute(bt_next, "MULTITEXT");
    char *str = IupGetAttribute(multitext, "VALUE");
    int find_pos = IupGetInt(multitext, "FIND_POS");

    Ihandle *txt = IupGetDialogChild(bt_next, "FIND_TEXT");
    char *str_to_find = IupGetAttribute(txt, "VALUE");

    Ihandle *find_case = IupGetDialogChild(bt_next, "FIND_CASE");
    int casesensitive = IupGetInt(find_case, "VALUE");

    int pos = str_find(str + find_pos, str_to_find, casesensitive);
    if (pos >= 0)
        pos += find_pos;
    else if (find_pos > 0)
        pos = str_find(str, str_to_find, casesensitive); /* try again from the start */

    if (pos >= 0)
    {
        int lin, col,
            end_pos = pos + (int)strlen(str_to_find);

        IupSetInt(multitext, "FIND_POS", end_pos);

        IupSetFocus(multitext);
        IupSetfAttribute(multitext, "SELECTIONPOS", "%d:%d", pos, end_pos);

        IupTextConvertPosToLinCol(multitext, pos, &lin, &col);
        IupTextConvertLinColToPos(multitext, lin, 0, &pos); /* position at col=0, just scroll lines */
        IupSetInt(multitext, "SCROLLTOPOS", pos);
    }
    else
        IupMessage("Warning", "Text not found.");

    return IUP_DEFAULT;
}

int find_close_action_cb(Ihandle *bt_close)
{
    IupHide(IupGetDialog(bt_close));
    return IUP_DEFAULT;
}

int item_find_action_cb(Ihandle *item_find)
{
    Ihandle *dlg = (Ihandle *)IupGetAttribute(item_find, "FIND_DIALOG");
    if (!dlg)
    {
        Ihandle *multitext = IupGetDialogChild(item_find, "MULTITEXT");
        Ihandle *box, *bt_next, *bt_close, *txt, *find_case;

        txt = IupText(NULL);
        IupSetAttribute(txt, "NAME", "FIND_TEXT");
        IupSetAttribute(txt, "VISIBLECOLUMNS", "20");
        find_case = IupToggle("Case Sensitive", NULL);
        IupSetAttribute(find_case, "NAME", "FIND_CASE");
        bt_next = IupButton("Find Next", NULL);
        IupSetAttribute(bt_next, "PADDING", "10x2");
        IupSetCallback(bt_next, "ACTION", (Icallback)find_next_action_cb);
        bt_close = IupButton("Close", NULL);
        IupSetCallback(bt_close, "ACTION", (Icallback)find_close_action_cb);
        IupSetAttribute(bt_close, "PADDING", "10x2");

        box = IupVbox(
            IupLabel("Find What:"),
            txt,
            find_case,
            IupSetAttributes(IupHbox(
                                 IupFill(),
                                 bt_next,
                                 bt_close,
                                 NULL),
                             "NORMALIZESIZE=HORIZONTAL"),
            NULL);
        IupSetAttribute(box, "MARGIN", "10x10");
        IupSetAttribute(box, "GAP", "5");

        dlg = IupDialog(box);
        IupSetAttribute(dlg, "TITLE", "Find");
        IupSetAttribute(dlg, "DIALOGFRAME", "Yes");
        IupSetAttributeHandle(dlg, "DEFAULTENTER", bt_next);
        IupSetAttributeHandle(dlg, "DEFAULTESC", bt_close);
        IupSetAttributeHandle(dlg, "PARENTDIALOG", IupGetDialog(item_find));

        /* Save the multiline to acess it from the callbacks */
        IupSetAttribute(dlg, "MULTITEXT", (char *)multitext);

        /* Save the dialog to reuse it */
        IupSetAttribute(item_find, "FIND_DIALOG", (char *)dlg);
    }

    /* centerparent first time, next time reuse the last position */
    IupShowXY(dlg, IUP_CURRENT, IUP_CURRENT);

    return IUP_DEFAULT;
}

int item_font_action_cb(Ihandle *item_font)
{
    Ihandle *multitext = IupGetDialogChild(item_font, "MULTITEXT");
    Ihandle *fontdlg = IupFontDlg();
    char *font = IupGetAttribute(multitext, "FONT");
    IupSetStrAttribute(fontdlg, "VALUE", font);
    IupSetAttributeHandle(fontdlg, "PARENTDIALOG", IupGetDialog(item_font));

    IupPopup(fontdlg, IUP_CENTERPARENT, IUP_CENTERPARENT);

    if (IupGetInt(fontdlg, "STATUS") == 1)
    {
        font = IupGetAttribute(fontdlg, "VALUE");
        IupSetStrAttribute(multitext, "FONT", font);
    }

    IupDestroy(fontdlg);
    return IUP_DEFAULT;
}

int item_about_action_cb(void)
{
    IupMessage("About", "   Simple Notepad\n\nAuthors:\n   Gustavo Lyrio\n   Antonio Scuri");
    return IUP_DEFAULT;
}

/********************************** Main *****************************************/

int main(int argc, char **argv)
{
    Ihandle *dlg, *vbox, *multitext, *menu;
    Ihandle *sub_menu_file, *file_menu, *item_exit, *item_open, *item_saveas;
    Ihandle *sub_menu_edit, *edit_menu, *item_find, *item_goto;
    Ihandle *sub_menu_format, *format_menu, *item_font;
    Ihandle *sub_menu_help, *help_menu, *item_about;

    IupOpen(&argc, &argv);

    multitext = IupText(NULL);
    IupSetAttribute(multitext, "MULTILINE", "YES");
    IupSetAttribute(multitext, "EXPAND", "YES");
    IupSetAttribute(multitext, "NAME", "MULTITEXT");

    item_open = IupItem("Open...", NULL);
    item_saveas = IupItem("Save As...", NULL);
    item_exit = IupItem("Exit", NULL);
    item_find = IupItem("Find..", NULL);
    item_goto = IupItem("Go To...", NULL);
    item_font = IupItem("Font...", NULL);
    item_about = IupItem("About...", NULL);

    IupSetCallback(item_open, "ACTION", (Icallback)item_open_action_cb);
    IupSetCallback(item_saveas, "ACTION", (Icallback)item_saveas_action_cb);
    IupSetCallback(item_exit, "ACTION", (Icallback)item_exit_action_cb);
    IupSetCallback(item_find, "ACTION", (Icallback)item_find_action_cb);
    IupSetCallback(item_goto, "ACTION", (Icallback)item_goto_action_cb);
    IupSetCallback(item_font, "ACTION", (Icallback)item_font_action_cb);
    IupSetCallback(item_about, "ACTION", (Icallback)item_about_action_cb);

    file_menu = IupMenu(
        item_open,
        item_saveas,
        IupSeparator(),
        item_exit,
        NULL);
    edit_menu = IupMenu(
        item_find,
        item_goto,
        NULL);
    format_menu = IupMenu(
        item_font,
        NULL);
    help_menu = IupMenu(
        item_about,
        NULL);

    sub_menu_file = IupSubmenu("File", file_menu);
    sub_menu_edit = IupSubmenu("Edit", edit_menu);
    sub_menu_format = IupSubmenu("Format", format_menu);
    sub_menu_help = IupSubmenu("Help", help_menu);

    menu = IupMenu(
        sub_menu_file,
        sub_menu_edit,
        sub_menu_format,
        sub_menu_help,
        NULL);

    vbox = IupVbox(
        multitext,
        NULL);

    dlg = IupDialog(vbox);
    IupSetAttributeHandle(dlg, "MENU", menu);
    IupSetAttribute(dlg, "TITLE", "Simple Notepad");
    IupSetAttribute(dlg, "SIZE", "HALFxHALF");

    /* parent for pre-defined dialogs in closed functions (IupMessage) */
    IupSetAttributeHandle(NULL, "PARENTDIALOG", dlg);

    IupShowXY(dlg, IUP_CENTERPARENT, IUP_CENTERPARENT);
    IupSetAttribute(dlg, "USERSIZE", NULL);

    IupMainLoop();

    IupClose();
    return EXIT_SUCCESS;
}
