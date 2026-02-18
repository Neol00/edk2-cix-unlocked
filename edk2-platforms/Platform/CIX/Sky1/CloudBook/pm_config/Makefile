CC:=gcc
CFLAGS:=-Wall

csupm_bin_config: csupm_bin_config.c user_config.h opp_config_custom.h
	$(CC) $(CFLAGS) -o $@ $(firstword $^)
	./$@

clean:
	rm csupm_bin_config csu_pm_config.bin
