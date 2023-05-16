SRCFILES := $(wildcard Movies/*.cpp InputHandling/*.cpp)
TSTFILES := $(wildcard Tests/*.cpp)
OUTDIR = artifacts
TDIR = Tests
TSTOUT = tests.exe
OUT = run.exe

OBJFILES  := $(SRCFILES:%.cpp=%.o)
TSTOBJFILES := $(TSTFILES:%.cpp=%.o)

CMPFLG = -I.
LNKFLG = -I. -lm
TSTLNKFLG = -I. -lm -lgtest_main -lgtest

.PHONY: run
run: $(OUTDIR)/$(OUT)
	@echo $(OUTDIR)/$(OUT)
	$(OUTDIR)/$(OUT)

$(OUTDIR)/$(OUT): $(OBJFILES)
	g++ $(CMPFLG) -c main.cpp -o main.o
	g++ $(OBJFILES) main.o $(LNKFLG) -o $@

$(OUTDIR)/$(TSTOUT) : $(OBJFILES) $(TSTOBJFILES)
	@echo "--> Link obj files"
	@echo $(OBJFILES)
	@echo $(TSTOBJFILES)
	g++ $(OBJFILES) $(TSTOBJFILES) $(TSTLNKFLG) -o $@

%.o : %.cpp
	g++ $(CMPFLG) -c $< -o $@

.PHONY: tst
tst: $(OUTDIR)/$(TSTOUT)
	@echo $(OUTDIR)/$(TSTOUT)
	$(OUTDIR)/$(TSTOUT)

# TODO: OS-independent delete
.PHONY: clean
clean:
	@rm -f $(OUTDIR)/$(TSTOUT) $(OUTDIR)/$(OUT) $(OBJFILES) $(TSTOBJFILES)