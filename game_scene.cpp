#include "game_scene.h"
#include <QDebug>
#include <QGraphicsItem>
#include <math.h>
#include <random>
#include <time.h>
#include <QList>

game_scene::game_scene(QRectF rect, QGraphicsView *view, QObject *parent): QGraphicsScene(rect, parent)
{
    // Game Boundary
    m_gameBoundary = new QGraphicsRectItem(QRectF(0, 0, 1480, 650));
    this->addItem(m_gameBoundary);

    // Initialize other variables
    m_p1Speed = 0;
    m_p2Speed = 8;
    m_p2Level = 6;
    m_p1Score = 0;
    m_p2Score = 0;
    m_view = view;
    m_scoreRect = new QGraphicsRectItem(QRectF(0, 0, 15, 650));
    QLinearGradient grad = QLinearGradient(0, 650, 7, 650);
    grad.setColorAt(0, Qt::red);
    grad.setColorAt(0.8, Qt::white);
    m_scoreRect->setBrush(QBrush(grad));
    m_scoreRect->setPen(Qt::NoPen);
    m_scoreRect->setZValue(-1);

    // Choose a random x and y speed and horizontal direction for the ball
    srand(time(0));
    double rand_val = (1 + rand() % 10) / 10.0;
    QList<int> headsORtails = {1, -1};
    m_ballSpeedX = 10 * cos(rand_val) * headsORtails[rand() % 2];
    m_ballSpeedY = 10 * -sin(rand_val);

    // Set up players
    m_player1 = new QGraphicsRectItem(QRectF(0, 0, PLAYER_WIDTH, PLAYER_HEIGHT));
    m_player1->setPos(5, m_gameBoundary->sceneBoundingRect().center().y() - (PLAYER_HEIGHT / 2.0));
    m_player1->setBrush(QColor("#343434"));
    m_player1->setPen(QColor("#343434"));

    m_player2 = new QGraphicsRectItem(QRectF(0, 0, PLAYER_WIDTH, PLAYER_HEIGHT));
    m_player2->setPos(m_gameBoundary->sceneBoundingRect().width() - (PLAYER_WIDTH + 5),
                      m_gameBoundary->sceneBoundingRect().center().y() - (PLAYER_HEIGHT / 2.0));
    m_player2->setBrush(QColor("#343434"));
    m_player2->setPen(QColor("#343434"));

    /*this->addLine(0, m_gameBoundary->sceneBoundingRect().height()/2.0,
                  m_gameBoundary->sceneBoundingRect().width(), (m_gameBoundary->sceneBoundingRect().height())/2.0);*/
    this->addLine(m_gameBoundary->sceneBoundingRect().width()/2.0, 0,
                  m_gameBoundary->sceneBoundingRect().width()/2.0, m_gameBoundary->sceneBoundingRect().height(),
                  QPen(QColor("#343434")));

    // Set up ball
    m_ball = new QGraphicsEllipseItem(QRectF(0, 0, BALL_SIZE, BALL_SIZE));
    m_ball->setPos(m_gameBoundary->sceneBoundingRect().center().x() - (BALL_SIZE / 2.0),
                   m_gameBoundary->sceneBoundingRect().center().y() - (BALL_SIZE / 2.0));
    m_ball->setBrush(QColor("#343434"));
    m_ball->setPen(QColor("#343434"));

    // Add items to the scene
    this->addItem(m_player1);
    this->addItem(m_player2);
    this->addItem(m_ball);
    this->addRect(this->sceneRect(), QPen(QColor("#343434")));

    //Add scores
    m_p1ScoreText = new QGraphicsTextItem(QString("Score: %1") .arg(m_p1Score));
    m_p1ScoreText->setFont(QFont("Segoe UI", 16, QFont::Medium));
    m_p1ScoreText->setPos(300, 5);
    this->addItem(m_p1ScoreText);

    m_p2ScoreText = new QGraphicsTextItem(QString("Score: %1") .arg(m_p2Score));
    m_p2ScoreText->setFont(QFont("Segoe UI", 16, QFont::Medium));
    m_p2ScoreText->setPos(m_gameBoundary->sceneBoundingRect().center().x() + 300, 5);
    this->addItem(m_p2ScoreText);

    m_levelText = new QGraphicsTextItem(QString("Level: %1") .arg(m_p2Level));
    m_levelText->setFont(QFont("Segoe UI", 16, QFont::Medium));
    m_levelText->setPos(m_gameBoundary->sceneBoundingRect().center().x(), 5);
    this->addItem(m_levelText);

    // Set up game timer
    m_gameTimer = new QTimer(this);
    m_gameTimer->setInterval(5);
    connect(m_gameTimer, SIGNAL(timeout()), this, SLOT(updateObjects()));

    // Set up pause timer
    m_pauseTimer = new QTimer(this);
    m_pauseTimer->setInterval(500);
    connect(m_pauseTimer, SIGNAL(timeout()), m_gameTimer, SLOT(start()));

    // Set up score pause timer
    m_scoreTimer = new QTimer(this);
    m_scoreTimer->setInterval(500);
    connect(m_scoreTimer, SIGNAL(timeout()), this, SLOT(scored()));
}

