# Makefile for simulating mygpio with GHDL + GTKWave

# VHDL source files
SRC = top_entity_onboard.vhd top_entity_onboard_tb.vhd ../my_gpio.vhd ../../tristate_array/tristate_array.vhd ../../tristate_array/tristate_pad.vhd ../../irq_generation/irq_generation.vhd

# Top-level testbench (entity name)
TB = top_entity_tb

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

