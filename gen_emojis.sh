#!/bin/bash

spc () {
  str=""
  let l=$2-${#3}
  for ((i=0; i < $l; i++)); do str+=" "; done
  eval "$1='$str'"
}

# See https://www.codeproject.com/Articles/2995/The-Complete-Guide-to-C-Strings-Part-I-Win32-Chara
cpp_map_emojis () {
  # BOM=Byte Order Mark

  # UTF-8 BOM
  # printf '\xEF\xBB\xBF'

  # UTF-16 big-endian BOM
  #printf '\xFE\xFF'

  # UTF-16 little-endian BOM, that's what Windows use for its values stored in memory (mais ça marche pô ...)
  #printf '\xFF\xFE'

#  echo "static std::map<std::string, std::string> emojis = {"
  let sw1=55
  let sw2=15
  while read c1 c2
  do
    spc spc1 $sw1 $c1
    spc spc2 $sw2 $c2
    echo -E "  { \":${c1}:\", ${spc1}\"\\U000${c2}\"${spc2} },"
  done <emoji_kv.txt  | sort | sed '$ s/.$//'

#  echo "};"
}

disp_emojis () {
  let n=0
  while read c1 c2
  do
    (( n % 9 == 0 )) && echo && echo -n "  "
    let n++
    echo -nE "$c1\U$c2 "
  done <emoji_kv.txt

}

cpp_map_emojis >emoji_map.h
#disp_emojis
