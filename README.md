# context-path

used to add folders to the PATH variable through the context menu (only works on Windows right now)
it serves as a convenient, lightweight (~25 KB) utility to add folders to the PATH variable through the context menu (you don't need to open the System Properties window everytime as you normally would).

## installation
just download the [executable](https://github.com/deontic/context-path/releases/download/v1.0.0/context-path.exe), right click it, select "Run as Administrator" and it is ready to use 

## usage
<img src="https://user-images.githubusercontent.com/68165727/188911542-7913bff0-6479-4429-b8be-a84154e27b52.gif" width=512>

right click on the containing folder of the file you want to be able to access through the PATH variable and select "Add Folder to Path"

**note:** you must close *all* terminal instances or find a way to refresh PATH before using the newly added directory, 
in the gif above I closed the only open terminal and then opened a new one, after which file.bat was accessible 
