# ada

## Requisitos

- [xmake](https://xmake.io) 3.x
- Clang 17 o superior

## Compilar y ejecutar

```sh
xmake f -m debug --toolchain=clang   # una sola vez
xmake build

xmake run ada_cli potencia 2 100    # base y exponente
xmake run ada_cli fibonacci 90      # n
xmake run ada_cli matriz 256        # tamaño (potencia de dos)
```

## Pruebas

```sh
xmake test
```

## Formato y análisis estático

```sh
xmake format          # aplica .clang-format
xmake format -n -e    # solo verifica (sale 255 si algo está mal)
xmake project -k compile_commands --lsp=clangd   # compdb para clangd
xmake check clang.tidy
```

## Tiempos

```sh
xmake f -m release --toolchain=clang
xmake build
xmake run bench          # y opcionalmente: bench <tamaño máximo>
```
