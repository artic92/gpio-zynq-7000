----------------------------------------------------------------------------------
-- Company:
-- Engineer:
--
-- Create Date: 11.04.2017 21:59:07
-- Design Name:
-- Module Name: gpio_pad - DataFlow
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

---------------------------------------------------
--! @file gpio.vhd
--! @brief Periferica GPIO a parallelismo unitario
---------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity gpio_pad is
    Port ( pad_out : in STD_LOGIC;  --! segnale per la scrittura del dato verso l'esterno
           pad_en : in STD_LOGIC;   --! segnale di selezione modalità. Se il segnale è 1 la periferica è in scrittura, se 0 è in lettura
           pad_in : out STD_LOGIC;  --! segnale per la lettura del dato dall'esterno
           pad : inout STD_LOGIC);  --! segnale per la comunicazione con l'esterno
end gpio_pad;

--! Architettura nel dominio dataflow per gpio_pad
architecture DataFlow of gpio_pad is

begin

--! La periferica viene descritta utilizzando una logica tri-state
pad <= pad_out when pad_en = '1'
       else 'Z';

pad_in <= pad;

end DataFlow;
