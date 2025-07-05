library ieee;
use ieee.std_logic_1164.all;

entity tristate_array_onboard_tb is
end entity;

architecture tb of tristate_array_onboard_tb is


    component tristate_array_onboard is
        port
        (
            clk            : in    std_logic;
            rst_raw        : in    std_logic;
            buttons_inout  : inout std_logic_vector(1 downto 0);
            switch_inout   : inout std_logic;
            leds_inout     : inout std_logic_vector(3 downto 0)
        );
    end component;

    constant TB_CLK_PERIOD   : time      := 20 ns;
    signal   tb_clk          : std_logic := 'U';
    signal   tb_rst_raw          : std_logic := 'U';
    signal   tb_is_complete  : boolean   := false;

    signal tb_buttons_inout  : std_logic_vector (1 downto 0) := (others => 'U');
    signal tb_switch_inout   : std_logic                     := 'U';
    signal tb_leds_inout     : std_logic_vector (3 downto 0) := (others => 'U');

begin

    tb_clk_gen_proc : process
        begin
            tb_clk <= '0';
            wait for TB_CLK_PERIOD/2;
            tb_clk <= '1';
            wait for TB_CLK_PERIOD/2;

            if (tb_is_complete = true) then
                wait;
            end if;
        end process;

    tb_stimuli_proc : process
        begin
            tb_buttons_inout <= (others => '0');
            tb_switch_inout  <= '0';
            tb_rst_raw           <= '1';

            wait for 4*TB_CLK_PERIOD;

            assert tb_leds_inout(3) = '1'
                report "led(3) is not set in reset mode!"
                severity error;

            wait for 2*TB_CLK_PERIOD;

            tb_rst_raw <= '0';

            wait for 2*TB_CLK_PERIOD;

            tb_buttons_inout(0) <= '1';

            wait for 12*TB_CLK_PERIOD;

            assert tb_leds_inout(0) = '1'
                report "led(0) is not lighted up!"
                severity error;

            tb_buttons_inout(0) <= '0';

            wait for 10*TB_CLK_PERIOD;

            tb_buttons_inout <= (others => '1');

            wait for 12*TB_CLK_PERIOD;

            assert tb_leds_inout(0) = '1'
                report "led(0) is not lighted up!"
                severity error;

            assert tb_leds_inout(1) = '1'
                report "led(1) is not lighted up!"
                severity error;

            tb_buttons_inout <= (others => '0');

            wait for 10*TB_CLK_PERIOD;

            tb_switch_inout  <= '1';

            wait for 12*TB_CLK_PERIOD;

            tb_switch_inout  <= '0';

            assert tb_leds_inout(2) = '1'
                report "led(2) is not lighted up!"
                severity error;

            wait for 10*TB_CLK_PERIOD;

            tb_is_complete <= true;

            wait;

        end process;

    dut_inst : tristate_array_onboard
        port map
        (
            clk           => tb_clk,
            rst_raw       => tb_rst_raw,
            buttons_inout => tb_buttons_inout,
            switch_inout  => tb_switch_inout,
            leds_inout    => tb_leds_inout
        );

end architecture;