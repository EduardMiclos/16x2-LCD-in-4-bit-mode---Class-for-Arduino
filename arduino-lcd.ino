/* Time is measured in miliseconds (ms). */
#define ENABLE_DELAY_TIME 5

/* The number of pins used for data transfer. */
#define DATA_PINS_NUM 4

/* Logical values. */
#define TRUE 1
#define FALSE (!TRUE)

/* Size of char. */
#define SIZE_OF_CHAR sizeof(char)

/* Dimensions. */
#define ROW_DIM 16
#define COL_DIM 2
#define TOTAL_ENTRIES_ROW 40

typedef enum
{
  False = FALSE,
  True = TRUE
} Bool;

/* 16x2 LCD. */
typedef enum
{
  LCD_ROW_MIN = 0,
  LCD_ROW_MAX = (COL_DIM - 1),
  LCD_COL_MIN = 0,
  LCD_COL_MAX = (ROW_DIM - 1)
} _4bitLCD_DISPLAY_DIMENSIONS;

typedef enum 
{
  INSTRUCTION = LOW, 
  DATA = HIGH
} _4bitLCD_REGISTER_TYPE;

typedef enum 
{
  WRITE = LOW, 
  READ = HIGH
} _4bitLCD_OPERATION;

/* The values are taken from the LCD instruction table. */
typedef enum 
{
  CLEAR_SCREEN = 0x01, /* clears the display */
  CONFIGURE = 0x28, /* 2-line, 4 bit mode, 5x8 dots */
  CRS_HOME = 0x02, /* returns cursor to home position */
  CRS_INC = 0x14, /* increment cursor */
  CRS_DEC = 0x10 /* decrement cursor */
} INSTRUCTIONS;

typedef struct
{
  unsigned int uiRowIdx, uiColIdx;
} _4bitLCD_CURSOR_POS;

/* LCD with BUS length of 4 bits */
class _4bitLCD 
{
  private:
  unsigned int uiRS_PIN, uiRW_PIN, uiENABLE_PIN;
  unsigned int arruiDATA_PINS[DATA_PINS_NUM];

  _4bitLCD_REGISTER_TYPE regType;
  _4bitLCD_OPERATION operation;
  _4bitLCD_CURSOR_POS cursor;

  /* Constructor. */
  public:
  _4bitLCD(unsigned int uiRs, unsigned int uiRw, unsigned int uiEn, unsigned int arruiData[])
    : uiRS_PIN(uiRs),
      uiRW_PIN(uiRw),
      uiENABLE_PIN(uiEn)
    {
      int iItr;
      for(iItr = 0; iItr < DATA_PINS_NUM; iItr = iItr + 1)
      {
       this->arruiDATA_PINS[iItr] = arruiData[iItr];
      }
       
      this->cursor = {LCD_ROW_MIN, LCD_COL_MIN};
    }  

    /* === Methods === */
    /* Private Methods. */
    private:

    /* Data transfer enabled iff ENABLE_PIN goes from HIGH to LOW. */
    Bool enableData(void)
    {
      digitalWrite(this->uiENABLE_PIN, HIGH);
      delay(ENABLE_DELAY_TIME);
      digitalWrite(this->uiENABLE_PIN, LOW);
      delay(ENABLE_DELAY_TIME);

      return True;
    }

    /* Sets data mode on. Now the LCD receives DATA. */
    Bool setDataMode(void)
    {
      this->regType = DATA;
      digitalWrite(this->uiRS_PIN, DATA);

      return True;
    }

    /* Sets instruction mode on. Now the LCD receives INSTRUCTIONS. */
    Bool setInstructionMode(void)
    {
      this->regType = INSTRUCTION;
      digitalWrite(this->uiRS_PIN, INSTRUCTION);

      return True;
    }

    /* Sets write mode on. Now we can write DATA or INSTRUCTIONS to LCD, depending on RS pin. */
    Bool setWriteMode(void)
    {
      this->operation = WRITE;
      digitalWrite(this->uiRW_PIN, WRITE);

      return True;
    }

