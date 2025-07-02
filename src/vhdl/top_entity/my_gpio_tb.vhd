library ieee;
use ieee.std_logic_1164.all;

entity my_gpio_tb is
end entity;

architecture tb of my_gpio_tb is

    component my_gpio is
        generic
        (
            GPIO_WIDTH  : natural := 4;
            INT_ENABLED : boolean := true
        );
        port
        (
            clock           : in    std_logic;
            reset           : in    std_logic;
            data_in         : in    std_logic_vector(GPIO_WIDTH-1 downto 0);
            data_out_en_in  : in    std_logic_vector(GPIO_WIDTH-1 downto 0);
            int_enable_in   : in    std_logic_vector(GPIO_WIDTH-1 downto 0);
            int_clear_in    : in    std_logic_vector(GPIO_WIDTH-1 downto 0);
            int_pending_out : out   std_logic_vector(GPIO_WIDTH-1 downto 0);
            irq_out         : out   std_logic;
            data_out        : out   std_logic_vector(GPIO_WIDTH-1 downto 0);
            gpio_pin_inout  : inout std_logic_vector(GPIO_WIDTH-1 downto 0)
        );
    end component;

    constant TB_CLK_PERIOD      : time      := 20 ns;
    signal   tb_clock           : std_logic := 'U';
    signal   tb_reset           : std_logic := 'U';
    signal   tb_is_complete     : boolean   := false;

    constant TB_GPIO_WIDTH      : natural   := 4;
    constant TB_INT_ENABLED     : boolean   := true;
    signal   tb_data_in         : std_logic_vector(TB_GPIO_WIDTH-1 downto 0) := (others => 'U');
    signal   tb_data_out_en_in  : std_logic_vector(TB_GPIO_WIDTH-1 downto 0) := (others => 'U');
    signal   tb_int_enable_in   : std_logic_vector(TB_GPIO_WIDTH-1 downto 0) := (others => 'U');
    signal   tb_int_clear_in    : std_logic_vector(TB_GPIO_WIDTH-1 downto 0) := (others => 'U');
    signal   tb_int_pending_out : std_logic_vector(TB_GPIO_WIDTH-1 downto 0) := (others => 'U');
    signal   tb_irq_out         : std_logic                               := 'U';
    signal   tb_data_out        : std_logic_vector(TB_GPIO_WIDTH-1 downto 0) := (others => 'U');
    signal   tb_gpio_pin_inout  : std_logic_vector(TB_GPIO_WIDTH-1 downto 0) := (others => 'U');

begin

    tb_clk_gen_proc : process
    begin
        tb_clock <= '0';
        wait for TB_CLK_PERIOD/2;
        tb_clock <= '1';
        wait for TB_CLK_PERIOD/2;

        if (tb_is_complete = true) then
            wait;
        end if;
    end process;

    tb_stimulus_proc : process
    begin
        tb_gpio_pin_inout <= "ZZZZ";

        ----------------------------------------
        -- reset state
        ----------------------------------------
        tb_data_in        <= "1111";
        tb_data_out_en_in <= "1111";
        tb_int_enable_in  <= "1111";
        tb_int_clear_in   <= "0000";
        tb_reset          <= '1';

        wait for 5*TB_CLK_PERIOD;

        ----------------------------------------
        -- input mode
        ----------------------------------------
        tb_reset          <= '0';
        tb_data_in        <= "1010";
        tb_data_out_en_in <= "0000";
        tb_int_enable_in  <= "0100";
        wait for 2*TB_CLK_PERIOD;
        tb_gpio_pin_inout <= "0101";

        wait for 10*TB_CLK_PERIOD;

        assert tb_data_out(0) = '1'
            report "data_out(0) should be high"
            severity error;

        assert tb_int_pending_out(0) = '1'
            report "IRQ0 should be pending"
            severity error;

        assert tb_irq_out = '1'
            report "IRQ (IRQ0) should be active"
            severity error;

        tb_int_clear_in   <= "0100";
        wait for 2*TB_CLK_PERIOD;
        tb_int_clear_in   <= "0100";

        assert tb_int_pending_out(0) = '1'
            report "IRQ0 should not be pending"
            severity error;

        assert tb_irq_out = '1'
            report "IRQ should not be active"
            severity error;

        ----------------------------------------
        -- output mode
        ----------------------------------------
        wait for 2*TB_CLK_PERIOD;

        tb_reset          <= '0';
        tb_data_in        <= "0101";
        tb_data_out_en_in <= "1111";
        tb_int_enable_in  <= "0000";
        tb_gpio_pin_inout <= "ZZZZ";

        wait for 10*TB_CLK_PERIOD;

        assert tb_gpio_pin_inout(0) = '1'
            report "tb_gpio_pin_inout(0) should be high"
            severity error;

        assert tb_int_pending_out(1) = '0'
            report "IRQ should not be pending"
            severity error;

        assert tb_irq_out = '0'
            report "IRQ should not be active"
            severity error;


        wait for 10*TB_CLK_PERIOD;

        tb_is_complete <= true;

        wait;

    end process tb_stimulus_proc;

    uut_inst : my_gpio
        generic map
        (
            GPIO_WIDTH  => TB_GPIO_WIDTH,
            INT_ENABLED => TB_INT_ENABLED
        )
        port map
        (
            clock           => tb_clock,
            reset           => tb_reset,
            data_in         => tb_data_in,
            data_out_en_in  => tb_data_out_en_in,
            int_enable_in   => tb_int_enable_in,
            int_clear_in    => tb_int_clear_in,
            int_pending_out => tb_int_pending_out,
            irq_out         => tb_irq_out,
            data_out        => tb_data_out,
            gpio_pin_inout  => tb_gpio_pin_inout
        );

end architecture;