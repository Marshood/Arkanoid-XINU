# Arkanoid-XINU version 1.0 bug Not Fixed
Team Members:
 <br>Marshood Ayoub  ([Marshood](https://github.com/Marshood).) <br>
Firas Gadban  <br>
Ameer amer <br>


To run the game , run the resulting exe,
in this case Arkanoid.exe. Do not forget that Ctrl-F1 enables you to view  inners of the system to run just enter "Arkanoid.exe"
in the dosbox. <br>

Arkanoid game in DoxBox Using XINU .<br>

You have to install dosbox to run the exe file : https://www.dosbox.com/download.php?main=1<br>
You have to install XINU OS to compile the c file  : http://math.haifa.ac.il/~oscourse/index_files/xinu4win.exe<br>
You have to install the game and save it on XINU4WIN\NEWSRC\ArkanoidGame<br>
Compile a game :<br>
Lets say you would like to compile one of the examples in
XINU4WIN\NEWSRC\EXAMPLES, ex5.c for instance.<br>
if you want to run the exe file you can do : <br>
    Open a DOSBOX window, and after this write :-<br>
    mount c c:\<br>
    c:<br>
    then go to the folder of the game and run Arkanoid.exe <br>
if you want to compile the code Arkanoid.c you can install Xinu :- <br>
    Open a DOSBOX window, and after this write :-<br>
    mount c c:\<br>
    c:     <br>
Use the "path" command to include tc\bin in your path, for instance:<br>
    path c:\tc\bin <br>
<br>
    cd XINU4WIN\NEWSRC\FileName <br>
    
    and then :<br>
    tcc -I.. Arkanoid.c ..\XINU.LIB <br>
    Arkanoid.exe<br>
NOTE !! <br>
To check the compilation and error messages run<br>
 tcc -I.. Arkanoid.c ..\XINU.LIB > errs<br>
type errs<br>

<br><br><br>

The "> errs" is solely in order not to lose the doskey abilities of the <br>
command window. Due to some bug the turbo linker disables this.<br>

To run the application, run the resulting exe, in this case Arkanoid.exe.<br>
Do not forget that Ctrl-F1 enables you to view  inners of the system<br>
to run just enter "Arkanoid.exe" in the dosbox.<br>
