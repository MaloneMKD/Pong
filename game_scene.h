#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QtGui>
#include <QtCore>
#include <QTimer>
#include <QGraphicsSceneMouseEvent>

class game_scene: public QGraphicsScene
{
    Q_OBJECT

public:
    game_scene(QRectF rect, QGraphicsView *view, QObject *parent);

    void reset();
    void restart();

public slots:
    void updateObjects();
    void adjustLevel();
    void start();
    void stop();
    void scored();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    QGraphicsView *m_view;
    QGraphicsRectItem *m_player1;
    QGraphicsRectItem *m_player2;
    QGraphicsRectItem *m_gameBoundary;
    QGraphicsRectItem *m_scoreRect;
    QGraphicsEllipseItem *m_ball;
    QGraphicsTextItem *m_p1ScoreText;
    QGraphicsTextItem *m_p2ScoreText;
    QGraphicsTextItem *m_levelText;
    QTimer *m_gameTimer;
    QTimer *m_pauseTimer;
    QTimer *m_scoreTimer;
    QTimer *m_playTimer;
    QTime m_startTime;
    QTime m_endTime;
    QRectF m_screenRect;
    int m_p1Score;
    int m_p2Score;
    double m_p1Speed;
    double m_p2Speed;
    double m_p2Level;
    double m_ballSpeedX;
    double m_ballSpeedY;
    const int PLAYER_WIDTH = 20;
    const int PLAYER_HEIGHT = 140;
    const int BALL_SIZE = 20;
    const int SCREEN_WIDTH_OFFSET = 300;
    const int SCREEN_HEIGHT_OFFSET = 100;
};

#endif // GAME_SCENE_H
