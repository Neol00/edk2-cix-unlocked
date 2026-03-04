CC:=gcc
CFLAGS:=-Wall

all: csu_pm_config.bin

csu_pm_config.bin: csupm_bin_config
	./csupm_bin_config

csupm_bin_config: csupm_bin_config.c user_config.h opp_config_custom.h
	$(CC) $(CFLAGS) -o $@ $(firstword $^)

.PHONY: csupm_bin_config

clean:
	rm -f csupm_bin_config csu_pm_config.bin
