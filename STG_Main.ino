#define field_width 122
#define field_height 32
#define field_size field_width * field_height
#define player_point 1
#define enemy_point 2
#define bullet_point 3
#define max_enemy 10
#define max_bullet 10
#define bullet_interval 1.0
#define enemy_interval 2.0


//debug
int digitalread(int n){return n;}
int digitalwrite(int m){return m;}

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

    // fire bullet
    void fire();
};

player::player(/* args */){
    x = field_width / 4;
    y = field_height / 2;
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

void bulletCC(){}
void bulletmove(){}
void enemymove(){}
void poscheck(){}
void enemyCC(){}
void breakcheck(){}

bool stop = false;
void main(){
    field field;
    player player;
    enemy *enemys[8];
    bullet *bullets[10];

    field.data[player.x][player.y] = 1;

    while (stop){
        if(digitalread(1) == 1){
            player.x++;
        }if(digitalread(2) == 1){
            player.x--;
        }if(digitalread(3) == 1){
            player.y++;
        }if(digitalread(4) == 1){
            player.y++;
        }if(digitalread(5) == 1){
            bulletCC();
        }

        bulletmove();
        enemymove();
        enemyCC();
        poscheck();
        breakcheck();

    }
    
}

