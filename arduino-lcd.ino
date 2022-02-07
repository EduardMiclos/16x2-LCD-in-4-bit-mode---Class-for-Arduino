#define DISPLAY_TIME 3000 /* ms */

/* LCD with BUS length of 4 bits */
class _4bitLCD {
  private:
  int RS_PIN, RW_PIN, ENABLE_PIN;
  int DATA_PINS[4];

  #define ENABLE_DELAY_TIME 5 /* ms */  

  typedef enum {INSTRUCTION = LOW, DATA = HIGH} REGISTER_TYPE;
  typedef enum {WRITE = LOW, READ = HIGH} OPERATION;

  typedef enum {
    CLEAR_SCREEN=0x01,
    CONFIGURE=0x28, /* 2-line, 4 bit mode, 5x8 dots */
    CRS_HOME=0x02, /* returns cursor to home position */
    CRS_INC=0x14, /* increment cursor */
    CRS_DEC=0x10 /* decrement cursor */
  } INSTRUCTIONS;

  REGISTER_TYPE regType;
  OPERATION operation;

  typedef struct{
    int x, y;
  } CURSOR_POS;

  CURSOR_POS cursor;

  /* Constructor. */
  public:
  _4bitLCD(int rs, int rw, int en, int data[])
    : RS_PIN(rs),
      RW_PIN(rw),
      ENABLE_PIN(en)
    {
      int i;
      for(i = 0; i < 4; i++)
       this->DATA_PINS[i] = data[i];

       this->cursor = {0, 0};
    }  

    /* Methods */

    /* Private Methods. */
    private:
    void enableData(void){
        digitalWrite(this->ENABLE_PIN, HIGH);
        delay(ENABLE_DELAY_TIME);
        digitalWrite(this->ENABLE_PIN, LOW);
        delay(ENABLE_DELAY_TIME);
    }

    void setDataMode(void){
      this->regType = DATA;
      digitalWrite(this->RS_PIN, DATA);
    }

    void setInstructionMode(void){
      this->regType = INSTRUCTION;
      digitalWrite(this->RS_PIN, INSTRUCTION);
    }

    void setWriteMode(void){
      this->operation = WRITE;
      digitalWrite(this->RW_PIN, WRITE);
    }

    void _8bitWrite(char c){
        int i;
        
        for(i = 0; i <= 3; i++)
          digitalWrite(this->DATA_PINS[3 - i], (c >> (7 - i)) & 1);

        this->enableData();

        for(i = 3; i >= 0; i--)
          digitalWrite(this->DATA_PINS[i], (c >> i) & 1);

        this->enableData();
    }

    void _8bitWriteData(String  str){
      if(this->cursor.y == 16){
        if(this->cursor.x)
          return;

        this->moveCursor(1, 0); 
      }

      if(regType == INSTRUCTION)
        this->setDataMode();

      if(this->operation == READ)
        this->setWriteMode();

     for(char c : str){
        this->_8bitWrite(c);

        /* Incrementing the cursor's y coordinate and checking for limits. */
        if(!this->incrementCursor())
          return;

        this->setDataMode();
      }
    }

    void _8bitWriteInstruction(char c){

      if(this->regType == DATA)
        this->setInstructionMode();
      
      if(this->operation == READ)
        this->setWriteMode();

      this->_8bitWrite(c);
    }
 
    int incrementCursor(void){
      this->cursor.y++;
      //this->_8bitWriteInstruction(CRS_INC);

      if(this->cursor.y == 16){
        if(this->cursor.x)
          return 0;
        
        this->cursor.x = 1;
        this->cursor.y = 0;
      }

      return 1;
    }

    void decrementCursor(void){
      this->cursor.y--;
      this->_8bitWriteInstruction(CRS_DEC);
    }

    void setPinsMode(void){
      pinMode(this->RS_PIN, OUTPUT);
      pinMode(this->RW_PIN, OUTPUT);
      pinMode(this->ENABLE_PIN, OUTPUT);

      int i;
      for(i = 0; i < 4; i++)
        pinMode(this->DATA_PINS[i], OUTPUT);
    }

    void configure(void){
      this->setPinsMode();

      this->setInstructionMode();
      this->setWriteMode();
      
      this->_8bitWriteInstruction(CLEAR_SCREEN);
      this->_8bitWriteInstruction(CRS_HOME);
      this->_8bitWriteInstruction(CONFIGURE);
      this->_8bitWriteInstruction(CLEAR_SCREEN);
    }


    /* Public Methods. */
    public:
    void begin(void){
      this->configure();      
    }

    void print(String str){
      this->_8bitWriteData(str);
    }

    void clear(){
      _8bitWriteInstruction(CLEAR_SCREEN);
    }

    void moveCursor(int x, int y){
      if(x < 0 || x > 1 || y < 0 || y > 15) return;

      int dist;
      INSTRUCTIONS instr;

      if(this->cursor.x != x){
        dist = 24;

        if(this->cursor.x < x)
          dist += 15 - this->cursor.y + y + 1;
        else
          dist += 15 - y + this->cursor.y + 1;
      }
      else
          dist = abs(this->cursor.y - y);

      if((this->cursor.y < y && this->cursor.x == x) || this->cursor.x < x)
        instr = CRS_INC;
      else
        instr = CRS_DEC;

        while(dist--)
         this->_8bitWriteInstruction(instr);
      
        this->cursor.x = x;
        this->cursor.y = y;
    }
};

int RS_PIN = 2; /* Register select. */
int RW_PIN = 4; /* Read/Write pin. */
int ENABLE_PIN = 7; /* Enable pin. */

int DATA_PINS[] = {8, 9, 10, 11}; /* We unly use 4 bits for data transmission. */

_4bitLCD lcd(RS_PIN, RW_PIN, ENABLE_PIN, DATA_PINS);

void setup(){
  lcd.begin();
}

void loop() {
  delay(20);
  lcd.moveCursor(1, 5);
  delay(3000);
  lcd.print("123");
  delay(4000);
  lcd.moveCursor(0, 9);
  lcd.print("123");
  delay(3000);
}