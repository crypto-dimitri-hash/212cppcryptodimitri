#!/bin/bash

DIR=txt

mkdir -p $DIR
rm -f $DIR/*.txt

for file in *.cpp; do
    name="${file%.cpp}"
    cp "$file" "$DIR/$name.txt"
    echo "  $file -> $DIR/$name.txt"
done

for file in *.h; do
    name="${file%.h}"
    cp "$file" "$DIR/$name-h.txt"
    echo "  $file -> $DIR/$name-h.txt"
done

echo "done: $(ls -1 $DIR | wc -l) files"
