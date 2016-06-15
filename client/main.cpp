/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
//#include <QApplication>

#include "editor_client.h"
//#include "cliente.h"
//#include "codeeditor.h"
//#include <iostream>

//using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



    EditorCliente ed;
    ed.show();

    return a.exec();
}

/*
void TextEdit::on_text_changed()
{
     cout << "Hola" << endl;
}


int main(int argv, char **args)
{
    QApplication app(argv, args);

    //CodeEditor editor;
    //editor.setWindowTitle(QObject::tr("Code Editor Example"));
    //editor.show();

    QTextEdit t;
    t.setWindowTitle("Texto");
    t.setText("Hola");
    //t.setReadOnly(true);
    t.show();
    //if(t.textChanged()) printf("Texto cambio\n");

    QString str = t.toPlainText();

    connect(txtEdit, SIGNAL(textChanged()), this, SLOT(on_text_changed()));

    cout << "Texto recibido: " << str.toStdString() << endl;

    //QObject::connect(&t, textChanged(),
//                    &str, SLOT(print()));

    return app.exec();
}
*/
