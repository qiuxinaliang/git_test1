#include "public.h"
#include <QString>

Public::Public(QWidget *parent) : QWidget(parent)
{

}

void Public::hexstringToBytes(QString *string, uint8_t *hexBytes)
{
    int bytelen = string->length() / 2;
    QString strbyte;
    for(int i = 0; i < bytelen; i++)
    {
        strbyte = string-> mid(i *2, 2);
        hexBytes[i] = static_cast<uint8_t>(strbyte.toInt(nullptr, 16));
    }
}
