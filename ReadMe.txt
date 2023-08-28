# The-Sushi-Train-Problem
A script that accepts a file path as a command line argument.. The given file will
contain the scans per minute from the start of the day up till the last time. Calculate the output
expected on the display screen at the final timestamp and write to stdOut.
The output is a single integer representing the total number of trays on the sushi train
(accumulated number of trays that have been scanned in minus the number of trays scanned
out) at the last time after applying the adjustment logic.

**Follow the following instructions to run the files**

1. Open the "Sushi_train_V4.c" file in a C compiler (preferably VS Code).

2. Open the terminal of the compiler. If you are on VS code press "Ctrl+Shift+`".

3. Make sure you are in the folder/workspace where the files are placed.

4. Write the following command in terminal window:

	"gcc -o Sushi_train_V4.exe Sushi_train_V4.c"

5. Followed by the command:

	"./Sushi_train_V4.exe [filename.csv]"
