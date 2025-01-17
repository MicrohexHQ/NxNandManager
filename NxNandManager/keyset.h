#ifndef KEYSET_H
#define KEYSET_H
#include <QMainWindow>
#include <QDialog>
#include <QtWidgets>
#include "utils.h"
#include <QObject>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;

namespace Ui {
    class DialogKeySet;
}

class KeySetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeySetDialog(QWidget *parent = nullptr);
    ~KeySetDialog();
    Ui::DialogKeySet *ui;

private:
    QWidget *parent;

signals:
    void finished();

public slots:
private slots:
    void on_ImportButton_clicked();
    void on_buttonBox_accepted();
};

#endif // KEYSET_H
