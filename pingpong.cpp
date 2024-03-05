#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <X11/Xutil.h> 
struct Ball {
    int x, y; 
    int xdir, ydir; 
    int size;
};
struct Paddle {
    int x, y; // Position
    int width, height;
};

bool checkCollision(Ball ball, Paddle paddle) {
    return ball.x >= paddle.x &&
           ball.x <= paddle.x + paddle.width &&
           ball.y >= paddle.y &&
           ball.y <= paddle.y + paddle.height;
}

void changeColor(Display* display, Window window);

void moveAutoPaddle(Ball ball, Paddle &paddle) {
    if (ball.y < paddle.y + paddle.height / 2) {
        paddle.y -= 2; // Move up
    } else {
        paddle.y += 2; // Move down
    }
    if (paddle.y < 0) {
        paddle.y = 0;
    }
    if (paddle.y + paddle.height > 300) {
        paddle.y = 300 - paddle.height;
    }
}

int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Error: Unable to open X display" << std::endl;
        return -1;
    }

    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                        10, 10, 500, 300, 1,
                                        BlackPixel(display, screen),
                                        WhitePixel(display, screen));

    XSelectInput(display, window, KeyPressMask);
    XMapWindow(display, window);

    GC gc = XCreateGC(display, window, 0, nullptr);
    XSetForeground(display, gc, BlackPixel(display, screen));

    XEvent event;
    bool running = true;

    // Initialize ball
    Ball ball;
    ball.x = 250;
    ball.y = 150;
    ball.xdir = -1;
    ball.ydir = 1;
    ball.size = 10;

    // Initialize paddles
    Paddle player1, player2;
    player1.x = 10;
    player1.y = 100;
    player1.width = 10;
    player1.height = 60;

    player2.x = 480;
    player2.y = 100;
    player2.width = 10;
    player2.height = 60;

    int player1_point = 0;
    int player2_point = 0;

    // Load font and set attributes for Player Score text
    XFontStruct* fontInfo;
    const char* fontname = "*-helvetica-bold-r-normal--12-*";
    fontInfo = XLoadQueryFont(display, fontname);
    if (!fontInfo) {
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }

    XSetFont(display, gc, fontInfo->fid);

    // Game loop
    while (running) {
        while (XPending(display)) {
            XNextEvent(display, &event);
            if (event.type == KeyPress) {
                KeySym key = XLookupKeysym(&event.xkey, 0);
                if (key == XK_s) {
                    player1.y += 25;
                }
                else if (key == XK_Up) {
                    player2.y -= 25;
                }
                else if (key == XK_Down) {
                    player2.y += 25;
                }
            }
        }

        // Move AI-controlled paddle (Player 1)
        moveAutoPaddle(ball, player1);

        // Move ball
        ball.x += ball.xdir;
        ball.y += ball.ydir;

        // Check collision with walls
        if (ball.y <= 0 || ball.y >= 300 - ball.size) {
            ball.ydir = -ball.ydir;
        }

        // Check collision with paddles
        if (checkCollision(ball, player1) || checkCollision(ball, player2)) {
            ball.xdir = -ball.xdir;
            changeColor(display, window);
        }

        // Check for scoring
        if (ball.x <= 0) {
            player2_point++;
            ball.x = 250;
            ball.y = 150;
            ball.xdir = -ball.xdir;
        }
        else if (ball.x >= 500 - ball.size) {
            player1_point++;
            ball.x = 250;
            ball.y = 150;
            ball.xdir = -ball.xdir;
        }

        // Clear the screen
        XClearWindow(display, window);

        // Draw ball
        XFillArc(display, window, gc, ball.x, ball.y, ball.size, ball.size, 0, 360 * 64);

        // Draw paddles
        XFillRectangle(display, window, gc, player1.x, player1.y, player1.width, player1.height);
        XFillRectangle(display, window, gc, player2.x, player2.y, player2.width, player2.height);

        // Draw score
        std::string player1_score = "Player 1 Score: " + std::to_string(player1_point);
        std::string player2_score = "Player 2 Score: " + std::to_string(player2_point);

        XTextItem player1_score_item, player2_score_item;
        player1_score_item.chars = const_cast<char*>(player1_score.c_str());
        player1_score_item.nchars = player1_score.length();
        player1_score_item.delta = 0;
        player1_score_item.font = fontInfo->fid;

        player2_score_item.chars = const_cast<char*>(player2_score.c_str());
        player2_score_item.nchars = player2_score.length();
        player2_score_item.delta = 0;
        player2_score_item.font = fontInfo->fid;

        int text_y_pos = 20;
        XDrawText(display, window, gc, 10, text_y_pos, &player1_score_item, 1);
        XDrawText(display, window, gc, 385, text_y_pos, &player2_score_item, 1);

        std::string gameover = "GAME OVER";
        if (player1_point == 5) {
            XDrawString(display, window, gc, 225, 110, gameover.c_str(), gameover.length());
            std::string player1_wins = "PLAYER 1 WINS";
            XDrawString(display, window, gc, 215, 130, player1_wins.c_str(), player1_wins.length());

            XFlush(display);
            usleep(5000000);
            running = false;
        }
        if (player2_point == 5) {
            XDrawString(display, window, gc, 225, 110, gameover.c_str(), gameover.length());
            std::string player2_wins = "PLAYER 2 WINS";
            XDrawString(display, window, gc, 215, 130, player2_wins.c_str(), player2_wins.length());

            XFlush(display);
            usleep(5000000);
            running = false;
        }

        XFlush(display);
        usleep(8000); // 10 ms

    }

    XCloseDisplay(display);
    return 0;
}
void changeColor(Display* display, Window window){
    int color1 = 200 + rand() % 56;
    int color2 = 200 + rand() % 56;
    int color3 = 200 + rand() % 56;
     XSetWindowAttributes attr;
     attr.background_pixel = color1 << 16 | color2 << 8 | color3;
     XChangeWindowAttributes(display, window, CWBackPixel, &attr);

     XClearWindow(display, window);
     XFlush(display);
}
