#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QString>
#include <QDialog>
#include <QFormLayout>
#include <QFrame>
#include <QDialogButtonBox>
#include <QEvent>
#include <QMouseEvent>
#include <QIcon>
#include <algorithm>

void loadDataFromDB();
void Categorize();
void Filter(std::vector<Material>&allMaterials, std::vector<t_HC>&Hard_Constraints);
void Selection_Method2(std::vector<Material>&Filtered_Materials, std::string objectives, std::string geometry, std::string constraint, bool option);

// ── NADIR DARK THEME ─────────────────────────────────────────
static const QString BASE_STYLE = R"(
    QWidget {
        background: #081120;
        color: #F5F7FA;
        font-size: 12px;
        font-family: Arial, sans-serif;
    }
    QGroupBox {
        font-size: 12px;
        font-weight: bold;
        color: #4FD1C5;
        border: 1px solid #1a2a40;
        border-radius: 6px;
        margin-top: 10px;
        padding: 8px 6px 6px 6px;
    }
    QGroupBox::title {
        subcontrol-origin: margin;
        left: 8px;
        padding: 0 4px;
        color: #4FD1C5;
    }
    QComboBox {
        border: 1px solid #1a2a40;
        border-radius: 4px;
        padding: 4px 8px;
        background: #0d1e36;
        color: #F5F7FA;
        combobox-popup: 0;
    }
    QComboBox:hover { border-color: #6F7CFF; }
    QComboBox QAbstractItemView {
        border: 1px solid #1a2a40;
        background: #0d1e36;
        color: #F5F7FA;
        selection-background-color: #6F7CFF;
        selection-color: #ffffff;
        outline: none;
    }
    QLineEdit {
        border: 1px solid #1a2a40;
        border-radius: 4px;
        padding: 4px 8px;
        background: #0d1e36;
        color: #F5F7FA;
    }
    QLineEdit:focus { border-color: #6F7CFF; }
    QLabel { color: #a0aec0; }
    QPushButton {
        border: 1px solid #1a2a40;
        border-radius: 4px;
        padding: 5px 10px;
        background: #0d1e36;
        color: #F5F7FA;
    }
    QPushButton:hover   { background: #1a2a40; border-color: #6F7CFF; }
    QPushButton:pressed { background: #6F7CFF; color: #ffffff; }
    QListWidget {
        border: 1px solid #1a2a40;
        border-radius: 4px;
        background: #0d1e36;
        color: #F5F7FA;
        font-size: 11px;
    }
    QListWidget::item {
        padding: 4px 6px;
        border-bottom: 1px solid #1a2a40;
    }
    QListWidget::item:selected {
        background: #6F7CFF;
        color: #ffffff;
    }
    QScrollBar:vertical {
        width: 6px;
        background: #081120;
    }
    QScrollBar::handle:vertical {
        background: #1a2a40;
        border-radius: 3px;
    }
    QScrollBar::add-line:vertical,
    QScrollBar::sub-line:vertical { height: 0px; }
)";

static const QString TABLE_STYLE = R"(
    QTableWidget {
        background: #0a1628;
        font-size: 12px;
        border: none;
        gridline-color: #1a2a40;
        color: #F5F7FA;
    }
    QHeaderView::section {
        background: #0d1e36;
        color: #4FD1C5;
        font-size: 11px;
        font-weight: bold;
        padding: 6px;
        border: none;
        border-bottom: 1px solid #1a2a40;
        border-right: 1px solid #1a2a40;
    }
    QTableWidget::item {
        padding: 6px 10px;
        color: #F5F7FA;
        border-bottom: 1px solid #1a2a40;
    }
    QTableWidget::item:selected {
        background: #6F7CFF;
        color: #ffffff;
    }
    QTableWidget::item:alternate {
        background: #0d1e36;
    }
)";

static const QString DIALOG_STYLE = R"(
    QDialog { background: #081120; }
    QLabel  { font-size: 12px; color: #cbd5e0; }
    QGroupBox {
        font-size: 12px; font-weight: bold; color: #4FD1C5;
        border: 1px solid #1a2a40; border-radius: 6px;
        margin-top: 10px; padding: 8px 6px 6px 6px;
    }
    QGroupBox::title {
        subcontrol-origin: margin; left: 8px;
        padding: 0 4px; color: #4FD1C5;
    }
    QPushButton {
        border: 1px solid #1a2a40; border-radius: 4px;
        padding: 6px 20px; background: #0d1e36;
        font-size: 12px; color: #F5F7FA;
    }
    QPushButton:hover { background: #1a2a40; border-color: #6F7CFF; }
    QFrame { color: #1a2a40; }
)";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Nadir — Material Selection Engine");
    setWindowIcon(QIcon(":/nadir.ico"));
    resize(1100, 700);

    loadDataFromDB();
    Categorize();
    setupUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == sideScroll->viewport()) {
        if (event->type() == QEvent::Wheel)
            return false;
        if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::MouseButtonRelease)
            return false;
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    central->setStyleSheet("background: #081120;");
    setCentralWidget(central);

    QHBoxLayout *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── SIDEBAR ───────────────────────────────────────────────
    sideScroll = new QScrollArea();
    sideScroll->setFixedWidth(300);
    sideScroll->setWidgetResizable(true);
    sideScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sideScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    sideScroll->setStyleSheet(
        "QScrollArea { border: none; border-right: 1px solid #1a2a40;"
        " background:#081120; }"
        "QScrollBar:vertical { width: 6px; background: #081120; }"
        "QScrollBar::handle:vertical { background: #1a2a40; border-radius: 3px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical"
        " { height: 0px; }"
        );
    sideScroll->viewport()->installEventFilter(this);

    QWidget *sidebar = new QWidget();
    sidebar->setStyleSheet(BASE_STYLE);

    QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(12, 12, 12, 12);
    sideLayout->setSpacing(10);

    // logo + title row
    QHBoxLayout *titleRow = new QHBoxLayout();
    QLabel *logoText = new QLabel("NADIR");
    logoText->setStyleSheet(
        "font-size: 20px; font-weight: bold;"
        "color: #6F7CFF; letter-spacing: 4px;");
    QLabel *versionLabel = new QLabel("v1.0");
    versionLabel->setStyleSheet("font-size: 10px; color: #445566;");
    titleRow->addWidget(logoText);
    titleRow->addStretch();
    titleRow->addWidget(versionLabel);
    sideLayout->addLayout(titleRow);

    QFrame *titleLine = new QFrame();
    titleLine->setFrameShape(QFrame::HLine);
    titleLine->setStyleSheet("color: #1a2a40;");
    sideLayout->addWidget(titleLine);

    // -- family filter --
    QGroupBox *familyGroup = new QGroupBox("Material Family");
    QVBoxLayout *familyLayout = new QVBoxLayout(familyGroup);
    familyLayout->setSpacing(6);
    familyCombo = new QComboBox();
    familyCombo->addItems({"All","Metal","Ceramic","Polymer",
                           "Composite","Semiconductor","Natural"});
    familyLayout->addWidget(familyCombo);
    sideLayout->addWidget(familyGroup);

    // -- scoring method --
    QGroupBox *methodGroup = new QGroupBox("Scoring Method");
    QVBoxLayout *methodLayout = new QVBoxLayout(methodGroup);
    methodLayout->setSpacing(6);
    methodCombo = new QComboBox();
    methodCombo->addItems({"Ashby","WPI","TOPSIS"});
    methodLayout->addWidget(methodCombo);
    sideLayout->addWidget(methodGroup);

    // -- ashby parameters --
    QGroupBox *ashbyGroup = new QGroupBox("Ashby Parameters");
    QVBoxLayout *ashbyLayout = new QVBoxLayout(ashbyGroup);
    ashbyLayout->setSpacing(6);

    ashbyLayout->addWidget(new QLabel("Objective"));
    objectiveCombo = new QComboBox();
    objectiveCombo->addItems({"mass"});
    ashbyLayout->addWidget(objectiveCombo);

    ashbyLayout->addWidget(new QLabel("Constraint Property"));
    constraintCombo = new QComboBox();
    constraintCombo->addItems({"stiffness","toughness",
                               "thermal-conductivity","electrical-resistivity"});
    ashbyLayout->addWidget(constraintCombo);

    ashbyLayout->addWidget(new QLabel("Geometry"));
    geometryCombo = new QComboBox();
    geometryCombo->addItems({"bending","tension","plate","torsion"});
    ashbyLayout->addWidget(geometryCombo);

    ashbyLayout->addWidget(new QLabel("Optimise for"));
    QHBoxLayout *optRow = new QHBoxLayout();
    optRow->setSpacing(4);
    minimizeBtn = new QPushButton("Minimize");
    maximizeBtn = new QPushButton("Maximize");
    minimizeBtn->setCheckable(true);
    maximizeBtn->setCheckable(true);
    minimizeBtn->setChecked(true);
    minimizeBtn->setStyleSheet(
        "QPushButton:checked { background:#6F7CFF;"
        " color:white; border-color:#6F7CFF; }");
    maximizeBtn->setStyleSheet(
        "QPushButton:checked { background:#6F7CFF;"
        " color:white; border-color:#6F7CFF; }");
    optRow->addWidget(minimizeBtn);
    optRow->addWidget(maximizeBtn);
    ashbyLayout->addLayout(optRow);

    connect(minimizeBtn, &QPushButton::clicked, this, [this](){
        optimiseMaximize = false;
        minimizeBtn->setChecked(true);
        maximizeBtn->setChecked(false);
    });
    connect(maximizeBtn, &QPushButton::clicked, this, [this](){
        optimiseMaximize = true;
        maximizeBtn->setChecked(true);
        minimizeBtn->setChecked(false);
    });

    sideLayout->addWidget(ashbyGroup);

    // -- hard constraints --
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

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setSpacing(6);
    addConstraintBtn    = new QPushButton("+ Add");
    clearConstraintsBtn = new QPushButton("Clear All");
    btnRow->addWidget(addConstraintBtn);
    btnRow->addWidget(clearConstraintsBtn);
    constraintLayout->addLayout(btnRow);

    constraintLayout->addWidget(new QLabel("Active constraints:"));
    constraintList = new QListWidget();
    constraintList->setMinimumHeight(80);
    constraintList->setMaximumHeight(160);
    constraintList->setToolTip(
        "Select a constraint then click Remove to delete it");
    constraintLayout->addWidget(constraintList);

    QPushButton *removeBtn = new QPushButton("Remove Selected");
    constraintLayout->addWidget(removeBtn);
    connect(removeBtn, &QPushButton::clicked,
            this, &MainWindow::onRemoveConstraintClicked);

    sideLayout->addWidget(constraintGroup);
    sideLayout->addSpacing(12);

    runBtn = new QPushButton("Run Selection");
    runBtn->setFixedHeight(40);
    runBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                        stop:0 #6F7CFF, stop:1 #4FD1C5);
            color: white; font-size: 13px; font-weight: bold;
            border-radius: 6px; border: none;
        }
        QPushButton:hover   { background: #6F7CFF; }
        QPushButton:pressed { background: #4FD1C5; }
    )");
    sideLayout->addWidget(runBtn);
    sideLayout->addStretch();

    sideScroll->setWidget(sidebar);
    root->addWidget(sideScroll);

    // ── MAIN PANEL ────────────────────────────────────────────
    QWidget *mainPanel = new QWidget();
    mainPanel->setStyleSheet("background:#081120;");
    QVBoxLayout *panelLayout = new QVBoxLayout(mainPanel);
    panelLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->setSpacing(0);

    // stats bar
    QWidget *statsBar = new QWidget();
    statsBar->setFixedHeight(52);
    statsBar->setStyleSheet(
        "background:#0a1628; border-bottom:1px solid #1a2a40;");
    QHBoxLayout *statsLayout = new QHBoxLayout(statsBar);
    statsLayout->setContentsMargins(16, 0, 16, 0);
    statsLayout->setSpacing(24);

    totalLabel       = new QLabel("Total: 0");
    filteredLabel    = new QLabel("Filtered: 0");
    topMaterialLabel = new QLabel("Top material: —");
    totalLabel->setStyleSheet("font-size:13px; color:#a0aec0;");
    filteredLabel->setStyleSheet("font-size:13px; color:#a0aec0;");
    topMaterialLabel->setStyleSheet(
        "font-size:13px; font-weight:bold; color:#4FD1C5;");

    statsLayout->addWidget(totalLabel);
    statsLayout->addWidget(filteredLabel);
    statsLayout->addStretch();
    statsLayout->addWidget(topMaterialLabel);
    panelLayout->addWidget(statsBar);

    // hint
    QLabel *hint = new QLabel(
        "  Click any row to view full material properties");
    hint->setStyleSheet(
        "font-size:11px; color:#445566; background:#081120;"
        "border-bottom:1px solid #1a2a40; padding:4px 12px;");
    hint->setFixedHeight(24);
    panelLayout->addWidget(hint);

    // results table
    resultsTable = new QTableWidget();
    resultsTable->setColumnCount(6);
    resultsTable->setHorizontalHeaderLabels(
        {"#","Material","Family","E (GPa)","ρ (kg/m³)","Score"});
    resultsTable->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::Stretch);
    resultsTable->horizontalHeader()->setDefaultSectionSize(90);
    resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultsTable->setAlternatingRowColors(true);
    resultsTable->verticalHeader()->setVisible(false);
    resultsTable->setShowGrid(true);
    resultsTable->setCursor(Qt::PointingHandCursor);
    resultsTable->setStyleSheet(TABLE_STYLE);
    panelLayout->addWidget(resultsTable);
    root->addWidget(mainPanel);

    // signals
    connect(runBtn,              &QPushButton::clicked,
            this, &MainWindow::onRunClicked);
    connect(addConstraintBtn,    &QPushButton::clicked,
            this, &MainWindow::onAddConstraintClicked);
    connect(clearConstraintsBtn, &QPushButton::clicked,
            this, &MainWindow::onClearConstraintsClicked);
    connect(resultsTable,        &QTableWidget::cellClicked,
            this, &MainWindow::onTableRowClicked);

    totalLabel->setText("Total: " + QString::number(allMaterials.size()));
}

void MainWindow::refreshConstraintList()
{
    constraintList->clear();
    for (const auto &c : Hard_Constraints) {
        QString entry = QString::fromStdString(c.property_name)
        + "  " + QString::fromStdString(c.symbol)
            + "  " + QString::number(c.value);
        constraintList->addItem(entry);
    }
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
    refreshConstraintList();
}

void MainWindow::onRemoveConstraintClicked()
{
    int row = constraintList->currentRow();
    if (row < 0 || row >= (int)Hard_Constraints.size()) return;
    Hard_Constraints.erase(Hard_Constraints.begin() + row);
    refreshConstraintList();
}

void MainWindow::onClearConstraintsClicked()
{
    Hard_Constraints.clear();
    Filtered_Materials.clear();
    constraintList->clear();
    resultsTable->setRowCount(0);
    filteredLabel->setText("Filtered: 0");
    topMaterialLabel->setText("Top material: —");
}

void MainWindow::onRunClicked()
{
    if (Hard_Constraints.empty()) {
        QMessageBox::warning(this, "No Constraints",
                             "Add at least one constraint before running.");
        return;
    }

    Filtered_Materials.clear();

    QString family = familyCombo->currentText();
    std::vector<Material> pool;
    if (family == "All") {
        pool = allMaterials;
    } else {
        for (const auto &m : allMaterials)
            if (QString::fromStdString(m.family) == family)
                pool.push_back(m);
    }

    Filter(pool, Hard_Constraints);

    if (Filtered_Materials.empty()) {
        QMessageBox::information(this, "No Results",
                                 "No materials passed the constraints.");
        filteredLabel->setText("Filtered: 0");
        topMaterialLabel->setText("Top material: —");
        resultsTable->setRowCount(0);
        return;
    }

    std::string obj = objectiveCombo->currentText().toStdString();
    std::string geo = geometryCombo->currentText().toStdString();
    std::string con = constraintCombo->currentText().toStdString();

    Selection_Method2(Filtered_Materials, obj, geo, con, optimiseMaximize);

    std::sort(Filtered_Materials.begin(), Filtered_Materials.end(),
              [](const Material &a, const Material &b){
                  return a.performance_score > b.performance_score;
              });

    filteredLabel->setText("Filtered: " +
                           QString::number(Filtered_Materials.size()));
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
        resultsTable->setItem(i, 1,
                              item(QString::fromStdString(m.name), i == 0));
        resultsTable->setItem(i, 2,
                              item(QString::fromStdString(m.family)));
        resultsTable->setItem(i, 3,
                              item(QString::number(m.mech.modulus,  'f', 1)));
        resultsTable->setItem(i, 4,
                              item(QString::number(m.mech.density,  'f', 0)));
        resultsTable->setItem(i, 5,
                              item(QString::number(m.performance_score, 'e', 4)));

        if (i == 0) {
            for (int col = 0; col < 6; col++)
                resultsTable->item(i, col)->setBackground(
                    QColor(0x0d, 0x2a, 0x2a));
        }
    }
}

void MainWindow::onTableRowClicked(int row, int /*column*/)
{
    if (row < 0 || row >= (int)Filtered_Materials.size()) return;
    showMaterialDetail(Filtered_Materials[row]);
}

void MainWindow::showMaterialDetail(const Material &m)
{
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle(QString::fromStdString(m.name) + " — Properties");
    dlg->setWindowIcon(QIcon(":/nadir.ico"));
    dlg->setMinimumWidth(460);
    dlg->setStyleSheet(DIALOG_STYLE);

    QVBoxLayout *mainLayout = new QVBoxLayout(dlg);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    // header
    QLabel *nameLabel = new QLabel(QString::fromStdString(m.name));
    nameLabel->setStyleSheet(
        "font-size:16px; font-weight:bold; color:#6F7CFF;");
    QLabel *famLabel = new QLabel("Family: " +
                                  QString::fromStdString(m.family));
    famLabel->setStyleSheet("font-size:12px; color:#4FD1C5;");
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(famLabel);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color:#1a2a40;");
    mainLayout->addWidget(line);

    // addGroup — key labels are now clearly readable
    auto addGroup = [&](const QString &title,
                        const QList<QPair<QString,QString>> &rows)
    {
        QGroupBox *grp = new QGroupBox(title);
        QFormLayout *form = new QFormLayout(grp);
        form->setSpacing(6);
        form->setHorizontalSpacing(24);
        for (const auto &r : rows) {
            QLabel *key = new QLabel(r.first + ":");
            key->setStyleSheet("color:#cbd5e0;");  // light readable grey
            QLabel *val = new QLabel(r.second);
            val->setStyleSheet("color:#F5F7FA; font-weight:bold;");
            form->addRow(key, val);
        }
        mainLayout->addWidget(grp);
    };

    addGroup("Mechanical", {
                               {"Young's Modulus",  QString::number(m.mech.modulus,        'f',1) + " GPa"},
                               {"Density",          QString::number(m.mech.density,        'f',0) + " kg/m³"},
                               {"Tensile Strength", QString::number(m.mech.tensileStrength,'f',1) + " MPa"},
                               {"Hardness",         QString::number(m.mech.hardness,       'f',2) + " Mohs"},
                               {"Poisson's Ratio",  QString::number(m.mech.p_ratio,        'f',3)}
                           });

    addGroup("Thermal", {
                            {"Conductivity",      QString::number(m.therm.conductivity, 'f',2) + " W/mK"},
                            {"Melting Point",     QString::number(m.therm.meltingpoint, 'f',0) + " °C"},
                            {"Thermal Expansion", QString::number(m.therm.expansion,    'f',2) + " ×10⁻⁶/K"},
                            {"Heat Capacity",     QString::number(m.therm.heatcapacity, 'f',1) + " J/kgK"}
                        });

    addGroup("Electrical", {
                               {"Resistivity",         QString::number(m.elec.resistivity, 'e',3) + " Ω·m"},
                               {"Dielectric Constant", QString::number(m.elec.d_constant,  'f',2)}
                           });

    addGroup("Optical", {
                            {"Refractive Index", QString::number(m.optics.refractive_index,'f',3)},
                            {"Transparency",     QString::fromStdString(m.optics.transparency)}
                        });

    addGroup("Chemical", {
                             {"Corrosion Resistance",
                              QString::fromStdString(m.chem.corrosion_resistance)},
                             {"pH Range",
                              QString::number(m.chem.ph_min,'f',1) + " – " +
                                  QString::number(m.chem.ph_max,'f',1)}
                         });

    QLabel *scoreLabel = new QLabel(
        "Performance Score:  " +
        QString::number(m.performance_score, 'e', 4));
    scoreLabel->setStyleSheet(
        "font-size:13px; font-weight:bold; color:#4FD1C5; padding:8px 0;");
    mainLayout->addWidget(scoreLabel);

    QPushButton *closeBtn = new QPushButton("Close");
    closeBtn->setFixedWidth(100);
    closeBtn->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                        stop:0 #6F7CFF, stop:1 #4FD1C5);
            color: white; font-size: 12px; font-weight: bold;
            border-radius: 4px; border: none; padding: 6px 20px;
        }
        QPushButton:hover { background: #6F7CFF; }
    )");
    connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::accept);

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    btnRow->addWidget(closeBtn);
    mainLayout->addLayout(btnRow);

    dlg->exec();
}
