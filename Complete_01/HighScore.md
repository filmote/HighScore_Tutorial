# Getting Started

A high score feature turns a good game into a great game by tempting the player to continue playing and beat their previous effort.

In this tutorial, we will show you how to create a simple high score screen that allows the player to enter their initials alongside their new high score.  The high scores will be saved on the Arduboy's non-volatile memory allowing them to retrieved even after the device has been turned off and back on again.  We will start by saving and reading data from this special memory, build a high score screen showing a list of entries and finally adding the ability for the player to enter their initials.

**Important**: this tutorial builds on the State Machine presented in the previous tutorial.  If you have not completed that tutorial, I recommend you do it prior to embarking on this one!


# EEPROM Memory

The Arduboy's non-volatile memory is known as EEPROM memory and is ideal for saving user settings, high scores and other information between sessions.  EEPROM stands for **E**lectrically **E**rasable **P**rogrammable **R**ead-**O**nly **M**emory but this is a misnomer as the memory can actually be updated.  EEPROMs have a limited life and will eventually fail after they have been erased and rewritten too many times – this number may be in the millions of operations but a poorly written program that attempts to use it as working memory could easily reach that!

The Arduboy has 1KB of EEPROM memory only.

The various functions required to access the EEPROM memory are wrapped up in a standard Arduino library named `EEPROM.h`.  Normally, to access external functions you would have to add the line `#include <EEPROM.h>` to the top of your program but as the `Arduboy2` library already uses the library, there is no need as its included implicitly.

The EEPROM class provides three basic functions to read and write a **single** byte of memory, as shown below.  The memory location can be anywhere in the 1Kb and equates to a value between 0 and 1023.  The `update()` function differs from the `write()` function in that it checks the value to be written against what is already stored in order to minimize the number of updates thus prolonging the life of the EEPROM.
 
```cpp
EEPROM.read(memory_location);
EEPROM.update(memory_location, value);
EEPROM.write(memory_location, value);
```

An example application might use functions such as those below:
 
```cpp
#define EEPROM_START    200

uint8_t levelNo = 3;

uint8_t getSavedLevel() { 
    return EEPROM.read(EEPROM_START); 
}

void updateSavedLevel(uint8_t levelNo) { 
    EEPROM.update(EEPROM_START, levelNo);
}
```

However, some variables are larger than a single byte and the library offers two more functions that can save and retrieve datatypes other than a byte, such as a float, int or even a structure.  These functions are smart enough to know how big each variable is and read or write the correct amount of bytes accordingly.

```cpp
EEPROM.put(memory_location, value);
EEPROM.get(memory_location, value);
```
 
To save and retrieve a 16-bit integer, we can use these functions as shown:

```cpp
#define EEPROM_START    200

uint16_t score = 1234;

void saveScore(uint16_t score) { 
  EEPROM.put(SAVEEEPROM_STARTD_STATUS, score); 
}

uint16_t getScore() {

  uint16_t score;
  EEPROM.get(EEPROM_START, score);

  return score;
}
```

You will note in the `getScore()` function above that a variables called score is created before calling the `EEPROM.get()` function.  This sets up an area in memory (in this case two bytes) for the function to write the bytes returned from the EEPROM memory to.


> Your Turn:<br/><br/>
> The code provided consists of two screen states - a title and a high score.  Initially, we will store a single high score to the EEPROM memory and retrieve these to display on the high score screen.  A high score entry consists of three initials and a single score.  The initials are represented by a single byte (or uint8\_t) whereas the score will be a 16-bit integer (uint16\_t).<br/><br/>
>In the `highScoreTable_Init()` function, add code to write the three initials and score into EEPROM.  You will see that there is already a constant `EEPROM_START` defined and your data should be stored from this location onwards.  Review the sample code above to see how to write single bytes and multi-byte variables to EEPROM.<br/><br/>

Now that we have saved data to the EEPROM, lets retrieve it and display it on the High Score screen.

> Your Turn:<br/><br/>
>In the `highScore` function, retrieve the saved data back from the EEPROM and display it on the screen.  You will see that the first initial is already done for you and it shows a little trick - even though we saved the data as `chars` such as 'A', 'B' and 'C' when we read it back it looks like an `uint8_t` and printing them would actually print the numeric 65, 66 and 67.  Casting the value to a `char` using the code `static_cast<char>(...)` ensures the `print()` statement prints out the character representation rather than the numerical value.<br/><br/>
>Again in the `highScore` function, retrieve the score from EEPROM and display it on the screen.  As we are trying to achieve a retro-arcade look, see if you can add zero-padding in front of number so that the number is shown like `0012` rather than plain `12`.



# A Final Note
 
The Arduboy2 library uses the first 16 bytes of EEPROM to store settings such as the unit’s name and the current audio settings.  The library also contains a constant, `EEPROM_STORAGE_SPACE_START`, which defines the starting memory location for user applications to use.  You are free to use the memory above this location anyway you like however to ensure compatibility with this and future versions of the library, you should define the constants used when saving and retrieving EEPROM values relative to the `EEPROM_STORAGE_SPACE_START `constant.  An example of this is shown below.

```
#define EEPROM_START     EEPROM_STORAGE_SPACE_START
#define EEPROM_FINISH    EEPROM_STORAGE_SPACE_START + 12
```

