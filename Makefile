.PHONY: truco

alloc:
		${CC} -c ./lib/alloc/malloc-list.c -o alloc.o

deck:
		${CC} -c ./lib/deck/cards.c -o deck.o

state:
		${CC} -c ./lib/state/truco-state.c -o state.o

tree:
		${CC} -c ./lib/tree/truco-node.c -o tree.o

clean:
		rm -f alloc.o deck.o state.o tree.o truco.o truco

truco: clean deck alloc state tree
		${CC} -c truco.c -o truco.o
		${CC} truco.o deck.o alloc.o state.o tree.o -lm -o truco