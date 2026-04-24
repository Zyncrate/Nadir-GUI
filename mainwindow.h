#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QDialog>
#include <QScrollArea>
#include "../include/globals.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onRunClicked();
    void onAddConstraintClicked();
    void onClearConstraintsClicked();
    void onRemoveConstraintClicked();
    void onTableRowClicked(int row, int column);

private:
    Ui::MainWindow *ui;

    QScrollArea  *sideScroll;

    // sidebar widgets
    QComboBox    *methodCombo;
    QComboBox    *objectiveCombo;
    QComboBox    *constraintCombo;
    QComboBox    *geometryCombo;
    QComboBox    *familyCombo;
    QComboBox    *propertyCombo;
    QComboBox    *operatorCombo;
    QLineEdit    *valueInput;
    QPushButton  *addConstraintBtn;
    QPushButton  *clearConstraintsBtn;
    QPushButton  *runBtn;
    QPushButton  *maximizeBtn;
    QPushButton  *minimizeBtn;
    QListWidget  *constraintList;

    // results widgets
    QTableWidget *resultsTable;
    QLabel       *totalLabel;
    QLabel       *filteredLabel;
    QLabel       *topMaterialLabel;

    bool optimiseMaximize = false;

    void setupUI();
    void populateTable();
    void refreshConstraintList();
    void showMaterialDetail(const Material &m);
};

#endif // MAINWINDOW_H