void game_scene::reset()
{
    // Remove score rect if its there
    if(this->items().contains(m_scoreRect))
        this->removeItem(m_scoreRect);

    // Reset all object positions
    m_player1->setPos(5, m_gameBoundary->sceneBoundingRect().center().y() - (PLAYER_HEIGHT / 2.0));
    m_player2->setPos(m_gameBoundary->sceneBoundingRect().width() - (PLAYER_WIDTH + 5),
                      m_gameBoundary->sceneBoundingRect().center().y() - (PLAYER_HEIGHT / 2.0));
    m_ball->setPos(m_gameBoundary->sceneBoundingRect().center().x() - (BALL_SIZE / 2.0),
                   m_gameBoundary->sceneBoundingRect().center().y() - (BALL_SIZE / 2.0));

    // Choose a random x and y speed and horizontal direction for the ball
    QList<int> headsORtails = {1, -1};
    m_ballSpeedX = (5 + 10 * (rand() % 10)) / 10.0 * headsORtails[rand() % 2];;
    m_ballSpeedY = (5 + 10 * (rand() % 10)) / 10.0;
}

void game_scene::restart()
{
    reset();
    if(m_gameTimer->isActive())
        m_gameTimer->stop();
    if(m_scoreTimer->isActive())
        m_scoreTimer->stop();
    m_pauseTimer->start();
}

