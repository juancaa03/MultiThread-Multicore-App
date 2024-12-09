def analizar_primos(fichero):
    try:
        # Leer los números del fichero en una lista
        with open(fichero, 'r') as f:
            numeros = [int(line.strip()) for line in f if line.strip().isdigit()]

        print(f"Total de números en el archivo: {len(numeros)}")

        # Verificar si hay números repetidos
        vistos = set()
        repetidos = set()
        for num in numeros:
            if num in vistos:
                repetidos.add(num)
            else:
                vistos.add(num)

        if repetidos:
            print(f"Se encontraron números repetidos: {len(repetidos)} (ejemplo: {list(repetidos)})")
        else:
            print("No hay números repetidos.")

        # Verificar si hay números pares (excepto el 2)
        pares = [x for x in numeros if x % 2 == 0 and x != 2]
        if pares:
            print(f"Se encontraron números pares (excepto el 2): {len(pares)} (ejemplo: {pares[:10]})")
        else:
            print("No hay números pares (excepto el 2).")

    except FileNotFoundError:
        print(f"No se pudo encontrar el fichero '{fichero}'. Asegúrate de que exista.")
    except ValueError:
        print("Hubo un error al procesar los datos. Asegúrate de que el fichero contenga solo números.")

# Llamar a la función
if __name__ == "__main__":
    archivo = input("Introduce el nombre del fichero con los números primos: ")
    analizar_primos(archivo)