SRCFILES := $(wildcard Movies/*.cpp InputHandling/*.cpp)
TSTFILES := $(wildcard Tests/*.cpp)
OUTDIR = artifacts
TDIR = Tests
TSTOUT = tests.exe
OUT = run.exe

RMCMD = rm -f

NANALIBPATH = ../nana
NANAINCLUDE = $(NANALIBPATH)/include
NANALIBS = -lnana -lgdi32 -lcomdlg32 -lz

OBJFILES  := $(SRCFILES:%.cpp=%.o)
TSTOBJFILES := $(TSTFILES:%.cpp=%.o)

CMPFLG = -I. -O3 -I$(NANAINCLUDE)
LNKFLG = -I. -I$(NANAINCLUDE) -L$(NANALIBPATH) -lm $(NANALIBS)
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
	@$(RMCMD) $(OUTDIR)/$(TSTOUT) $(OUTDIR)/$(OUT) $(OBJFILES) $(TSTOBJFILES)
