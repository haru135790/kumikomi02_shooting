#define field_width 122
#define field_height 32
#define field_size field_width * field_height
#define player_point 1
#define enemy_point 2
#define bullet_point 3
#define max_enemy 5
#define max_bullet 10
#define bullet_interval 1.0
#define enemy_interval 2.0


//debug
int digitalread(int n){return n;}
int digitalwrite(int m){return m;}
int random(int a,int b){return a+b;}

// common class
class common
{
private:
    /* data */
public:
    int x;
    int y;
};

// main field class
class field{
private:
    /* data */
public:
    field(/* args */); // constructor
    int data[field_width][field_height]; // field data
    void update(); // update field
    void draw(); // draw field
};
field::field(/* args */){
    for (int i = 0; i < field_width; i++){
        for (int j = 0; j < field_height; j++){
            data[i][j] = 0;
        }
    }
}

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
    if (y < field_height){
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
    y = player_y;
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
    x=random((field_width/4)*3,field_width);
    y=random(0,field_height);
}

enemy::~enemy(){
}

void enemy::move(){
    x--;
}



field mainfield;
player mainplayer;
enemy *enemys[max_enemy];
bullet *bullets[max_bullet];
bool stop = false;

void setup(){

}

// core0
void loop(){
    mainfield.data[mainplayer.x][mainplayer.y] = 1;

    while (1){
        if(digitalread(1) == 1){
            mainplayer.up();
        }if(digitalread(2) == 1){
            mainplayer.down();
        }if(digitalread(3) == 1){
            mainplayer.left();
        }if(digitalread(4) == 1){
            mainplayer.right();
        }if(digitalread(5) == 1){
            bulletCC();//弾の生成判定
        }

        bulletmove();//弾を動かす
        enemymove();//敵を動かす
        breakcheck();//敵と弾の衝突判定
        poscheck();//プレイヤーと敵の衝突判定
        enemyCC();//敵の生成判定
        deletebullet();//弾の削除判定
        deleteenemy();//敵の削除判定

        // mainfield.update();

        sleep_ms(1000);

    }
    
}

// core1
void setup1(){
    Serial.begin(9600);
}

void loop1(){
    Serial.print("+");
    for (int i = 0; i < field_width; i++){
        Serial.print("-");
    }
    Serial.println("+");

    for (int i = 0; i < field_height; i++){
        Serial.print("|");
        for (int j = 0; j < field_width; j++){
            if (mainfield.data[j][i] == 0){
                Serial.print(" ");
            }else if (mainfield.data[j][i] == 1){
                Serial.print("P");
            }else if (mainfield.data[j][i] == 2){
                Serial.print("E");
            }else if (mainfield.data[j][i] == 3){
                Serial.print("B");
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
            if (mainplayer.x == enemys[i]->x && mainplayer.y == enemys[i]->y){
                stop = true;
            }
        }
    }
}
void enemyCC(){
    for (int i = 0; i < max_enemy; i++){
        if (enemys[i] == nullptr){
            enemys[i] = new enemy();
            mainfield.data[enemys[i]->x][enemys[i]->y] = 2;
            break;
        }
    }
}
void breakcheck(){
    for (int i = 0; i < max_enemy; i++){
        for (int j = 0; j < max_bullet; j++){
            if (enemys[i] != nullptr && bullets[j] != nullptr){
                if (enemys[i]->x == bullets[j]->x && enemys[i]->y == bullets[j]->y){
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
