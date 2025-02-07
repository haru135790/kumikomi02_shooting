#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <I2CLiquidCrystal.h>

#define field_width 128
#define field_height 64
#define CENTER_WIDTH field_width/2
#define CENTER_HEIGHT field_height/2
#define player_point 1
#define enemy_point 2
#define bullet_point 3
#define max_enemy 10
#define max_bullet 20
#define bullet_interval 1.0
#define enemy_interval 2.0

#define JoyStickX  26
#define JoyStickY  27
#define Button  18
#define BUZZER  19

#define OLED_RESET     -1 

#define SCREEN_ADDRESS 0x7B
Adafruit_SSD1306 display(field_width, field_height, &Wire, OLED_RESET);

I2CLiquidCrystal lcd(20, (bool)false);

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

field mainfield;
player mainplayer;
enemy *enemys[max_enemy] = {nullptr};
bullet *bullets[max_bullet] = {nullptr};
bool stop = false;

int xValue = analogRead(JoyStickX);
int yValue = analogRead(JoyStickY);

void setup(){
	randomSeed(analogRead(26));
	Serial.begin(9600);

	pinMode(Button,INPUT);
	pinMode(BUZZER,OUTPUT);
	Wire1.setSDA(18);
	Wire1.setSCL(19);
	lcd.setWire(&Wire1);
	lcd.begin(8, 2);
}

// core0
void loop(){
	if(!stop){
    stop = false;
		//プレイヤーの移動
		xValue = analogRead(JoyStickX);
  	yValue = analogRead(JoyStickY);
		if(yValue < 300){
			mainplayer.up();
		}if(yValue > 700){
			mainplayer.down();
		}if(xValue < 300){
			mainplayer.left();
		}if(xValue > 700){
			mainplayer.right();
		}if(digitalRead(Button) == LOW){
			bulletCC();//弾の生成判定
		}
		mainfield.update(mainplayer, enemys, bullets);

		//弾の移動
		bulletmove();//弾を動かす
		breakcheck();//敵と弾の衝突判定
		deletebullet();//弾の削除判定
		mainfield.update(mainplayer, enemys, bullets);

		sleep_ms(250);

		//弾の移動
		bulletmove();//弾を動かす
		breakcheck();//敵と弾の衝突判定
		deletebullet();//弾の削除判定
		mainfield.update(mainplayer, enemys, bullets);

		sleep_ms(250);

		//プレイヤーの移動
		xValue = analogRead(JoyStickX);
  	yValue = analogRead(JoyStickY);
		if(yValue < 300){
			mainplayer.up();
		}if(yValue > 700){
			mainplayer.down();
		}if(xValue < 300){
			mainplayer.left();
		}if(xValue > 700){
			mainplayer.right();
		}if(digitalRead(Button) == LOW){
			bulletCC();//弾の生成判定
		}
		mainfield.update(mainplayer, enemys, bullets);

		//弾の移動
		bulletmove();//弾を動かす
		breakcheck();//敵と弾の衝突判定
		deletebullet();//弾の削除判定
		mainfield.update(mainplayer, enemys, bullets);

		//敵の移動
		enemymove();//敵を動かす
		poscheck();//プレイヤーと敵の衝突判定
		deleteenemy();//敵の削除判定
		enemyCC();//敵の生成判定
		mainfield.update(mainplayer, enemys, bullets);

		sleep_ms(250);

		//弾の移動
		bulletmove();//弾を動かす
		breakcheck();//敵と弾の衝突判定
		deletebullet();//弾の削除判定
		mainfield.update(mainplayer, enemys, bullets);

		sleep_ms(250);

	}else{
    // game reset
    if(digitalRead(Button) == LOW){
      stop = false;
      for(int i = 0; i < max_enemy; i++){
        delete enemys[i];
				enemys[i] = nullptr;
      }
      for(int i = 0; i < max_bullet; i++){
        delete bullets[i];
				bullets[i] = nullptr;
      }
      mainplayer.x = field_width / 4;
      mainplayer.y = field_height / 2;
      
      mainfield.update(mainplayer, enemys, bullets);
    }else{
      sleep_ms(10);
    }
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
  for(;;){
    display.clearDisplay();

    playerPositionDisp();     //自機の表示

    bulletPositionDisp();     //弾の表示

    enemyPositionDisp();

    display.display();

    if(stop == true){
      gameOverDisp();
    }
    while(stop == true);
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
          digitalWrite(BUZZER, HIGH);
					delete enemys[i];
					delete bullets[j];
          delay(1);
          digitalWrite(BUZZER, LOW);
					enemys[i] = nullptr;
					bullets[j] = nullptr;
          delay(1);
          digitalWrite(BUZZER, HIGH);
          delay(1);
          digitalWrite(BUZZER, LOW);
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

void initialDisp(){
	display.clearDisplay();
	display.setTextColor(SSD1306_INVERSE);
	display.setTextSize(1);
	display.setCursor(CENTER_WIDTH-20, CENTER_HEIGHT-4);
	display.println("STG");
	display.display();
	delay(2000);
}

void playerPositionDisp(){
	display.setTextColor(SSD1306_INVERSE);
	display.setTextSize(1);
	display.setCursor(mainplayer.x, mainplayer.y);
	display.println(">");
}

void bulletPositionDisp(){
	for (int i = 0; i < max_bullet; i++){
		if (bullets[i] != nullptr){
		display.fillCircle(bullets[i]->x, bullets[i]->y, 1, SSD1306_INVERSE);
		}
	}
}

void enemyPositionDisp(){
	for (int i = 0; i < max_enemy; i++){
		if (enemys[i] != nullptr){
			display.fillRect(enemys[i]->x, enemys[i]->y, 4, 4, SSD1306_INVERSE);
		}
	}
}

void gameOverDisp(){
  display.clearDisplay();
  display.setTextColor(SSD1306_INVERSE);
  display.setTextSize(1);
  display.setCursor(CENTER_WIDTH-20, CENTER_HEIGHT+4);
  display.println("GAME OVER");
  display.display();
}
