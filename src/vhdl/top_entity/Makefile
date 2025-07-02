# Makefile for simulating mygpio with GHDL + GTKWave

# VHDL source files
SRC = my_gpio.vhd my_gpio_tb.vhd ../tristate_array/tristate_array.vhd ../tristate_array/tristate_pad.vhd ../irq_generation/irq_generation.vhd

# Top-level testbench (entity name)
TB = my_gpio_tb

# Waveform file
WAVE = waveform.ghw

# GHDL flags
GHDLFLAGS = --ieee=synopsys

all: gtkwave

analyze: $(SRC)
	@ghdl -a $(GHDLFLAGS) $(SRC)

elaborate: analyze
	@ghdl -e $(GHDLFLAGS) $(TB)

run: elaborate
	@ghdl -r $(TB) --wave=$(WAVE)

gtkwave: run
	@gtkwave $(WAVE) waveform.gtkw &

clean:
	@rm -f *.o *.cf $(WAVE) $(TB)

