# Ruta al archivo .htpasswd
htpasswd_file = "../.htpasswd"

# Frase a escribir
record = "Hola Mundo\n"

# Abrir el archivo en modo append para agregar la frase
with open(htpasswd_file, "a") as f:
    f.write(record)

print("Frase añadida correctamente al archivo .htpasswd")
