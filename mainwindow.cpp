#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QString>
#include <algorithm>

void loadDataFromDB();
void Categorize();
void Filter(std::vector<Material>&allMaterials, std::vector<t_HC>&Hard_Constraints);
void Selection_Method2(std::vector<Material>&Filtered_Materials, std::string objectives, std::string geometry, std::string constraint, bool option);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Material Selection Engine");
    resize(1100, 650);

    loadDataFromDB();
    Categorize();

    setupUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // root widget
    QWidget *central = new QWidget(this);
    central->setStyleSheet("background: #f5f5f5;");
    setCentralWidget(central);

    QHBoxLayout *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── SIDEBAR ──────────────────────────────────────────────
    QWidget *sidebar = new QWidget();
    sidebar->setFixedWidth(270);
    sidebar->setStyleSheet(R"(
        QWidget       { background: #ffffff; color: #222222; }
        QGroupBox     { font-size: 12px; font-weight: bold; color: #444444;
                        border: 1px solid #dddddd; border-radius: 6px;
                        margin-top: 10px; padding: 8px 6px 6px 6px; }
        QGroupBox::title { subcontrol-origin: margin; left: 8px; padding: 0 4px; }
        QComboBox     { border: 1px solid #cccccc; border-radius: 4px;
                        padding: 4px 8px; font-size: 12px; background: #fafafa; }
        QComboBox:hover { border-color: #185FA5; }
        QLineEdit     { border: 1px solid #cccccc; border-radius: 4px;
                        padding: 4px 8px; font-size: 12px; background: #fafafa; }
        QLineEdit:focus { border-color: #185FA5; }
        QLabel        { font-size: 12px; color: #555555; }
        QPushButton   { border: 1px solid #cccccc; border-radius: 4px;
                        padding: 5px 10px; font-size: 12px; background: #fafafa; }
        QPushButton:hover  { background: #eeeeee; }
        QPushButton:pressed { background: #e0e0e0; }
    )");

    QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(12, 12, 12, 12);
    sideLayout->setSpacing(10);

    // scoring method group
    QGroupBox *methodGroup = new QGroupBox("Scoring Method");
    QVBoxLayout *methodLayout = new QVBoxLayout(methodGroup);
    methodLayout->setSpacing(6);
    methodCombo = new QComboBox();
    methodCombo->addItems({"Ashby", "WPI", "TOPSIS"});
    methodLayout->addWidget(methodCombo);
    sideLayout->addWidget(methodGroup);

    // ashby parameters group
    QGroupBox *ashbyGroup = new QGroupBox("Ashby Parameters");
    QVBoxLayout *ashbyLayout = new QVBoxLayout(ashbyGroup);
    ashbyLayout->setSpacing(6);

    ashbyLayout->addWidget(new QLabel("Objective"));
    objectiveCombo = new QComboBox();
    objectiveCombo->addItems({"mass"});
    ashbyLayout->addWidget(objectiveCombo);

    ashbyLayout->addWidget(new QLabel("Constraint Property"));
    constraintCombo = new QComboBox();
    constraintCombo->addItems({"stiffness","toughness","thermal-conductivity","electrical-resistivity"});
    ashbyLayout->addWidget(constraintCombo);

    ashbyLayout->addWidget(new QLabel("Geometry"));
    geometryCombo = new QComboBox();
    geometryCombo->addItems({"bending","tension","plate","torsion"});
    ashbyLayout->addWidget(geometryCombo);

    sideLayout->addWidget(ashbyGroup);

    // hard constraints group
    QGroupBox *constraintGroup = new QGroupBox("Hard Constraints");
    QVBoxLayout *constraintLayout = new QVBoxLayout(constraintGroup);
    constraintLayout->setSpacing(6);

    constraintLayout->addWidget(new QLabel("Property"));
    propertyCombo = new QComboBox();
    propertyCombo->addItems({
        "modulus","density","tensile-strength","hardness",
        "poisson-ratio","thermal-conductivity","melting-point",
        "thermal-expansion","heat-capacity","resistivity",
        "dielectric-constant","corrosion-resistance",
        "minimum-ph","maximum-ph","refractive-index","transparency"
    });
    constraintLayout->addWidget(propertyCombo);

    QHBoxLayout *opRow = new QHBoxLayout();
    opRow->setSpacing(6);
    operatorCombo = new QComboBox();
    operatorCombo->addItems({">=","<=",">","<","=="});
    operatorCombo->setFixedWidth(64);
    valueInput = new QLineEdit();
    valueInput->setPlaceholderText("enter value");
    opRow->addWidget(operatorCombo);
    opRow->addWidget(valueInput);
    constraintLayout->addLayout(opRow);

    addConstraintBtn = new QPushButton("+ Add Constraint");
    clearConstraintsBtn = new QPushButton("Clear All");
    constraintLayout->addWidget(addConstraintBtn);
    constraintLayout->addWidget(clearConstraintsBtn);

    sideLayout->addWidget(constraintGroup);
    sideLayout->addStretch();

    // run button
    runBtn = new QPushButton("Run Selection");
    runBtn->setFixedHeight(38);
    runBtn->setStyleSheet(R"(
        QPushButton {
            background: #185FA5; color: white; font-size: 13px;
            font-weight: bold; border-radius: 6px; border: none;
        }
        QPushButton:hover   { background: #1a6dbf; }
        QPushButton:pressed { background: #134d87; }
    )");
    sideLayout->addWidget(runBtn);

    root->addWidget(sidebar);

    // ── MAIN PANEL ───────────────────────────────────────────
    QWidget *mainPanel = new QWidget();
    mainPanel->setStyleSheet("background: #f5f5f5;");
    QVBoxLayout *panelLayout = new QVBoxLayout(mainPanel);
    panelLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->setSpacing(0);

    // stats bar
    QWidget *statsBar = new QWidget();
    statsBar->setFixedHeight(52);
    statsBar->setStyleSheet("background: #ffffff; border-bottom: 1px solid #dddddd;");
    QHBoxLayout *statsLayout = new QHBoxLayout(statsBar);
    statsLayout->setContentsMargins(16, 0, 16, 0);
    statsLayout->setSpacing(24);

    totalLabel       = new QLabel("Total: 0");
    filteredLabel    = new QLabel("Filtered: 0");
    topMaterialLabel = new QLabel("Top material: —");

    QString statStyle = "font-size: 13px; color: #333333;";
    totalLabel->setStyleSheet(statStyle);
    filteredLabel->setStyleSheet(statStyle);
    topMaterialLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #185FA5;");

    statsLayout->addWidget(totalLabel);
    statsLayout->addWidget(filteredLabel);
    statsLayout->addStretch();
    statsLayout->addWidget(topMaterialLabel);
    panelLayout->addWidget(statsBar);

    // results table
    resultsTable = new QTableWidget();
    resultsTable->setColumnCount(6);
    resultsTable->setHorizontalHeaderLabels({"#","Material","Family","E (GPa)","ρ (g/cm³)","Score"});
    resultsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    resultsTable->horizontalHeader()->setDefaultSectionSize(90);
    resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultsTable->setAlternatingRowColors(true);
    resultsTable->verticalHeader()->setVisible(false);
    resultsTable->setShowGrid(true);
    resultsTable->setStyleSheet(R"(
        QTableWidget {
            background: #ffffff; font-size: 12px;
            border: none; gridline-color: #eeeeee;
        }
        QHeaderView::section {
            background: #f0f0f0; color: #444444;
            font-size: 11px; font-weight: bold;
            padding: 6px; border: none;
            border-bottom: 1px solid #dddddd;
        }
        QTableWidget::item { padding: 6px 10px; color: #222222; }
        QTableWidget::item:selected { background: #ddeeff; color: #222222; }
        QTableWidget::item:alternate { background: #fafafa; }
    )");
    panelLayout->addWidget(resultsTable);

    root->addWidget(mainPanel);

    // ── SIGNALS ──────────────────────────────────────────────
    connect(runBtn,             &QPushButton::clicked, this, &MainWindow::onRunClicked);
    connect(addConstraintBtn,   &QPushButton::clicked, this, &MainWindow::onAddConstraintClicked);
    connect(clearConstraintsBtn,&QPushButton::clicked, this, &MainWindow::onClearConstraintsClicked);

    totalLabel->setText("Total: " + QString::number(allMaterials.size()));
}

void MainWindow::onAddConstraintClicked()
{
    QString prop = propertyCombo->currentText();
    QString op   = operatorCombo->currentText();
    QString val  = valueInput->text().trimmed();

    if (val.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a value.");
        return;
    }

    t_HC c;
    c.property_name = prop.toStdString();
    c.symbol        = op.toStdString();
    c.value         = val.toDouble();
    c.string_value  = val.toStdString();
    Hard_Constraints.push_back(c);

    valueInput->clear();
    QMessageBox::information(this, "Constraint Added",
                             prop + " " + op + " " + val + " added.");
}

void MainWindow::onClearConstraintsClicked()
{
    Hard_Constraints.clear();
    Filtered_Materials.clear();
    resultsTable->setRowCount(0);
    filteredLabel->setText("Filtered: 0");
    topMaterialLabel->setText("Top material: —");
}

void MainWindow::onRunClicked()
{
    if (Hard_Constraints.empty()) {
        QMessageBox::warning(this, "No Constraints", "Add at least one constraint before running.");
        return;
    }

    Filtered_Materials.clear();
    Filter(allMaterials, Hard_Constraints);

    if (Filtered_Materials.empty()) {
        QMessageBox::information(this, "No Results", "No materials passed the constraints.");
        filteredLabel->setText("Filtered: 0");
        topMaterialLabel->setText("Top material: —");
        resultsTable->setRowCount(0);
        return;
    }

    std::string obj = objectiveCombo->currentText().toStdString();
    std::string geo = geometryCombo->currentText().toStdString();
    std::string con = constraintCombo->currentText().toStdString();

    Selection_Method2(Filtered_Materials, obj, geo, con, false);

    std::sort(Filtered_Materials.begin(), Filtered_Materials.end(),
              [](const Material &a, const Material &b){
                  return a.performance_score > b.performance_score;
              });

    filteredLabel->setText("Filtered: " + QString::number(Filtered_Materials.size()));
    topMaterialLabel->setText("Top material: " +
                              QString::fromStdString(Filtered_Materials[0].name));

    populateTable();
}

void MainWindow::populateTable()
{
    resultsTable->setRowCount(0);

    for (int i = 0; i < (int)Filtered_Materials.size(); i++) {
        const Material &m = Filtered_Materials[i];
        resultsTable->insertRow(i);

        auto item = [](const QString &text, bool bold = false) {
            QTableWidgetItem *it = new QTableWidgetItem(text);
            it->setTextAlignment(Qt::AlignCenter);
            if (bold) {
                QFont f = it->font();
                f.setBold(true);
                it->setFont(f);
            }
            return it;
        };

        resultsTable->setItem(i, 0, item(QString::number(i + 1)));
        resultsTable->setItem(i, 1, item(QString::fromStdString(m.name), i == 0));
        resultsTable->setItem(i, 2, item(QString::fromStdString(m.family)));
        resultsTable->setItem(i, 3, item(QString::number(m.mech.modulus,  'f', 1)));
        resultsTable->setItem(i, 4, item(QString::number(m.mech.density,  'f', 2)));
        resultsTable->setItem(i, 5, item(QString::number(m.performance_score, 'e', 4)));

        if (i == 0) {
            for (int col = 0; col < 6; col++) {
                resultsTable->item(i, col)->setBackground(QColor("#ddeeff"));
            }
        }
    }
}
