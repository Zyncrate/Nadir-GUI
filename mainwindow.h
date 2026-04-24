#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "../include/globals.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
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

private:
    Ui::MainWindow *ui;

    // sidebar widgets
    QComboBox   *objectiveCombo;
    QComboBox   *constraintCombo;
    QComboBox   *geometryCombo;
    QComboBox   *methodCombo;
    QComboBox   *propertyCombo;
    QComboBox   *operatorCombo;
    QLineEdit   *valueInput;
    QPushButton *addConstraintBtn;
    QPushButton *clearConstraintsBtn;
    QPushButton *runBtn;

    // results widgets
    QTableWidget *resultsTable;
    QLabel       *totalLabel;
    QLabel       *filteredLabel;
    QLabel       *topMaterialLabel;

    void setupUI();
    void populateTable();
};

#endif // MAINWINDOW_H
