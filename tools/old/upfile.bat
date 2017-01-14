set _split_cmd=FileSplit.exe %2

FOR /f "tokens=*" %%G IN ('%_split_cmd%') DO (
upload %1 %~dp2%%G
rem del %~dp2%%G 
)
