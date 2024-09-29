!/bin/bash

if [ "$REQUEST_METHOD" == "GET" ]; then
  # Procesar GET (QUERY_STRING)
  DATA=$QUERY_STRING
elif [ "$REQUEST_METHOD" == "POST" ]; then
  # Procesar POST (stdin)
  read -n "$CONTENT_LENGTH" DATA
elif [ "$REQUEST_METHOD" == "DELETE" ]; then
  if [ -n "$QUERY_STRING" ]; then
    # Procesar DELETE con datos en la URL
    DATA=$QUERY_STRING
  elif [ "$CONTENT_LENGTH" -gt 0 ]; then
    # Procesar DELETE con datos en el cuerpo
    read -n "$CONTENT_LENGTH" DATA
  fi
fi

# Procesar DATA
DATA=$(echo "$DATA" | tr '&' '\n')
for param in $DATA; do
  key=$(echo "$param" | cut -d '=' -f 1)
  value=$(echo "$param" | cut -d '=' -f 2)
  
  if [ "$key" == "num1" ]; then
    num1=$value
  elif [ "$key" == "num2" ]; then
    num2=$value
  fi
done

# Validar que num1 y num2 sean números
if ! [[ "$num1" =~ ^[0-9]+$ ]] || ! [[ "$num2" =~ ^[0-9]+$ ]]; then
  echo "Content-Type: text/plain"
  echo ""
  echo "Error: num1 and num2 must be numbers."
  exit 1
fi

# Suma los números
sum=$(($num1 + $num2))
