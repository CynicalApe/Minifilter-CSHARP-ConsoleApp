# Minifilter-CSHARP-Console-Command

## The Filter
* Currenly can deny access to individual files or volumes as a whole. 
* It can categorize permissions between read-only, write-only and no access restrictions
* uses :permission_enum:filtepath; style for each entry such as (:5:C:\random.txt where 5 is write-only access)
* For every request, it sends the name of the file that triggered the request to the console app for a quick restriction check
* Then according to the the app's response it either cancels or forwards the request to the next driver on the stack

## The APP
* App has the decision mechanism for the driver to ignore or forward the request to the next level. 
* Uses marhsalled Ftllib.dll functions and structures to talk with the driver.
* Uses Entity framework to read from or add new restricted files. 
* Has built-in functions for changing the database
* Every successfull access to the listed files will be logged in another entity framework database.

