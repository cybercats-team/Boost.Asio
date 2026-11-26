timer=tutorial/timer
daytime=tutorial/daytime

.PHONY: all
all: tutorial-timer1 tutorial-timer2 tutorial-timer3 tutorial-timer4 tutorial-timer5 tutorial-clock tutorial-daytime

bin:
	mkdir -p bin

tutorial-timer1: bin bin/tutorial-timer1

tutorial-timer2: bin bin/tutorial-timer2

tutorial-timer3: bin bin/tutorial-timer3

tutorial-timer4: bin bin/tutorial-timer4

tutorial-timer5: bin bin/tutorial-timer5

tutorial-clock: bin bin/tutorial-clock

tutorial-daytime-client: bin bin/tutorial-daytime-client

tutorial-daytime-server: bin bin/tutorial-daytime-server

tutorial-daytime-server-async: bin bin/tutorial-daytime-server-async

tutorial-daytime: tutorial-daytime-client tutorial-daytime-server tutorial-daytime-server-async

bin/tutorial-timer1:
	g++ -Wall -Wextra -Wpedantic $(timer)/timer1.cpp $(timer)/util/stopwatch.cpp -o bin/tutorial-timer1

bin/tutorial-timer2:
	g++ -Wall -Wextra -Wpedantic $(timer)/timer2.cpp $(timer)/util/stopwatch.cpp -o bin/tutorial-timer2

bin/tutorial-timer3:
	g++ -Wall -Wextra -Wpedantic $(timer)/timer3.cpp -o bin/tutorial-timer3

bin/tutorial-timer4:
	g++ -Wall -Wextra -Wpedantic $(timer)/timer4.cpp -o bin/tutorial-timer4	

bin/tutorial-timer5:
	g++ -Wall -Wextra -Wpedantic $(timer)/timer5.cpp -o bin/tutorial-timer5

bin/tutorial-clock:
	g++ -Wall -Wextra -Wpedantic $(timer)/clock.cpp $(timer)/util/rawterminal.cpp -o bin/tutorial-clock	

bin/tutorial-daytime-client:
	g++ -Wall -Wextra -Wpedantic $(daytime)/tcp-client.cpp -o bin/tutorial-daytime-client

bin/tutorial-daytime-server:
	g++ -Wall -Wextra -Wpedantic $(daytime)/tcp-server.cpp $(daytime)/util/datetime.cpp -o bin/tutorial-daytime-server

bin/tutorial-daytime-server-async:
	g++ -Wall -Wextra -Wpedantic $(daytime)/tcp-server-async.cpp $(daytime)/util/datetime.cpp -o bin/tutorial-daytime-server-async

.PHONY: clean
clean:
	rm -f bin/*