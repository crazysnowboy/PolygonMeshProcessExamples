#! /bin/bash  
#批量格式化  
STYLE=--style=ansi
INDENT=--indent-classes --indent-namespaces --indent-cases
KEEP=--keep-one-line-statements --keep-one-line-blocks --break-after-logical
MAXLENGH=--max-code-length=200
FORMATING=--add-brackets
PADDING=--break-blocks --break-blocks=all --pad-paren
for f in $(find . -name '*.c' -or -name '*.cpp' -or -name '*.h' -or -name '*.hpp' -or -name '*.cu' -type f)  
do  
     #indent -l180 $f  
     astyle $STYLE $INDENT $KEEP $MAXLENGH $FORMATING $f  

done  
  
# 删除.orig文件  
for f in $(find . -name '*.orig' -type f)  
do  
     rm $f  
done