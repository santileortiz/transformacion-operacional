#include <MyTextEdit.h>
#include <QKeyEvent>
#include <iostream>

void MyTextEdit::keyPressEvent(QKeyEvent * event)
{
    int k = event->key();
    if(k == Qt::Key_F1) {
        ignore_incoming = !ignore_incoming;
        if (ignore_incoming) {
            std::cout << "Ignorando entrada" << std::endl;
        } else {
            std::cout << "Oyendo entrada" << std::endl;
        }
    }
    
    if(k == Qt::Key_Backspace) {
        is_backspace = true;
    } else {
        is_backspace = false;
    }
    QTextEdit::keyPressEvent(event);
}

