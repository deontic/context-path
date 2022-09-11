# context-path

### (only Windows supported right now)
it serves as a convenient, lightweight (<20 KB) utility to add folders to the PATH variable through the context menu so you don't need to open the System Properties window everytime as you normally would.

## installation
just download the [executable](https://github.com/deontic/context-path/releases/download/v2.0.0/context-path.exe), right click it, select "Run as Administrator" and it is ready to use 

## usage
<img src="https://user-images.githubusercontent.com/68165727/188911542-7913bff0-6479-4429-b8be-a84154e27b52.gif" width=512>


**NOTE:** as usual, you must close *all* terminal instances or find another way to refresh environment variables in your terminal (e.g refreshenv) before using the newly added directory (read more: https://superuser.com/a/107530)

in the gif above I closed the only open terminal and then opened a new one so it uses updated environment variables

right click on the containing folder of the file you want to be able to access through the PATH variable and select `CP Add Folder to Path`

