# Makefile for simulating irq_generation with GHDL + GTKWave

# VHDL source files
SRC = ../irq_generation.vhd irq_generation_onboard.vhd irq_generation_onboard_tb.vhd

# Top-level testbench (entity name)
TB = irq_generation_onboard_tb

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

