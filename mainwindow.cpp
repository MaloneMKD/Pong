#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Window settings
    this->setWindowTitle("Pong");
    // Layout
    QVBoxLayout *main_layout = new QVBoxLayout();
    QHBoxLayout *menu_layout = new QHBoxLayout();
    QHBoxLayout *game_layout = new QHBoxLayout();
    main_layout->addLayout(menu_layout);
    main_layout->addLayout(game_layout);

    // Buttons
    QPushButton *startButton = new QPushButton("Start");
    menu_layout->addWidget(startButton);
    startButton->setFocusPolicy(Qt::NoFocus);
    startButton->setStyleSheet("height: 30; color: #FFFFFF; background-color: #343434;");
    startButton->setFont(QFont("Segoe UI", 12, QFont::DemiBold));

    QPushButton *stopButton = new QPushButton("Stop");
    menu_layout->addWidget(stopButton);
    stopButton->setFocusPolicy(Qt::NoFocus);
    stopButton->setStyleSheet("height: 30; color: #FFFFFF; background-color: #343434;");
    stopButton->setFont(QFont("Segoe UI", 12, QFont::DemiBold));

    QPushButton *helpButton = new QPushButton("Help");
    menu_layout->addWidget(helpButton);
    helpButton->setFocusPolicy(Qt::NoFocus);
    helpButton->setStyleSheet("height: 30; color: #FFFFFF; background-color: #343434;");
    helpButton->setFont(QFont("Segoe UI", 12, QFont::DemiBold));

    // Set up view and scene
    m_view = new QGraphicsView(this);
    m_scene = new game_scene(QRectF(), m_view, this);
    m_view->setScene(m_scene);
    m_view->setRenderHint(QPainter::Antialiasing);

    game_layout->addWidget(m_view);

    // Connections
    connect(startButton, SIGNAL(clicked(bool)), m_scene, SLOT(start()));
    connect(stopButton, SIGNAL(clicked(bool)), m_scene, SLOT(stop()));
    connect(helpButton, SIGNAL(clicked(bool)), this, SLOT(helpButtonClicked()));

    // Dummy widget
    QWidget *dummy = new QWidget(this);
    dummy->setLayout(main_layout);
    this->setCentralWidget(dummy);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::helpButtonClicked()
{
    m_scene->stop();
    QMessageBox *helpBox = new QMessageBox(this);
    helpBox->setWindowTitle("Help");
    helpBox->setFont(QFont("Segoe UI", 12, QFont::Normal));
    helpBox->addButton(QMessageBox::Close);
    helpBox->setText("Levels:\n"
                     "The computer will adjust it's difficulty level depending on the difference between your scores.\n"
                     "When you and the computer's scores are equal, the computer plays at level 6, then: \n"
                     "        You are 3 points up: level 7\n"
                     "        You are 5 points up: level 8\n"
                     "        You are 8 points up: level 10\n"
                     "        You are 10 points up: level Impossible\n\n"
                     "        Computer is 3 points up: level 5\n"
                     "        Computer is 5 points up: level 4.\n\n"
                     "Moving:\nYou can move your paddle using the arrow keys on the keyboard or using the mouse.\n\n"
                     "Shotcuts:\n"
                     "Start game: press <S>\n"
                     "Restart round: press <R>\n"
                     "Stop game: press <T>"
                     );
    helpBox->setIcon(QMessageBox::Information);
    helpBox->show();
}
