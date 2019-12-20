CC = g++

MAIN = main
ENGI = engine
MINE = miner
ENTI = entity
PERL = perlin

$(MAIN): $(MAIN).o $(ENGI).o $(PERL).o $(ENTI).o
	$(CC) -o $@ $^ -Wall -lm -std=c++11 -lsfml-graphics -lsfml-window -lsfml-system

$(MAIN).o: $(MAIN).cpp
	$(CC) -c $^
	
$(ENGI).o: $(ENGI).cpp $(ENGI).hpp
	$(CC) -c $^
	
$(PERL).o: $(PERL).cpp $(PERL).hpp
	$(CC) -c $^
	
$(ENTI).o: $(ENTI).cpp
	$(CC) -c $^
	
clean:
	rm -f *.o
	rm -f *.hpp.gch

