#include <QTextEdit>
class MyTextEdit : public QTextEdit 
{
    Q_OBJECT

public:
    bool is_backspace;
    bool ignore_incoming;
    MyTextEdit(QWidget *parent = 0) : QTextEdit(parent) {ignore_incoming=false;}
    ~MyTextEdit() {}
protected:
    virtual void keyPressEvent(QKeyEvent * event);
};

