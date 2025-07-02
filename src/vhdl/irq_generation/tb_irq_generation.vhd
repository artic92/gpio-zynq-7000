library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tb_irq_generation is
end tb_irq_generation;

architecture tb_arch of tb_irq_generation is

    component irq_generation is
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
    end component;

constant TB_CLK_PERIOD   : time       := 10 ns;
signal   tb_complete     : boolean    := false;
signal   tb_clock        : std_logic  := 'U';
signal   tb_reset        : std_logic  := 'U';

constant TB_GPIO_WIDTH       : natural                                     := 4;
signal   uut_irq_source_in   : std_logic_vector (TB_GPIO_WIDTH-1 downto 0) := (others => 'U');
signal   uut_irq_enable_in   : std_logic_vector (TB_GPIO_WIDTH-1 downto 0) := (others => 'U');
signal   uut_irq_clear_in    : std_logic_vector (TB_GPIO_WIDTH-1 downto 0) := (others => 'U');
signal   uut_irq_pending_out : std_logic_vector (TB_GPIO_WIDTH-1 downto 0) := (others => 'U');
signal   uut_irq_out         : std_logic;

begin

    -- Instantiate the DUT
    uut : irq_generation
    generic map
        (
            GPIO_WIDTH => TB_GPIO_WIDTH
        )
    port map (
        clock           => tb_clock,
        reset           => tb_reset,
        irq_source_in   => uut_irq_source_in,
        irq_enable_in   => uut_irq_enable_in,
        irq_clear_in    => uut_irq_clear_in,
        irq_pending_out => uut_irq_pending_out,
        irq_out         => uut_irq_out
    );

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
        tb_complete           <= false;
        uut_irq_enable_in     <= "1111";
        uut_irq_clear_in      <= "0000";

        -----------------------------------------------------------------------
        -- RESET phase
        -----------------------------------------------------------------------
        tb_reset              <= '1';
        uut_irq_source_in     <= "1010";

        wait for TB_CLK_PERIOD;

        assert (uut_irq_out = '0')
            report "IRQ should be low during reset!"
            severity error;

        assert (uut_irq_pending_out = "0000")
            report "IRQ pending should be zero during reset!"
            severity error;

        wait for TB_CLK_PERIOD;

        tb_reset             <= '0';

        -----------------------------------------------------------------------
        -- Trigger IRQ0
        -----------------------------------------------------------------------
        uut_irq_source_in    <= "0001";
        wait for 2*TB_CLK_PERIOD;
        uut_irq_source_in    <= "0000";

        wait for TB_CLK_PERIOD;

        assert uut_irq_pending_out(0) = '1'
            report "IRQ0 should be pending"
            severity error;

        assert uut_irq_out = '1'
            report "IRQ should be active due to IRQ0"
            severity error;

        uut_irq_clear_in <= "0001";
        wait for 2*TB_CLK_PERIOD;
        uut_irq_clear_in <= "0000";
        wait for TB_CLK_PERIOD;

        assert uut_irq_pending_out(0) = '0'
            report "IRQ0 should be cleared"
            severity error;
        assert uut_irq_out = '0'
            report "IRQ should be inactive after clearing IRQ0"
            severity error;

        -----------------------------------------------------------------------
        -- Trigger multiple IRQs
        -----------------------------------------------------------------------
        uut_irq_enable_in <= "1010";

        uut_irq_source_in <= "1001";
        wait for 2*TB_CLK_PERIOD;
        uut_irq_source_in <= "0000";
        wait for TB_CLK_PERIOD;

        assert uut_irq_pending_out(3) = '1'
            report "IRQ3 should be pending"
            severity error;
        assert uut_irq_out = '1'
            report "IRQ should be active (IRQ3 and IRQ0 enabled)"
            severity error;

        uut_irq_clear_in <= "1000";
        wait for 2*TB_CLK_PERIOD;
        uut_irq_clear_in <= "0000";
        wait for TB_CLK_PERIOD;

        assert uut_irq_pending_out(3) = '0'
            report "IRQ3 should be cleared"
            severity error;
        assert uut_irq_out = '0'
            report "IRQ should be cleared after IRQ3 ack"
            severity error;

        -----------------------------------------------------------------------
        -- Trigger IRQ0
        -----------------------------------------------------------------------
        uut_irq_enable_in <= "0001";
        uut_irq_source_in <= "0001";

        wait for 3*TB_CLK_PERIOD;

        assert uut_irq_pending_out(0) = '1'
            report "IRQ3 should be pending"
            severity error;
        assert uut_irq_out = '1'
            report "IRQ should be active (IRQ3 enabled)"
            severity error;

        uut_irq_clear_in <= "0001";
        wait for 2*TB_CLK_PERIOD;
        uut_irq_clear_in <= "0000";
        wait for TB_CLK_PERIOD;

        assert uut_irq_pending_out(0) = '0'
            report "IRQ3 should be cleared"
            severity error;
        assert uut_irq_out = '0'
            report "IRQ should be cleared after IRQ3 ack"
            severity error;

        -----------------------------------------------------------------------
        -- IRQ0 is pending but not enabled
        -----------------------------------------------------------------------
        uut_irq_enable_in <= "0000";
        uut_irq_source_in <= "0001";

        wait for 3*TB_CLK_PERIOD;

        assert uut_irq_pending_out(0) = '1'
            report "IRQ0 should be pending but not triggered"
            severity error;
        assert uut_irq_out = '0'
            report "IRQ0 should not be active"
            severity error;

        wait for 10*TB_CLK_PERIOD;

        tb_complete <= true;

        wait;

    end process;

end architecture;