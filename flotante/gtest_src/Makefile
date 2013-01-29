
# Setup variables so we find Gtest libraries
GITROOTDIR = $(shell git rev-parse --show-toplevel)
GTEST_DIR = $(GITROOTDIR)/gtest-1.6.0

# Flags passed to the preprocessor & compiler.
CPPFLAGS += -I$(GTEST_DIR)/include
CXXFLAGS += -g -Wall -Wextra
CFLAGS   += -Wall

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS_   = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)


USER_DIR  = ../
BINS	  = gtest-generator gtest-flotante
AUTOGTEST = auto-gtest

# ================ GENERIC RULES ================= #

all : $(BINS)

clean :
	rm -f $(BINS) $(AUTOGTEST) gtest.a gtest_main.a *.o

gtest-generator: gtest-generator.c
	$(CC) $(CFLAGS) $^ -o $@
	@echo " - Generating Test Cases ..."
	@./$@ > $(AUTOGTEST)

# ================= GTEST RULES =================== #

gtest-all.o : $(GTEST_SRCS_)
	@$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	@$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest_main.a : gtest-all.o gtest_main.o
	@$(AR) $(ARFLAGS) $@ $^


# ================== UNIT TESTS =================== #

main.o : $(USER_DIR)/main.c $(USER_DIR)/main.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/main.c

$(AUTOGTEST).o : $(AUTOGTEST).cc $(USER_DIR)/main.h $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

gtest-flotante : main.o $(AUTOGTEST).o gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@
