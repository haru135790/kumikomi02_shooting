#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define field_width 128
#define field_height 64
#define player_point 1
#define enemy_point 2
#define bullet_point 3
#define max_enemy 10
#define max_bullet 20
#define bullet_interval 1.0
#define enemy_interval 2.0

#define OLED_RESET     -1 

#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// common class
class common
{
private:
    /* data */
public:
    int x;
    int y;
};

// player class
class player : public common{
private:
    /* data */
public:
    player(/* args */); // constructor

    // move player
    void up();
    void down();
    void left();
    void right();
};

player::player(/* args */){
    x = field_width / 4;
    y = field_height / 2;
}

void player::up(){
    if (y < field_height-8){
        y++;
    }    
}

void player::down(){
    if (y > 0){
        y--;
    }
}

void player::left(){
    if (x > 0){
        x--;
    }
}

void player::right(){
    if (x < field_width/4){
        x++;
    }
}

// bullet class
class bullet : public common{
private:
    int bullet_spead = 1;
public:
    bullet(int player_x, int player_y);
    ~bullet();

    // move bullet
    void move();
};

bullet::bullet(int player_x, int player_y){
    x = player_x + 3;
    y = player_y + 2;
}

bullet::~bullet(){
}

void bullet::move(){
    x++;
}

// enemy class
class enemy : public common{
private:
    /* data */
public:
    enemy(/* args */);
    ~enemy();

    void move();
};

enemy::enemy(/* args */){
    x=(int)random(field_width/2,field_width);
    y=(int)random(0,field_height);
}

enemy::~enemy(){
}

void enemy::move(){
    x--;
}

// main field class
class field{
private:
    /* data */
public:
    field(/* args */); // constructor
    int data[field_width][field_height]; // field data
    int image[field_width][field_height]; // field image
    void update(player mainplayer, enemy *enemys[], bullet *bullets[]); // update field
    void draw(); // draw field
};
field::field(/* args */){
    for (int i = 0; i < field_width; i++){
        for (int j = 0; j < field_height; j++){
            data[i][j] = 0;
        }
    }
}

void field::update(player mainplayer, enemy *enemys[], bullet *bullets[]){
    for (int i = 0; i < field_width; i++){
        for (int j = 0; j < field_height; j++){
            data[i][j] = 0;
        }
    }

    data[mainplayer.x][mainplayer.y] = 1;

    for (int i = 0; i < max_enemy; i++){
        if (enemys[i] != nullptr){
            data[enemys[i]->x][enemys[i]->y] = 2;
        }
    }

    for (int i = 0; i < max_bullet; i++){
        if (bullets[i] != nullptr){
            data[bullets[i]->x][bullets[i]->y] = 3;
        }
    }
}


void field::draw(){
    // reset image
    for (int i = 0; i < field_width; i++){
        for (int j = 0; j < field_height; j++){
            image[i][j] = 0;
        }
    }

    // draw player, enemy, bullet
    for (int i = 0; i < field_width; i++){
        for (int j = 0; j < field_height; j++){
            if (data[i][j] == 1){
                for (int k = 0; k < player_size; k++){
                    for (int l = 0; l < player_size; l++){
                        image[i+k][j+l] = player_image[l][k];
                    }
                }
            }else if (data[i][j] == 2){
                for (int k = 0; k < enemy_size; k++){
                    for (int l = 0; l < enemy_size; l++){
                        image[i+k][j+l] = enemy_image[k][l];
                    }
                }
            }else if (data[i][j] == 3){
                for (int k = 0; k < bullet_size; k++){
                    for (int l = 0; l < bullet_size; l++){
                        image[i+k][j+l] = bullet_image[k][l];
                    }
                }
            }
        }
    }
}


field mainfield;
player mainplayer;
enemy *enemys[max_enemy] = {nullptr};
bullet *bullets[max_bullet] = {nullptr};
bool stop = false;

void setup(){
    for (int i = 0; i < 6; i++){
        pinMode(i, INPUT_PULLUP);
    }
    randomSeed(analogRead(26));
    Serial.begin(9600);
}