    /* TBD: Validate char input. */
    /* Writes an 8 bit value to the LCD, using the nibble method (4 bit LCD). */
    Bool _8bitWriteChar(char c)
    {
      int iItr;
      unsigned int uiCharSize = SIZE_OF_CHAR*8;
      
      for(iItr = 0; iItr < DATA_PINS_NUM; iItr = iItr + 1)
      {
        digitalWrite(this->arruiDATA_PINS[DATA_PINS_NUM - 1 - iItr], ((c) >> (uiCharSize - 1 - iItr)) & (1));
      }

      Bool enableData_ReturnValue;
      enableData_ReturnValue = this->enableData();
      if(enableData_ReturnValue != True)
      {
        return False;
      }
      else
      {
        for(iItr = DATA_PINS_NUM - 1; iItr >= 0; iItr = iItr - 1)
        {
          digitalWrite(this->arruiDATA_PINS[iItr], (c >> iItr) & 1);
        }

        enableData_ReturnValue = this->enableData();
        if(enableData_ReturnValue != True)
        {
          return False;
        }
        else
        {
          return True;
        }
      }

      return True;
    }

    /* TBD: Validate string input. */
    /* TBD: returnValue shall always contains new information about current state of the function (bit-wise). */
    int _8bitWriteString(String strOutputText)
    {
      int returnValue = 1; // to be modified.

      if(this->cursor.uiColIdx == LCD_COL_MAX)
      {
        if(this->cursor.uiRowIdx != LCD_ROW_MAX)
        { 
          this->moveCursor(this->cursor.uiRowIdx + 1, LCD_COL_MIN);
        }
        else
        {
          returnValue = 0;
          return returnValue;
        } 
      }
      else
      {
          /* TBE. */
      }

      if(regType != DATA)
      {
        Bool setDataMode_ReturnValue = this->setDataMode();
        if(setDataMode_ReturnValue != True)
        {
          return False;
        }
        else
        {
          /* TBE. */
        }
      }
      else
      {
        /* TBE */
      }

      if(this->operation != WRITE)
      {
        Bool setWriteMode_ReturnValue = this->setWriteMode();
        if(setWriteMode_ReturnValue != True)
        {
          return False;
        }
        else
        {
          /* TBE */
        }
      }
      else
      {
        /* TBE */
      }

     for(char c : strOutputText)
     {
       Bool _8bitWrite_ReturnValue;

       _8bitWrite_ReturnValue = this->_8bitWriteChar(c);

       if(_8bitWrite_ReturnValue != True)
       {
         return False;
       }
       else
       {
        /* Incrementing the cursor's uiColIdx coordinate and checking for limits. */
        Bool incrementCursor_ReturnValue = this->incrementCursor();
        if(incrementCursor_ReturnValue != True)
        {
          return False;
        }
        else
        {
          this->setDataMode();
        }
       }
      }
      return returnValue;
    }

    Bool _8bitWriteInstruction(char c)
    {
      if(this->regType != INSTRUCTION)
      {
        Bool setInstructionMode_ReturnValue = this->setInstructionMode();
        if(setInstructionMode_ReturnValue != True)
        {
          return False;
        }
      }
      
      if(this->operation != WRITE)
      {
        Bool setWriteMode_ReturnValue = this->setWriteMode();

        if(setWriteMode_ReturnValue != True)
        {
          return False;
        }
      }

      Bool _8bitWriteChar_ReturnValue = this->_8bitWriteChar(c);
      if(_8bitWriteChar_ReturnValue != True)
      {
        return False;
      }

      return True;
    }
 
    Bool incrementCursor(void)
    { 
      if(this->cursor.uiColIdx == LCD_COL_MAX)
      {
        if(this->cursor.uiRowIdx != LCD_ROW_MAX)
        {
          this->moveCursor(this->cursor.uiRowIdx + 1, LCD_COL_MIN);
        }
        else
        {
          /* TBE. */
        }    
      }
      else
      {
        this->moveCursor(this->cursor.uiRowIdx, this->cursor.uiColIdx + 1);
        /* TBE. */
      }

      return True;
    }

    void decrementCursor(void)
    {      
      if(this->cursor.uiColIdx == LCD_COL_MIN)
      {
        if(this->cursor.uiRowIdx != LCD_ROW_MIN)
        {
        this->moveCursor(this->cursor.uiRowIdx - 1, LCD_COL_MIN);
        }
        else
        {
          /* TBE. */
        }    
      }
      else
      {
        this->moveCursor(this->cursor.uiRowIdx, this->cursor.uiColIdx - 1);
        /* TBE. */
      }
    }

    Bool setPinsMode(void)
    {
      pinMode(this->uiRS_PIN, OUTPUT);
      pinMode(this->uiRW_PIN, OUTPUT);
      pinMode(this->uiENABLE_PIN, OUTPUT);

      int iItr;
      for(iItr = 0; iItr < DATA_PINS_NUM; iItr = iItr + 1)
      {
        pinMode(this->arruiDATA_PINS[iItr], OUTPUT);
      }

      return True;
    }

