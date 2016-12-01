#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QPushButton>
#include "pageturner.h"
#include "drawable.h"
#include "renderarea361.h"

class Window361 : public QWidget
{
    Q_OBJECT

public:
    Window361();
    ~Window361();
    Drawable *getDrawable();
    void setPageTurner(PageTurner *pageTurner);

private:
    RenderArea361 *renderArea;
    QPushButton *nextPageButton;
    QPushButton *refreshPageButton;
    PageTurner *pageTurner;


private slots:
    void nextPageClicked();
    void refreshPageClicked();

};

#endif // WINDOW_H
