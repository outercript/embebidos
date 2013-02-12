
# Setup variables so we find Gtest libraries
GITROOTDIR = $(shell git rev-parse --show-toplevel)
GTEST_DIR = $(GITROOTDIR)/gtest-1.6.0

# Flags passed to the preprocessor & compiler.
CPPFLAGS += -I$(GTEST_DIR)/include
CXXFLAGS += -g -Wall -Wextra
CFLAGS   += -Wall
MAKEFLAGS += -j4

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS_   = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)


USER_DIR  = ../
BINS	  = gtest-generator gtest-flotante
TEST_NAME = multiplication division addition substraction
TESTS	  = $(addprefix test-,$(TEST_NAME))
TESTS_SRC = $(addsuffix .cc, $(TESTS))
TESTS_OBJ = $(addsuffix .o, $(TESTS_SRC))

# ================ GENERIC RULES ================= #

all : $(BINS)

clean :
	rm -f $(BINS) $(TESTS_SRC) gtest.a gtest_main.a *.o

gtest-generator: gtest-generator.c
	$(CC) $(CFLAGS) $^ -o $@

$(TESTS_SRC): gtest-generator
	@echo "Generating $@"
	@./gtest-generator $(subst test-,,$@) > $@

# ================= GTEST RULES =================== #

gtest-all.o : $(GTEST_SRCS_)
	@$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	@$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(USER_DIR)/gtest_src/gtest_main.cc

gtest_main.a : gtest-all.o gtest_main.o
	@$(AR) $(ARFLAGS) $@ $^


# ================== UNIT TESTS =================== #

$(TESTS_OBJ) : $(TESTS_SRC) $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(subst .o,,$@) -o $@

gtest-flotante : $(TESTS_OBJ) gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@
