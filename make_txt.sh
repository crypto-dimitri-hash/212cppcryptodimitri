#!/bin/bash
# make_txt.sh -- копии всех исходников с расширением .txt (для отправки письмом).
# Автор: Горчак Дмитрий, 212
#
# Запуск из этой папки:
#   bash make_txt.sh
# Копии складываются в папку txt: point.h -> txt/point.h.txt и так далее.

DIR=txt

mkdir -p $DIR
rm -f $DIR/*.txt

for file in *.h *.cpp; do
    cp "$file" "$DIR/$file.txt"
    echo "  $file -> $DIR/$file.txt"
done

echo "done: $(ls -1 $DIR | wc -l) файлов"
