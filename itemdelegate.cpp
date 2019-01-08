#include "itemdelegate.h"
#include <QLineEdit>
#include <QDoubleValidator>

ItemDelegate::ItemDelegate(QObject* parent) :
    QItemDelegate(parent)
{}

QWidget *ItemDelegate::createEditor(QWidget *parent,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    QLocale locale(QLocale::C);
    locale.setNumberOptions(QLocale::RejectGroupSeparator);
    auto validator = new QDoubleValidator;
    validator->setLocale(locale);
    editor->setValidator(validator);
    return editor;
}
