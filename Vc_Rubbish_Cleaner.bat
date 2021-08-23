@echo off
echo 正在清除Vc rubbish,请稍等......
echo /s 删除子目录下文件 /f 删除只读文件
del /S /f *.obj
del /S /f *.exp
del /S /f *.pch
del /S /f *.idb
del /S /f *.ilk
del /S /f *.pdb
del /S /f *.sbr
del /S /f *.ncb
del /S /f *.suo
del /S /f *.o
rd /S /Q debug

echo 清除Vc rubbish完成!
echo. & pause

