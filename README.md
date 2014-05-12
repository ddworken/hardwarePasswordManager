hardwarePasswordManager
=======================
A password manager program for an Arduino Micro. 
Current Features: 

    1. Counts reset button presses to determine password to enter (acceptable values are 0-9)
    2. Passwords are received over serial as Strings and stored into EEPROM in place designated by the user (Currently supports 10 passwords)
        1. Due to storing the passwords in the EEPROM, they will remain in the password manager after a power off (even if not set in compiled code)
    3. Supports passwords up to 99 characters with all ASCII printable characters (32-127 in DEC)
    4. 3 different ways of combating keyloggers: 
        1. Moves mouse away from current input box, types in a random amount of random chars, then moves mouse back to input box. The keylogger will only be able to recover the password if it tracks mouse movement and clicking. Furthermore, a human would need to review the data to determine the password. 
        2. Types password into input box one character at a time. After each character it types a random amount of random characters and then proceeds to delete these. The keylogger will only be able to recover the password if it tracks the use of delete. Furthermore, the keylogger may require human intervention in order to turn the log into a usable password. 
        3. Types password into input box one character at a time. After each character it opens a new browser tab (Ctrl+T), types in a random amount of random characters and then closes the tab. The keylogger will only be able to recover the password if it has been specifically made to track usage pattern. 
    
