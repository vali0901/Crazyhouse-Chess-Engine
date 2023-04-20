CXXFLAGS = -g -Wall -Werror -std=c++17 -Iinclude
LDLIBS =

SRCDIR := ./src
HDRDIR := ./include
BLDDIR := ./build

PRGM  = Main
SRCS := $(shell find $(SRCDIR) -name '*.cpp')
HDRS := $(wildcard $(HDRDIR)/*.h)

OBJSH := $(HDRS:.h=.o)
DEPSH := $(OBJSH:.o=.d)

OBJSTMP := $(SRCS:.cpp=.o)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(BLDDIR)/%.o)

DEPS := $(OBJS:.o=.d)

.PHONY: all clean

build: $(PRGM)

$(PRGM): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDLIBS) -o $@

$(BLDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJS) $(OBJSH) $(DEPS) $(DEPSH)
	rm -rf $(PRGM)

run: Main
	./Main

zip:
	zip -r Etapa3.zip ./src/* ./include/*
