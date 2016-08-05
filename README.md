# binCompare
two binary files Compare

usage:

```
bincmp  file1  file2  [type]  [start addr]  [lenght]
``` 

option | comment 
----|------
type | 1 2 4 for char word  double-word,default 1 for char
start addr | offset address from file,default 0
lenght | compare data lenght,default whole file content
 
 example:
    bincmp.exe file1 file2  
    bincmp.exe file1 file2 2  
    bincmp.exe file1 file2 4  
    bincmp.exe file1 file2 1 0x100000 256
