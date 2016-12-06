#ifndef PAGETURNER_H
#define PAGETURNER_H

#include <QString>

class PageTurner
{
public:
    virtual void nextPage(QString filename) = 0;
};

#endif // PAGETURNER_H