void game_scene::updateObjects()
{
    if(m_pauseTimer->isActive())
        m_pauseTimer->stop();

    // Update player 1 position
    m_player1->setPos(m_player1->scenePos().x(), m_player1->scenePos().y() + m_p1Speed);
    if(m_player1->scenePos().y() <= m_player1->mapFromScene(0, 0).y())
        m_player1->setPos(m_player1->scenePos().x(), 0);
    else if(m_player1->scenePos().y() >= m_gameBoundary->sceneBoundingRect().height() - PLAYER_HEIGHT)
        m_player1->setPos(m_player1->scenePos().x(), m_gameBoundary->sceneBoundingRect().height() - PLAYER_HEIGHT);

    // Update player 2 position
        // If ball is headed towards player2(computer player), start moving player 2
    if(m_ballSpeedX > 0)
    {
        QLineF line1 = QLineF(m_ball->sceneBoundingRect().center(), m_player2->sceneBoundingRect().center());
        QLineF line2 = QLineF(m_ball->sceneBoundingRect().center().x(), m_ball->sceneBoundingRect().center().y(),
                              m_player2->sceneBoundingRect().center().x(), m_ball->sceneBoundingRect().center().y());

        double angle = line1.angleTo(line2);
        if(angle > 10 &&  angle < 180)
            m_p2Speed = -m_p2Level;
        else if(angle > 180 && angle < 350)
            m_p2Speed = m_p2Level;
        else
            m_p2Speed = 0;

        m_player2->setPos(m_player2->scenePos().x(), m_player2->scenePos().y() + m_p2Speed);
    }
    else if(m_ballSpeedX < 0 && m_p2Level == 100) // If moving towards p1 and level == 100
    {
        QLineF line1 = QLineF(m_ball->sceneBoundingRect().center(), m_player2->sceneBoundingRect().center());
        QLineF line2 = QLineF(m_ball->sceneBoundingRect().center().x(), m_ball->sceneBoundingRect().center().y(),
                              m_player2->sceneBoundingRect().center().x(), m_ball->sceneBoundingRect().center().y());

        double angle = line1.angleTo(line2);
        if(angle > 10 &&  angle < 180)
            m_p2Speed = -m_p2Level;
        else if(angle > 180 && angle < 350)
            m_p2Speed = m_p2Level;
        else
            m_p2Speed = 0;

        m_player2->setPos(m_player2->scenePos().x(), m_player2->scenePos().y() + m_p2Speed);
    }

    if(m_player2->scenePos().y() <= m_player2->mapFromScene(0, 0).y())
        m_player2->setPos(m_player2->scenePos().x(), 0);
    else if(m_player2->scenePos().y() >= m_gameBoundary->sceneBoundingRect().height() - PLAYER_HEIGHT)
        m_player2->setPos(m_player2->scenePos().x(), m_gameBoundary->sceneBoundingRect().height() - PLAYER_HEIGHT);

    // Update ball position
    m_ball->setPos(m_ball->scenePos().x() + m_ballSpeedX, m_ball->scenePos().y() + m_ballSpeedY);

    // If ball collides with any of the four sides
    if(m_ball->scenePos().x() <= 0) // Left boundary
    {
        m_ball->setPos(0, m_ball->scenePos().y());
        m_ballSpeedX *= -1;

        // Update score
        m_p2Score++;
        m_p2ScoreText->setPlainText(QString("Score: %1") .arg(m_p2Score));
        adjustLevel();

        // Stop game and show score
        m_scoreRect->setPos(0, 0);
        QLinearGradient grad = QLinearGradient(0, 650, 7, 650);
        grad.setColorAt(0, Qt::red);
        grad.setColorAt(0.8, Qt::white);
        m_scoreRect->setBrush(QBrush(grad));
        this->addItem(m_scoreRect);
        m_gameTimer->stop();
        m_scoreTimer->start();
    }
    else if(m_ball->scenePos().x() >= m_gameBoundary->sceneBoundingRect().width() - BALL_SIZE) // Right boundary
    {
        m_ball->setPos(m_gameBoundary->sceneBoundingRect().width() - BALL_SIZE, m_ball->scenePos().y());
        m_ballSpeedX *= -1;

        // Update score
        m_p1Score++;
        m_p1ScoreText->setPlainText(QString("Score: %1") .arg(m_p1Score));
        adjustLevel();

        // Stop game and show score
        m_scoreRect->setPos(1475, 0);
        QLinearGradient grad = QLinearGradient(0, 650, 7, 650);
        grad.setColorAt(0.8, Qt::white);
        grad.setColorAt(0, Qt::red);
        m_scoreRect->setBrush(QBrush(grad));
        this->addItem(m_scoreRect);
        m_gameTimer->stop();
        m_scoreTimer->start();
    }
    else if(m_ball->scenePos().y() <= 0) // Top boundary
    {
        m_ball->setPos(m_ball->scenePos().x(), 0);
        m_ballSpeedY *= -1;
    }
    else if(m_ball->scenePos().y() >= m_gameBoundary->sceneBoundingRect().height() - BALL_SIZE) // Bottom boundary
    {
        m_ball->setPos(m_ball->scenePos().x(), m_gameBoundary->sceneBoundingRect().height() - BALL_SIZE);
        m_ballSpeedY *= -1;
    }

    // If  call collides with any of the players
    if(m_ball->collidesWithItem(m_player1))
    {
        // If the collision is detected when the ball is past the player, move it back that amount
        double dist = (m_player1->scenePos().x() + PLAYER_WIDTH) - m_ball->scenePos().x();
        if(dist > 0)
            m_ball->setPos(1 + m_ball->scenePos().x() + dist, m_ball->scenePos().y());

        // Get the ball's position relative to the player's height and normalize it and use it to calculate the angle of reflection
        double hit_pos = (m_player1->scenePos().y() + PLAYER_HEIGHT / 2.0) - (m_ball->scenePos().y() + BALL_SIZE/2.0);
        double normalized_hit_pos = hit_pos / (PLAYER_HEIGHT / 2.0);
        m_ballSpeedX = 10 * cos(normalized_hit_pos);
        m_ballSpeedY = 10 * -sin(normalized_hit_pos);
    }
    else if(m_ball->collidesWithItem(m_player2))
    {
        // If the collision is detected when the ball is past the player, move it back that amount
        double dist = ((m_ball->scenePos().x() + BALL_SIZE) - m_player2->scenePos().x());
        if(dist > 0)
            m_ball->setPos(-1 + m_ball->scenePos().x() - dist, m_ball->scenePos().y());

        // Get the ball's position relative to the player's height and normalize it and use it to calculate the angle of reflection
        double hit_pos = (m_player2->scenePos().y() + PLAYER_HEIGHT / 2.0) - (m_ball->scenePos().y() + BALL_SIZE/2.0);
        double normalized_hit_pos = hit_pos / (PLAYER_HEIGHT / 2.0);
        m_ballSpeedX = 10 * -cos(normalized_hit_pos);
        m_ballSpeedY = 10 * -sin(normalized_hit_pos);
    }

    this->update();
    m_view->update();
}