    Bool configure(void)
    {
      Bool setPinsMode_ReturnValue = this->setPinsMode();
      if(setPinsMode_ReturnValue != True)
      {
        return False;
      }

      Bool setInstructionMode_ReturnValue = this->setInstructionMode();
      if(setInstructionMode_ReturnValue != True)
      {
        return False;
      }

      Bool setWriteMode_ReturnValue = this->setWriteMode();
      if(setWriteMode_ReturnValue != True)
      {
        return False;
      }
      
      Bool _8bitWriteInstruction_ReturnValue = this->_8bitWriteInstruction(CLEAR_SCREEN);
      if(_8bitWriteInstruction_ReturnValue != True)
      {
        return False;
      }

      _8bitWriteInstruction_ReturnValue = this->_8bitWriteInstruction(CRS_HOME);
      if(_8bitWriteInstruction_ReturnValue != True)
      {
        return False;
      }

      _8bitWriteInstruction_ReturnValue = this->_8bitWriteInstruction(CONFIGURE);
      if(_8bitWriteInstruction_ReturnValue != True)
      {
        return False;
      }

      _8bitWriteInstruction_ReturnValue = this->_8bitWriteInstruction(CLEAR_SCREEN);
      if(_8bitWriteInstruction_ReturnValue != True)
      {
        return False;
      }

      return True;
    }


    /* Public Methods. */
    public:
    int begin(void)
    {
      Bool configure_ReturnValue = this->configure();

      if(configure_ReturnValue != True)
      {
        return 0;
      }
      else
      {
        return 1;
      }
    }

    void print(String strOutputText)
    {
      this->_8bitWriteString(strOutputText);
    }

    void clear()
    {
      Bool returnValue_ignored;
      returnValue_ignored = _8bitWriteInstruction(CLEAR_SCREEN);
    }

    int moveCursor(int iTargetRowIdx, int iTargetColIdx)
    {
      if((iTargetRowIdx < LCD_ROW_MIN) || (iTargetRowIdx > LCD_ROW_MAX) || (iTargetColIdx < LCD_COL_MIN) || (iTargetColIdx > LCD_COL_MAX))
      {
        return 0;
      }
        
      int iDistanceToTraverse;
      int iInactivePins = TOTAL_ENTRIES_ROW - ROW_DIM;
      INSTRUCTIONS instruction;


      if(this->cursor.uiRowIdx != iTargetRowIdx)
      {
        iDistanceToTraverse = iInactivePins;

        if(this->cursor.uiRowIdx < iTargetRowIdx)
        {
          iDistanceToTraverse += LCD_COL_MAX - this->cursor.uiColIdx + iTargetColIdx + 1;
        }
        else
        {
          iDistanceToTraverse += LCD_COL_MAX - iTargetColIdx + this->cursor.uiColIdx + 1;
        }
      }
      else
      {
          if(this->cursor.uiColIdx < iTargetColIdx)
          {
            iDistanceToTraverse = iTargetColIdx - this->cursor.uiColIdx;
          }
          else
          {
            iDistanceToTraverse = this->cursor.uiColIdx - iTargetColIdx;
          }
      }

      if(((this->cursor.uiColIdx < iTargetColIdx) && (this->cursor.uiRowIdx == iTargetRowIdx)) || (this->cursor.uiRowIdx < iTargetRowIdx))
      {
        instruction = CRS_INC;
      }
      else
      {
        instruction = CRS_DEC;
      }

      iDistanceToTraverse = iDistanceToTraverse - 1;
      while(iDistanceToTraverse != 0)
      {
        iDistanceToTraverse = iDistanceToTraverse - 1;
        this->_8bitWriteInstruction(instruction);
      }
      
      this->cursor.uiRowIdx = iTargetRowIdx;
      this->cursor.uiColIdx = iTargetColIdx;

      return 1;
    }
};

unsigned int RS_PIN = 2; /* Register select. */
unsigned int RW_PIN = 4; /* Read/Write pin. */
unsigned int ENABLE_PIN = 7; /* Enable pin. */

unsigned int DATA_PINS[] = {8, 9, 10, 11}; /* We unly use 4 bits for data transmission. */

_4bitLCD lcd(RS_PIN, RW_PIN, ENABLE_PIN, DATA_PINS);

void setup(){
  lcd.begin();
}

void loop() {
  delay(20);
  lcd.moveCursor(1, 5);
  delay(3000);
  lcd.moveCursor(0, 9);
  lcd.print("123456789");
  delay(3000);
}
