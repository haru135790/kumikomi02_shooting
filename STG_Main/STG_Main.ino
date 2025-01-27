#define field_width 122
#define field_height 32
#define player_point 1
#define enemy_point 2
#define bullet_point 3
#define max_enemy 5
#define max_bullet 10
#define bullet_interval 1.0
#define enemy_interval 2.0

//drawings
#define player_size 7
const int player_image[7][7] = {{0,1,0,0,0,0,0},{0,1,1,0,0,0,0},{1,1,1,1,0,0,0},{0,1,1,1,1,1,0},{1,1,1,1,0,0,0},{0,1,1,0,0,0,0},{0,1,0,0,0,0,0}};
#define bullet_size 3
const int bullet_image[3][3] = {{0,1,0},{1,1,1},{0,1,0}};
#define enemy_size 7
const int enemy_image[7][7] = {{0,0,0,1,0,0,0},{0,0,1,0,1,0,0},{0,1,0,0,0,1,0},{1,0,0,1,0,0,1},{0,1,0,0,0,1,0},{0,0,1,0,1,0,0},{0,0,0,1,0,0,0}};

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

    // fire bullet
    void fire();
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

void player::fire(){
    // create bullet
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
    x=(int)random(60,field_width);
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
        mainfield.draw();

        sleep_ms(1000);

    }
    
}

// core1
void setup1(){
    Serial.begin(9600);
}

void loop1(){
    while(!stop){
        Serial.print("+");
        for (int i = 0; i < field_width; i++){
            Serial.print("-");
        }
        Serial.println("+");

        for (int i = 0; i < field_height; i++){
            Serial.print("|");
            for (int j = 0; j < field_width; j++){
                if (mainfield.image[j][i] == 0){
                    Serial.print(" ");
                }else{
                    Serial.print("･");
                }
            }
            Serial.println("|");
        }

        Serial.print("+");
        for (int i = 0; i < field_width; i++){
            Serial.print("-");
        }
        Serial.println("+");

        delay(1000);    
    }
    Serial.print(" ");
    for (int i = 0; i < field_width; i++){
        Serial.print("-");
    }
    Serial.println(" ");

    for (int i = 0; i < field_height; i++){
        Serial.print("|");
        for (int j = 0; j < field_width; j++){
            if (mainfield.image[j][i] == 0){
                Serial.print(" ");
            }else{
                Serial.print("･");
            }
        }
        Serial.println("|");
    }

    Serial.print(" ");
    for (int i = 0; i < field_width; i++){
        Serial.print("-");
    }
    Serial.println(" ");
    delay(1000);
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
            if ((mainplayer.x >= enemys[i]->x -4 && mainplayer.x <= enemys[i]->x +4)&&(mainplayer.y >= enemys[i]->y-4 && mainplayer.y <= enemys[i]->y+4)){
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
                if (enemys[i]->x <= (bullets[j]->x)-2 && (enemys[i]->y <= bullets[j]->y && (enemys[i]->y)+6 >= bullets[j]->y)){
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
