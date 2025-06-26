# Makefile for simulating irq_generation with GHDL + GTKWave

# VHDL source files
SRC = irq_generation.vhd tb_irq_generation.vhd

# Top-level testbench (entity name)
TB = tb_irq_generation

# Waveform file
WAVE = waveform.ghw

# GHDL flags
GHDLFLAGS = --ieee=synopsys

all: gtkwave

analyze: $(SRC)
	ghdl -a $(GHDLFLAGS) $(SRC)

elaborate: analyze
	ghdl -e $(GHDLFLAGS) $(TB)

run: elaborate
	ghdl -r $(TB) --wave=$(WAVE)

gtkwave: run
	gtkwave $(WAVE) waveform.gtkw &

clean:
	rm -f *.o *.cf $(WAVE) $(TB)