void game_scene::adjustLevel()
{
    switch(m_p1Score - m_p2Score)
    {
    case -5:
        m_p2Level = 4;
        m_levelText->setPlainText(QString("Level %1") .arg(m_p2Level));
        break;
    case -3:
        m_p2Level = 5;
        m_levelText->setPlainText(QString("Level %1") .arg(m_p2Level));
        break;
    case 0:
        m_p2Level = 6;
        m_levelText->setPlainText(QString("Level %1") .arg(m_p2Level));
        break;
    case 3:
        m_p2Level = 7;
        m_levelText->setPlainText(QString("Level %1") .arg(m_p2Level));
        break;
    case 5:
        m_p2Level = 8;
        m_levelText->setPlainText(QString("Level %1") .arg(m_p2Level));
        break;
    case 8:
        m_p2Level = 10;
        m_levelText->setPlainText(QString("Level %1") .arg(m_p2Level));
        break;
    case 10:
        m_p2Level = 100;
        m_levelText->setPlainText(QString("Level Impossible"));
        break;
    }
}

void game_scene::start()
{
    if(!m_gameTimer->isActive())
    {
        reset();
        m_gameTimer->start();
    }
}

void game_scene::stop()
{
    // Pause all timers
    if(m_gameTimer->isActive())
        m_gameTimer->stop();
    if(m_pauseTimer->isActive())
        m_pauseTimer->stop();
    m_p1Score = 0;
    m_p2Score = 0;
    m_p1ScoreText->setPlainText(QString("Score: %1") .arg(m_p1Score));
    m_p2ScoreText->setPlainText(QString("Score: %1") .arg(m_p2Score));
    reset();
}

void game_scene::scored()
{
    if(m_gameTimer->isActive())
        m_gameTimer->stop();
    this->removeItem(m_scoreRect);
    restart();
}

void game_scene::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up)
        m_p1Speed -= 10;
    else if(event->key() == Qt::Key_Down)
        m_p1Speed += 10;
    else if(event->key() == Qt::Key_R)
        restart();
    else if(event->key() == Qt::Key_T)
        stop();
    else if(event->key() == Qt::Key_S)
        start();
}

void game_scene::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up)
        m_p1Speed = 0;
    else if(event->key() == Qt::Key_Down)
        m_p1Speed = 0;
}

void game_scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    //Q_UNUSED(event)
    // Remove comment to enable movement with the mouse
    QPointF pos = event->scenePos();
    if(pos.y() < (m_gameBoundary->sceneBoundingRect().height() - PLAYER_HEIGHT/2.0) && pos.y() >= PLAYER_HEIGHT/2.0 && m_gameTimer->isActive())
        m_player1->setPos(m_player1->scenePos().x(), (event->scenePos().y() - PLAYER_HEIGHT/2.0));
}
