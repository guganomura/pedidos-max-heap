CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic

MODULOS = src/heap.c src/visualizacao.c
CABECALHOS = src/pedido.h src/heap.h src/visualizacao.h

all: build/pedidos

build/pedidos: src/main.c $(MODULOS) $(CABECALHOS)
	@mkdir -p build
	$(CC) $(CFLAGS) src/main.c $(MODULOS) -o $@

build/testes: tests/test_unitarios.c $(MODULOS) $(CABECALHOS)
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_unitarios.c $(MODULOS) -o $@

run: build/pedidos
	./build/pedidos

test: build/testes build/pedidos
	./build/testes
	sh tests/test_sistema.sh

clean:
	rm -rf build

.PHONY: all run test clean
