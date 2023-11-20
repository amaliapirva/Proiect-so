#!/bin/bash

if [ "$#" -ne 1 ]; then
   echo "Ussage: $0 <c> "
   exit 1
fi

caracter="$1"
counter=0

is_sentence_right(){
    sentence="$1"

    if [[ ! "$sentence" = ~^[[:upper:]] ]]; then
	return 1
    fi

    if [[ "$sentence" = ~^[[:upper:][:lower:][:digit:][:space:],.!][.!?]$ || "$sentence" = ~,.*[:space:]] ]]; then
    return 1
fi

return 0

}
 while read -r sentence; do
     if is_sentence_right "$sentence"; then
	 ((counter++))
     fi
 done

 echo $counter
