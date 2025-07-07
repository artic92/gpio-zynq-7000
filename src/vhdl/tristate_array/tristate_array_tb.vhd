library ieee;
use ieee.std_logic_1164.all;

entity tristate_array_tb is
end tristate_array_tb;

architecture sim of tristate_array_tb is


    constant TB_CLK_PERIOD   : time       := 10 ns;
    signal   tb_complete     : boolean    := false;
    signal   tb_clock        : std_logic  := 'U';
    signal   tb_reset        : std_logic  := 'U';

    -- DUT ports
    constant TRISTATE_RD_LATENCY_CLK_CYCLES : time             := 3*TB_CLK_PERIOD;
    constant WIDTH                          : natural          := 4;
    signal tb_gpio_out_en : std_logic_vector(WIDTH-1 downto 0) := (others => 'U');
    signal tb_gpio_out    : std_logic_vector(WIDTH-1 downto 0) := (others => 'U');
    signal tb_gpio_in     : std_logic_vector(WIDTH-1 downto 0) := (others => 'U');
    signal tb_gpio_pin    : std_logic_vector(WIDTH-1 downto 0) := (others => 'U');

    component tristate_array is
        generic (
            TRISTATE_ARRAY_WIDTH : natural := 4
        );
        port (
            clock       : in    std_logic;
            reset       : in    std_logic;
            gpio_out_en : in    std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0);
            gpio_out    : in    std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0);
            gpio_in     : out   std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0);
            gpio_pin    : inout std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0)
        );
    end component;

begin

    -- DUT instantiation
    dut : tristate_array
        generic map
        (
            TRISTATE_ARRAY_WIDTH => WIDTH
        )
        port map (
            clock       => tb_clock,
            reset       => tb_reset,
            gpio_out_en => tb_gpio_out_en,
            gpio_out    => tb_gpio_out,
            gpio_in     => tb_gpio_in,
            gpio_pin    => tb_gpio_pin
        );

    -- Clock generation
    tb_clk_proc : process
    begin

        tb_clock <= '0';
        wait for TB_CLK_PERIOD/2;
        tb_clock <= '1';
        wait for TB_CLK_PERIOD/2;

        if (tb_complete = true) then
            wait;
        end if;
    end process;

    tb_stim_proc : process
    begin
        tb_complete <= false;

        tb_gpio_pin <= "ZZZZ";

        -----------------------------------------------------------------------
        -- RESET phase
        -----------------------------------------------------------------------
        tb_gpio_out_en <= "1010";
        tb_gpio_out    <= "0101";
        tb_reset       <= '1';

        wait for TRISTATE_RD_LATENCY_CLK_CYCLES;

        assert (tb_gpio_in = "0000")
            report "gpio_in should be zero during reset!"
            severity error;

        tb_reset    <= '0';

        wait for TB_CLK_PERIOD;

        -----------------------------------------------------------------------
        -- All outputs enabled (writing mode)
        -----------------------------------------------------------------------
        tb_gpio_pin    <= "ZZZZ";
        tb_gpio_out_en <= "1111";
        tb_gpio_out    <= "1010";

        wait for TRISTATE_RD_LATENCY_CLK_CYCLES;

        assert (tb_gpio_in = "1010")
            report "tb_gpio_pin should follow ext_drive during a writing!"
            severity error;

        wait for TB_CLK_PERIOD;

        -----------------------------------------------------------------------
        -- Simulate external driving (reading mode)
        -----------------------------------------------------------------------
        tb_gpio_out_en <= "0000";
        wait for TB_CLK_PERIOD;
        tb_gpio_pin    <= "1111";

        wait for TRISTATE_RD_LATENCY_CLK_CYCLES;

        assert (tb_gpio_in = "1111")
            report "gpio_in should follow the pin input!"
            severity error;

        wait for TB_CLK_PERIOD;

        -- -----------------------------------------------------------------------
        -- -- Mixed direction (writing pins 0 and 3, read pins 1 and 2)
        -- -----------------------------------------------------------------------
        tb_gpio_out_en <= "1001";
        tb_gpio_pin(0) <= 'Z';
        tb_gpio_pin(3) <= 'Z';
        wait for TB_CLK_PERIOD;
        tb_gpio_pin(1) <= '1';
        tb_gpio_pin(2) <= '1';
        tb_gpio_out    <= "1001";

        wait for TRISTATE_RD_LATENCY_CLK_CYCLES;

        assert (tb_gpio_in = "1111")
            report "gpio_in should follow ext_drive!"
            severity error;

        wait for 5*TB_CLK_PERIOD;

        tb_complete <= true;

        wait;
    end process;

end sim;
