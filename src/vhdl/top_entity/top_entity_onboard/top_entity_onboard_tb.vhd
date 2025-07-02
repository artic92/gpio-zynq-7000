library ieee;
use ieee.std_logic_1164.all;

entity top_entity_tb is
end entity;

architecture tb of top_entity_tb is

    component top_entity_onboard is
        port
        (
            clk      : in  std_logic;
            rst      : in  std_logic;
            buttons  : in  std_logic_vector(3 downto 0);
            switches : in  std_logic_vector(3 downto 0);
            leds     : out std_logic_vector(3 downto 0)
        );
    end component;

    constant TB_CLK_PERIOD : time    := 20 ns;
    signal tb_is_complete  : boolean := false;

    signal tb_clock    : std_logic                    := 'U';
    signal tb_reset    : std_logic                    := 'U';
    signal tb_buttons  : std_logic_vector(3 downto 0) := (others => 'U');
    signal tb_switches : std_logic_vector(3 downto 0) := (others => 'U');
    signal tb_leds     : std_logic_vector(3 downto 0) := (others => 'U');

begin

    clk_gen_proc : process
    begin
        tb_clock <= '0';
        wait for TB_CLK_PERIOD/2;
        tb_clock <= '1';
        wait for TB_CLK_PERIOD/2;

        if (tb_is_complete = true) then
            wait;
        end if;
    end process;

    tb_stimuli : process
    begin
        tb_reset       <= '1';
        tb_switches(0) <= '0';
        tb_buttons(0)  <= '1';

        wait for 4*TB_CLK_PERIOD;

        tb_buttons(0)  <= '0';
        tb_reset       <= '0';

        wait for 4*TB_CLK_PERIOD;

        tb_switches(0) <= '1';

        wait for 4*TB_CLK_PERIOD;

        tb_buttons(0)  <= '1';

        wait for 10*TB_CLK_PERIOD;

        tb_buttons(0)  <= '0';

        wait for 20*TB_CLK_PERIOD;

        tb_is_complete <= true;

        wait;

    end process;

    uut_inst : top_entity_onboard
    port map
        (
            clk      => tb_clock,
            rst      => tb_reset,
            buttons  => tb_buttons,
            switches => tb_switches,
            leds     => tb_leds
        );

end architecture;