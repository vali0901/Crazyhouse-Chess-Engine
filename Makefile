CXXFLAGS = -g -Wall -Werror -std=c++17 -Iinclude -O4
LDLIBS =

SRCDIR := ./src
HDRDIR := ./include
BLDDIR := ./build

# Main dependencies
PRGM  = Main
SRCS := $(shell find $(SRCDIR) -name '*.cpp' ! -iname 'Test*')
HDRS := $(wildcard $(HDRDIR)/*.h)

OBJSH := $(HDRS:.h=.o)
DEPSH := $(OBJSH:.o=.d)

OBJSTMP := $(SRCS:.cpp=.o)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(BLDDIR)/%.o)

DEPS := $(OBJS:.o=.d)

# Test dependencies
TEST = Test
TSRCS = $(shell find $(SRCDIR) -name '*.cpp' ! -iname 'Main*' ! -iname 'Bot*')
TOBJS := $(TSRCS:$(SRCDIR)/%.cpp=$(BLDDIR)/%.o)
TDEPS := $(TOBJS:.o=.d)

.PHONY: all clean

# Rules for building Main
build: $(PRGM)

$(PRGM): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDLIBS) -o $@

$(BLDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Rules for building Test
test: $(TEST)

$(TEST):$(TOBJS)
	$(CXX) $(CXXFLAGS) $(TOBJS) $(LDLIBS) -o $@


clean:
	rm -rf $(OBJS) $(OBJSH) $(DEPS) $(DEPSH) $(TOBJS) $(TDEPS)
	rm -rf $(PRGM) $(TEST)

run: Main
	./Main

zip:
	zip -r Etapa2.zip ./src ./build ./include Makefile README.md


