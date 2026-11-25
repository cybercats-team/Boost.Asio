timer=tutorial/timer
util=$(timer)/util

.PHONY: all
all: tutorial-timer1 tutorial-timer2 tutorial-timer3 tutorial-timer4 tutorial-clock

bin:
	mkdir -p bin

tutorial-timer1: bin bin/tutorial-timer1

tutorial-timer2: bin bin/tutorial-timer2

tutorial-timer3: bin bin/tutorial-timer3

tutorial-timer4: bin bin/tutorial-timer4

tutorial-clock: bin bin/tutorial-clock

bin/tutorial-timer1:
	g++ -Wall -Wextra -Wpedantic $(timer)/timer1.cpp $(util)/stopwatch.cpp -o bin/tutorial-timer1

bin/tutorial-timer2:
	g++ -Wall -Wextra -Wpedantic $(timer)/timer2.cpp $(util)/stopwatch.cpp -o bin/tutorial-timer2

bin/tutorial-timer3:
	g++ -Wall -Wextra -Wpedantic $(timer)/timer3.cpp -o bin/tutorial-timer3

bin/tutorial-timer4:
	g++ -Wall -Wextra -Wpedantic $(timer)/timer4.cpp -o bin/tutorial-timer4	

bin/tutorial-clock:
	g++ -Wall -Wextra -Wpedantic $(timer)/clock.cpp $(util)/rawterminal.cpp -o bin/tutorial-clock	

.PHONY: clean
clean:
	rm -f bin/*