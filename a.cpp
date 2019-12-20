#include <stdio.h>
#include "iup.h"

void SplitTest(void)
{
    Ihandle *dlg,
    *ui,
        *split,
            *record,
            *split_bottom,
                *row_insert,
                    *insert_emoji=IupButton("表情",NULL),
                    *insert_image=IupButton("图片",NULL),
                *message,
                *row_bottom,
                    *fill=IupFill(),
                    *send=IupButton("发送",NULL),
                    *close=IupButton("关闭",NULL);
    record=IupMultiLine(NULL);
    row_insert=IupHbox(insert_emoji,insert_image,NULL);
    message=IupText(NULL);
    row_bottom=IupHbox(fill,send,close,NULL);
    split_bottom=IupVbox(row_insert,message,row_bottom,NULL);
    split=IupSplit(record,split_bottom);
    ui=IupHbox(split,NULL);
    dlg=IupDialog(ui);
    IupSetAttribute(split,"ORIENTATION","HORIZONTAL");
    IupSetAttribute(split,"SHOWGRIP","NO");
    IupSetAttribute(record,"EXPAND","YES");
    IupSetAttribute(split_bottom,"EXPAND","YES");
    IupSetAttribute(message,"EXPAND","YES");
    IupSetAttribute(row_bottom,"ALIGNMENT","RIGHT");
    IupSetAttribute(ui,"MARGIN","4x4");
    IupSetAttribute(split,"SIZE","320x120");
    IupSetAttribute(record,"SIZE","0x40");
    IupSetAttribute(row_insert,"MARGIN","0x4");
    IupSetAttribute(split_bottom,"MARGIN","0x0");
    IupSetAttribute(row_bottom,"MARGIN","0x4");
    IupSetAttribute(dlg,"TITLE","测试");
    Ihandle *chat,*announce,*file;
    //IupSubmenu();
    IupShow(dlg);
}

int main(int argc, char *argv[])
{
    IupOpen(&argc, &argv);
    IupSetGlobal("UTF8MODE","YES");
    SplitTest();
    IupMainLoop();
    IupClose();
    return 0;
}
