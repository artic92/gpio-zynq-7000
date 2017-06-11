----------------------------------------------------------------------------------
-- Company:
-- Engineer:
--
-- Create Date: 01.06.2017 11:24:11
-- Design Name:
-- Module Name: irq_generation - Structural
-- Project Name:
-- Target Devices:
-- Tool Versions:
-- Description:
--
-- Dependencies:
--
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
----------------------------------------------------------------------------------
--! @file irq_generation.vhd
--! @author Antonio Riccio
--! @brief Logica di gestione delle interruzioni

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

--! Libreria necessaria per l'utilizzo della funzione or_reduce
use IEEE.STD_LOGIC_MISC.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity irq_generation is
    Generic ( size : natural := 4);
    Port ( clock : in STD_LOGIC;
           irq_enable : in STD_LOGIC_VECTOR (size-1 downto 0);    --! vettore di segnali per l'abilitazione alla generazione di interruzione
           irq_clear : in STD_LOGIC_VECTOR (size-1 downto 0);     --! vettore di segnali per l'acknowledge dell'interruzione
           irq_source : in STD_LOGIC_VECTOR (size-1 downto 0);    --! vettore di segnali fonte di interruzione
           irq_pending : out STD_LOGIC_VECTOR (size-1 downto 0);  --! vettore di segnali che indicano la presenza di un interrupt pending
           irq : out STD_LOGIC);                                  --! segnale collegato alla PS per la segnalazione di un evento interrompente
end irq_generation;

--! @brief Architettura nel dominio strutturale per irq_generation
architecture Structural of irq_generation is

--! @details Il componente seguente genera un segnale impulsivo a partire da un segnale a livelli.
--! Questo è necessario per consentire al latch di catturare il segnale
--! in ingresso solo nel momento in cui si verifica la richiesta di interruzione
--! da parte della fonte interrompente.
component livelli2impulsi is
    Port ( input : in  STD_LOGIC;
           clock : in  STD_LOGIC;
           output : out  STD_LOGIC);
end component;

signal irq_sig, pending_sig, latch_enable_sig : std_logic_vector(size-1 downto 0);

begin

detection_logic : for i in 0 to size-1 generate
  enable_impulsivo : livelli2impulsi
      port map(
               input => irq_source(i),
               clock => clock,
               output => latch_enable_sig(i));
end generate;

--! La logica per la generazione di un IRQ è composta da latch, in numero pari
--! al parametro size. Le uscite dei latch compongono il registro delle interruzioni pendenti (irq_pending).
--! Si è deciso di utilizzare latch invece di un singolo registro per dare all'utilizzatore della
--! periferica un controllo a grana fine delle interruzioni. In questo modo, infatti, è possibile controllare
--! lo stato di ciascun bit, il quale corrisponde al singolo pin della fonte interrompente.
irq_gen_inst : for i in 0 to size-1 generate
	pending_latch_i : process(latch_enable_sig(i), irq_clear(i), irq_source(i))
	begin
		if (irq_clear(i) = '1') then
			pending_sig(i) <= '0';
		elsif (latch_enable_sig(i) = '1') then
			pending_sig(i) <= irq_source(i);
		end if;
	end process;

	irq_pending(i) <= pending_sig(i);

  --! L'interruzione sul canale i-esimo sarà generata soltanto se il corrisponendete bit
  --! di abilitazione è 1
	irq_sig(i) <= pending_sig(i) and irq_enable(i);
end generate;

--! Il segnale irq sarà alto se almeno un bit del segnale irq_sig è alto
irq <= or_reduce(irq_sig);

end Structural;
