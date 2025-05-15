# Problem Statement:
### ***Extend SQLite syntax to incorporate at least one ML predicate to enable outlier detection with open-source ML libraries.***     
#### <ins>There are two efforts, broke into the separate directories</ins>:    
1. [SQLite-ML-Files](https://github.com/plmcdowe/54100/tree/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files)
   - This directory contains all files necessary to extend SQLite syntax for:
   numerical outlier detection and textual sentiment analysis.
   - SQLite-ML-Files is documented in this README first because it is the primary effort.
   - Shout out to 
2. [IOT-Arduino-Pi5](https://github.com/plmcdowe/54100/tree/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/IOT-Arduino-Pi5)
   - This directory contains all files necessary to run SQLite-ML on a Raspberry Pi 5 with data from an Arduino Giga R1.
   - IOT-Arduino-Pi5 is documented after SQLite-ML-Files.

# [ 1 ] SQLite-ML-Files

## Files:

#### [ 1.1 ] [ml_extension.c](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/ml_extension.c)

#### [ 1.2 ] [ml_module.py](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/ml_module.py)

#### [ 1.3 ] [shell.c](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/shell.c)

#### [ 1.4 ] [sqlite3](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/sqlite3)

#### [ 1.5 ] [sqlite3.h](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/sqlite3.h)

#### [ 1.6 ] [sqlite3ext.h](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/sqlite3ext.h)

## Local Ubuntu 24 Desktop with Python venv

### <ins>Environment</ins>:
> **<ins>Dell laptop booting Ubuntu Desktop from external NVMe drive</ins>:**
>> *picture*
>>

>> **Python 3.12.3**    
>>> **Python 3-venv**    
>>> **joblib**    
>>> **numpy**    
>>> **scipy**    
>>> **pandas**    
>>> **transformers**    
>>> **scikit-learn**    
>>> **tensorflow**    
>
