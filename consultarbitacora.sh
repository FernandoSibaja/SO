#!/bin/bash

bitacora="$1"
if [ ! -f "$bitacora" ]; then
    echo "no existe la bitacora $bitacora"
    exit 1
fi

if [ $# -eq 1 ]; then
    cat "$bitacora"
    exit 0
fi

filtro="$2"
valor="$3"

if [ -z "$valor" ]; then
    echo "falta el valor a filtrar"
    exit 1
fi

case "$filtro" in
    directorio)
        awk -F'|' -v val="$valor" '
        {
            if($1 ~ val) print $0
        }' "$bitacora"
        ;;
    archivo)
        awk -F '|' -v val="$valor" '
        {
            gsub(/^ +| +$/, "", $2)
            if($2 == val)print $0
        }' "$bitacora"
        ;;
    id)
        awk -F '|' -v val="$valor" '
        {
            gsub(/^ +| +$/, "", $3)
            if($3 == val)print $0
        }' "$bitacora"
        ;;
    usuario)
        awk -F '|' -v val="$valor" '
        {
            gsub(/^ +| +$/, "", $4)
            if($4 == val)print $0
        }' "$bitacora"
        ;;
  
    fecha)
        grep "$valor" "$bitacora"
        ;;
    *)
    echo "Filtro no valido: $filtro"
    echo "Filtros validos: usuario, id, fecha, archivo, directorio"
    exit 1
    ;;
esac


    
        
