#ifndef PUBLIC_H
#define PUBLIC_H

#include <QWidget>
#include <QString>

class Public : public QWidget
{
    Q_OBJECT
public:
    explicit Public(QWidget *parent = nullptr);
    void hexstringToBytes(QString *string, uint8_t *hexBytes);

signals:

public slots:
};

#endif // PUBLIC_H
