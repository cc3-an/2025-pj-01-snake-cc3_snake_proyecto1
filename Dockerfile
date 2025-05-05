# Usar la imagen oficial de Ubuntu como base
FROM ubuntu:22.04

# Instalar Python y pip
RUN apt-get update && apt-get install -y python3 python3-pip

# Instalar las bibliotecas necesarias
RUN pip3 install pycparser

# Definir el directorio de trabajo en el contenedor
WORKDIR /app

# Copiar los archivos del proyecto al contenedor
COPY . .

# Comando por defecto para interactuar con el contenedor
CMD ["/bin/bash"]
