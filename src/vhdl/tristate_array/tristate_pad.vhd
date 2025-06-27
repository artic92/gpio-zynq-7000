library ieee;
use ieee.std_logic_1164.all;

entity tristate_pad is
  port
  (
    pad_out_en : in    std_logic;   -- Enable output driver (1 = output, 0 = input/hi-Z)
    pad_out    : in    std_logic;   -- Value to drive when output enabled
    pad_in     : out   std_logic;   -- Value read from the pad
    pad_io     : inout std_logic    -- Bidirectional I/O pad
  );
end tristate_pad;

architecture data_flow of tristate_pad is
begin

  -- Drive the pad only when enabled
  pad_io <= pad_out when pad_out_en = '1' else 'Z';

  -- Always read the pad
  pad_in <= pad_io;

end data_flow;
