#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Uso: ./ocultarinfo.sh <archivo>"
    exit 1
fi

ARCHIVO="$1"

if [ ! -f "$ARCHIVO" ]; then
    echo "Error: no existe el archivo $ARCHIVO"
    exit 1
fi

awk '
function censura(cantidad, cadena, i) {
    cadena=""
    for(i=0; i<cantidad; i++) cadena=cadena "*"
    return cadena
}

function ocultar_correo(correo, partes, usuario, dominio) {
    split(correo, partes, "@")
    usuario=partes[1]
    dominio=partes[2]

    if(length(usuario) <= 2)
        return substr(usuario,1,1) "***@" dominio

    return substr(usuario,1,2) censura(length(usuario)-2) "@" dominio
}

{
    linea=$0

    while(match(linea, /[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}/)) {
        correo=substr(linea, RSTART, RLENGTH)
        censurado=ocultar_correo(correo)
        linea=substr(linea,1,RSTART-1) censurado substr(linea,RSTART+RLENGTH)
    }

    gsub(/password=[^ ]+/, "password=********", linea)
    gsub(/passwd=[^ ]+/, "passwd=********", linea)
    gsub(/clave=[^ ]+/, "clave=********", linea)
    gsub(/token=[^ ]+/, "token=********", linea)
    gsub(/api_key=[^ ]+/, "api_key=********", linea)

    while(match(linea, /[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}/)) {
        ip=substr(linea, RSTART, RLENGTH)
        split(ip, parte, ".")
        censurada=parte[1] "." parte[2] "." parte[3] ".xxx"
        linea=substr(linea,1,RSTART-1) censurada substr(linea,RSTART+RLENGTH)
    }

    print linea
}
' "$ARCHIVO"