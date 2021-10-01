#ifndef EQPAYPUSHBUTTON_H
#define EQPAYPUSHBUTTON_H
#include <QPushButton>
#include <QStyleOptionButton>
#include <QIcon>

class EquityPayPushButton : public QPushButton
{
public:
    explicit EquityPayPushButton(QWidget * parent = Q_NULLPTR);
    explicit EquityPayPushButton(const QString &text, QWidget *parent = Q_NULLPTR);

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    void updateIcon(QStyleOptionButton &pushbutton);

private:
    bool m_iconCached;
    QIcon m_downIcon;
};

#endif // EQPAYPUSHBUTTON_H
