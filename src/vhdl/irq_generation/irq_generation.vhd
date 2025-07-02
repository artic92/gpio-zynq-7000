library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_misc.all;

entity irq_generation is
    generic
    (
      GPIO_WIDTH : natural := 4
    );
    port
    (
      clock           : in  std_logic;
      reset           : in  std_logic;
      irq_source_in   : in  std_logic_vector (GPIO_WIDTH-1 downto 0);
      irq_enable_in   : in  std_logic_vector (GPIO_WIDTH-1 downto 0);
      irq_clear_in    : in  std_logic_vector (GPIO_WIDTH-1 downto 0);
      irq_pending_out : out std_logic_vector (GPIO_WIDTH-1 downto 0);
      irq_out         : out std_logic
    );
end irq_generation;

architecture behavioral of irq_generation is

  signal irq_enable_q   : std_logic_vector(GPIO_WIDTH-1 downto 0);
  signal irq_clear_q    : std_logic_vector(GPIO_WIDTH-1 downto 0);
  signal irq_source_q   : std_logic_vector(GPIO_WIDTH-1 downto 0);
  signal irq_pending_q  : std_logic_vector(GPIO_WIDTH-1 downto 0);

begin

  input_reg_proc : process(clock, reset)
  begin
    if (rising_edge(clock)) then
      if (reset = '1') then
        irq_enable_q <= (others => '0');
        irq_clear_q  <= (others => '0');
        irq_source_q <= (others => '0');
      else
        irq_enable_q <= irq_enable_in;
        irq_clear_q  <= irq_clear_in;
        irq_source_q <= irq_source_in;
      end if;
    end if;
  end process;

  irq_pending_gen : for i in 0 to GPIO_WIDTH-1 generate

    irq_pending_proc : process(clock, reset)
    begin
      if (rising_edge(clock)) then
        if (reset = '1') then
          irq_pending_q(i) <= '0';
        elsif (irq_clear_q(i) = '1') then
          irq_pending_q(i) <= '0';
        elsif (irq_source_q(i) = '1') then
          irq_pending_q(i) <= '1';
        end if;
      end if;
    end process;

    irq_pending_out(i) <= irq_pending_q(i);
  end generate;

  output_reg_proc : process(clock, reset)
  begin
    if (rising_edge(clock)) then
        if (reset = '1') then
          irq_out <= '0';
        else
          irq_out <= or_reduce(irq_pending_q and irq_enable_q);
      end if;
    end if;
  end process;

end behavioral;
