#ifndef EQPAYPUSHBUTTON_H
#define EQPAYPUSHBUTTON_H
#include <QPushButton>
#include <QStyleOptionButton>
#include <QIcon>

class EqPayPushButton : public QPushButton
{
public:
    explicit EqPayPushButton(QWidget * parent = Q_NULLPTR);
    explicit EqPayPushButton(const QString &text, QWidget *parent = Q_NULLPTR);

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    void updateIcon(QStyleOptionButton &pushbutton);

private:
    bool m_iconCached;
    QIcon m_downIcon;
};

#endif // EQPAYPUSHBUTTON_H
