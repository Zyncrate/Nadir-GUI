#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QButtonGroup>
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

private slots:
    void onRunClicked();
    void onAddConstraintClicked();
    void onClearConstraintsClicked();
    void onRemoveConstraintClicked();

private:
    Ui::MainWindow *ui;

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
};

#endif // MAINWINDOW_H
