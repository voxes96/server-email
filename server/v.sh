valgrind --leak-check=full \
	         --show-leak-kinds=all \
		          --track-origins=yes \
			           --verbose \
				            --log-file=valgrind-out.txt \
./server.out localhost 1234 wp.pl
