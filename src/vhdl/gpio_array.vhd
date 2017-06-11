----------------------------------------------------------------------------------
-- Company:
-- Engineer:
--
-- Create Date: 11.04.2017 22:00:58
-- Design Name:
-- Module Name: gpio_array - Structural
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
--! @file gpio_array.vhd
--! @author Antonio Riccio
--! @brief Periferica GPIO a parallelismo configurabile

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity gpio_array is
    Generic ( size : natural := 4 );                        --! parallelismo della periferica
    Port ( pad_out : in STD_LOGIC_VECTOR (size-1 downto 0); --! vettore di segnali per la scrittura del dato verso l'esterno
           pad_en : in STD_LOGIC_VECTOR (size-1 downto 0);  --! vettore di segnali di selezione modalità
           pad_in : out STD_LOGIC_VECTOR (size-1 downto 0); --! vettore di segnali per la lettura del dato dall'esterno
           pad : inout STD_LOGIC_VECTOR (size-1 downto 0)); --! vettore di segnali per la comunicazione con l'esterno
end gpio_array;

--! Architettura nel dominio strutturale per gpio_array
architecture Structural of gpio_array is

--! Dichiarazione del componente gpio definito in @ref GPIO
component gpio_pad is
    Port ( pad_out : in STD_LOGIC;
           pad_en : in STD_LOGIC;
           pad_in : out STD_LOGIC;
           pad : inout STD_LOGIC);
end component;

begin

--! Instanziazione di un array di dimensione size
gpio_gen: for i in 0 to size-1 generate
    gpio_inst: gpio_pad port map(
                                 pad_out => pad_out(i),
                                 pad_en => pad_en(i),
                                 pad_in => pad_in(i),
                                 pad => pad(i));
end generate;


end Structural;
