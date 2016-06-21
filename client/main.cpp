#include <QtWidgets>

#include "editor_client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EditorCliente ed;
    ed.show();

    return a.exec();
}

