#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <caracter>"
    exit 1
fi

caracter="$1"
prop_corecte=0

while IFS= read -r line; do
    if [[ $line =~ ^[[:upper:]][[:alnum:]\ \,]*[\?\!\.]$ && ! $line =~ ,\ (si) ]]; then
        if [[ $line == *"$caracter"* ]]; then
            ((prop_corecte++))
        fi
    fi
done

if [ "$prop_corecte" -eq 0 ]; then
    echo "Nicio propozitie corecta care sa contina '$caracter'."
else
    echo "Numarul de propozitii corecte care contin '$caracter': $prop_corecte"
fi
