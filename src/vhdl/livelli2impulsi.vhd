----------------------------------------------------------------------------------
-- Company:
-- Engineer:
--
-- Create Date:    10:10:06 12/28/2015
-- Design Name:
-- Module Name:    livelli2impulsi - Behavioral
-- Project Name:
-- Target Devices:
-- Tool versions:
-- Description:
--
-- Dependencies:
--
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
----------------------------------------------------------------------------------
----------------------------------------------------------
--! @file livelli2impulsi
--! @brief Automa a stati per la generazione di un impulso
----------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity livelli2impulsi is
    Port ( input : in  STD_LOGIC;
           clock : in  STD_LOGIC;
           output : out  STD_LOGIC);
end livelli2impulsi;

architecture Behavioral of livelli2impulsi is

signal output_sig : std_logic := '0';

begin

output <= output_sig;

generazione_segnale_impulsivo : process(clock)
variable state : std_logic := '0';
begin
	if(rising_edge(clock)) then
		if(state = '0' and input = '1') then
			output_sig <= '1';
			state := '1';
		elsif(state = '1' and input = '1') then
			output_sig <= '0';
		else -- Se input = '0' e si trova in qualsiasi stato
			state := '0';
			output_sig <= '0';
		end if;
	end if;
end process;

end Behavioral;
