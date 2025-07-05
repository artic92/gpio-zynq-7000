# Makefile for simulating irq_generation with GHDL + GTKWave

# VHDL source files
# SRC = irq_generation.vhd tb_irq_generation.vhd
SRC = ../tristate_array.vhd ../tristate_pad.vhd tristate_array_onboard.vhd tristate_array_onboard_tb.vhd

# Top-level testbench (entity name)
# TB = tb_irq_generation
TB = tristate_array_onboard_tb

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
	# gtkwave $(WAVE) waveform.gtkw &
	@gtkwave $(WAVE) waveform_tristate.gtkw &

clean:
	rm -f *.o *.cf $(WAVE) $(TB)

