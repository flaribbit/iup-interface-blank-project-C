#include <stdio.h>
#include "iup.h"

int Close_dlg(){
    return IUP_CLOSE;
}

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
                    *send=IupButton("发送(&S)",NULL),
                    *close=IupButton("关闭(&C)",NULL);
    record=IupMultiLine(NULL);
    row_insert=IupHbox(insert_emoji,insert_image,NULL);
    message=IupText(NULL);
    row_bottom=IupHbox(fill,close,send,NULL);
    split_bottom=IupVbox(row_insert,message,row_bottom,NULL);
    split=IupSplit(record,split_bottom);
    ui=IupHbox(split,NULL);
    dlg=IupDialog(ui);
    IupSetAttribute(split,"ORIENTATION","HORIZONTAL");
    IupSetAttribute(split,"SHOWGRIP","NO");
    IupSetAttribute(split,"VALUE","500");
    IupSetAttribute(record,"EXPAND","YES");
    IupSetAttribute(message,"EXPAND","YES");
    IupSetAttribute(dlg,"SIZE","360x160");
    IupSetAttribute(dlg,"SHRINK","YES");
    IupSetAttribute(ui,"NMARGIN","4x4");
    IupSetAttribute(row_insert,"NMARGIN","0x4");
    IupSetAttribute(row_bottom,"NMARGIN","0x4");
    IupSetCallback(close,"ACTION",(Icallback)Close_dlg);
    Ihandle *menu,*chat,*announce,*album,*file,*settings,*settings0,*settings1,*settings2;
    menu=IupMenu(
        chat=IupSubmenu("聊天",IupMenu(NULL)),
        announce=IupSubmenu("公告",IupMenu(NULL)),
        album=IupSubmenu("相册",IupMenu(NULL)),
        file=IupSubmenu("文件",IupMenu(NULL)),
        settings=IupSubmenu("设置",IupMenu(
            settings0=IupItem("查看群资料",NULL),
            settings1=IupItem("更新群信息",NULL),
            IupSeparator(),
            settings2=IupItem("退出该群",NULL),
            NULL)),
        NULL);
    IupSetHandle("mymenu",menu);
    IupSetAttribute(dlg,"TITLE","QQ群");
    IupSetAttribute(dlg,"MENU","mymenu");
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
