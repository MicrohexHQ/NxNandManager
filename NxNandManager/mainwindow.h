#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QtWinExtras>
#include <QtWinExtras>
#include <QWinTaskbarProgress>
#include <QTableWidgetItem>
#include "NxStorage.h"
#include "worker.h"
#include "opendrive.h"
#include "keyset.h"
#include "properties.h"

#define DUMP	102
#define RESTORE 103

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class Worker;

namespace Ui {
    class MainWindow;
}
using namespace std;


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	Worker *workerThread;

private:
	Ui::MainWindow *ui;
    OpenDrive* openDriveDialog;
    KeySetDialog* keysetDialog;
    Properties* PropertiesDialog;
	NxStorage* input;
	NxStorage* selected_io;
	bool m_ready;
	Worker* workThread;
	timepoint_t startWork;
	timepoint_t startWorkMD5;
	timepoint_t remainingTimeWork;
	bool workInProgress = false;
	bool bypassMD5 = false;
	bool progressMD5 = false;
	int cur_operation = 0;
	QWinTaskbarButton *TaskBarButton;
	QWinTaskbarProgress *TaskBarProgress;
	bool bTaskBarSet = FALSE;
    bool bKeyset;

	void createActions();
	void startWorkThread();
	void endWorkThread();
	void setProgressBarStyle(QString color = nullptr);
	void initButtons();

protected:
	void showEvent(QShowEvent *e) override;
	void closeEvent(QCloseEvent *event) override;

private slots:
	void open();
	void openDrive();
    void Properties();
    void openKeySet();
    void incognito();
    void dumpPartition(int crypto_mode=NULL);
    void dumpDecPartition();
    void dumpEncPartition();
	void restorePartition();
	void toggleAutoRCM();
    void on_rawdump_button_clicked(int crypto_mode=NULL);
    void on_rawdumpDec_button_clicked();
    void on_rawdumpEnc_button_clicked();
	void on_stop_button_clicked();
	void on_fullrestore_button_clicked();
	void on_partition_table_itemSelectionChanged();
    void on_moreinfo_button_clicked();

public slots:
	void inputSet(NxStorage *storage = nullptr);
	void driveSet(QString);
    void keySetSet();
	void error(int err, QString label = nullptr);
	void updateProgress(int percent, u64 *bytesAmount);
	void MD5begin();
	void timer1000();

public:
    KeySet biskeys;
};

#endif // MAINWINDOW_H