// core0
void loop(){
    mainfield.data[mainplayer.x][mainplayer.y] = 1;

    while (!stop){
        if(digitalRead(1) == LOW){
            mainplayer.up();
        }if(digitalRead(2) == LOW){
            mainplayer.down();
        }if(digitalRead(3) == LOW){
            mainplayer.left();
        }if(digitalRead(4) == LOW){
            mainplayer.right();
        }if(digitalRead(5) == LOW){
            bulletCC();//弾の生成判定
        }

        bulletmove();//弾を動かす
        enemymove();//敵を動かす
        breakcheck();//敵と弾の衝突判定
        poscheck();//プレイヤーと敵の衝突判定
        enemyCC();//敵の生成判定
        deletebullet();//弾の削除判定
        deleteenemy();//敵の削除判定

        mainfield.update(mainplayer, enemys, bullets);

        sleep_ms(1000);

    }
    
}

// core1
void setup1(){
    Wire.setSDA(20);
    Wire.setSCL(21);
    Wire.begin();

    Serial.begin(9600);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    initialDisp();
}

void loop1(){
    display.clearDisplay();

    playerPositionDisp();     //自機の表示

    bulletPositionDisp();     //弾の表示

    enemyPositionDisp();

    display.display();

    if(stop){
        gameOverDisp();
        resultDisp();
    }
}

void bulletCC(){
    // create bullet
    for (int i = 0; i < max_bullet; i++){
        if (bullets[i] == nullptr){
            bullets[i] = new bullet(mainplayer.x, mainplayer.y);
            break;
        }
    }
}
void bulletmove(){
    for (int i = 0; i < max_bullet; i++){
        if (bullets[i] != nullptr){
            bullets[i]->move();
        }
    }
}
void enemymove(){
    for (int i = 0; i < max_enemy; i++){
        if (enemys[i] != nullptr){
            enemys[i]->move();
        }
    }
}
void poscheck(){
    for (int i = 0; i < max_enemy; i++){
        if (enemys[i] != nullptr){
            if ((abs(mainplayer.x - enemys[i]->x) <= 4)&&(abs(mainplayer.y - enemys[i]->y) <= 4)){
                stop = true;
            }
        }
    }
}
void enemyCC(){
    for (int i = 0; i < max_enemy; i++){
        if (enemys[i] == nullptr){
            enemys[i] = new enemy();
            break;
        }
    }
}
void breakcheck(){
    for (int i = 0; i < max_enemy; i++){
        for (int j = 0; j < max_bullet; j++){
            if (enemys[i] != nullptr && bullets[j] != nullptr){
                if (abs(enemys[i]->x - (bullets[j]->x))<=3 && abs(enemys[i]->y - (bullets[j]->y))<=3){
                    delete enemys[i];
                    delete bullets[j];
                    enemys[i] = nullptr;
                    bullets[j] = nullptr;
                }
            }
        }
    }
}
void deletebullet(){
    for (int i = 0; i < max_bullet; i++){
        if (bullets[i] != nullptr){
            if (bullets[i]->x > field_width){
                delete bullets[i];
                bullets[i] = nullptr;
            }
        }
    }
}
void deleteenemy(){
    for (int i = 0; i < max_enemy; i++){
        if (enemys[i] != nullptr){
            if (enemys[i]->x < 0){
                delete enemys[i];
                enemys[i] = nullptr;
            }
        }
    }
}

playerPositionDisp(){
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(mainplayer.x, mainplayer.y);
  display.println(">");
}

bulletPositionDisp(){
    for (int i = 0; i < max_bullet; i++){
        if (bullets[i] != nullptr){
        display.fillCircle(bullets[i]->x, bullets[i]->y, 1, SSD1306_INVERSE);
        }
    }
}

enemyPositionDisp(){
    for (int i = 0; i < max_enemy; i++){
        if (enemys[i] != nullptr){
        display.fillRect(enemys[i]->x, enemys[i]->y, 4, SSD1306_INVERSE);
        }
    }
}

void gameOverDisp(){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(CENTER_WIDTH-20, CENTER_HEIGHT+4);
  display.println("GAME OVER");
  display.display();
  delay(2000);
}
