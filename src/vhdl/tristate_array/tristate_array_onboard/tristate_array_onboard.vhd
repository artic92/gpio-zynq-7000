library ieee;
use ieee.std_logic_1164.all;

entity tristate_array_onboard is
    port
    (
        clk            : in    std_logic;
        rst_raw        : in    std_logic;
        buttons_inout  : inout std_logic_vector(1 downto 0);
        switch_inout   : inout std_logic;
        leds_inout     : inout std_logic_vector(3 downto 0)
    );
end tristate_array_onboard;

architecture rtl of tristate_array_onboard is

    component ila_0
        port (
            clk    : in std_logic;
            probe0 : in std_logic
        );
    end component;

    component tristate_pad is
        port
        (
            pad_out_en : in    std_logic;   -- Enable output driver (1 = output, 0 = input/hi-Z)
            pad_out    : in    std_logic;   -- Value to drive when output enabled
            pad_in     : out   std_logic;   -- Value read from the pad
            pad_io     : inout std_logic    -- Bidirectional I/O pad
        );
    end component;

    component tristate_array is
        generic
        (
            TRISTATE_ARRAY_WIDTH : natural := 4
        );
        port
        (
            clock       : in    std_logic;
            reset       : in    std_logic;
            gpio_out_en : in    std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0);
            gpio_out    : in    std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0);
            gpio_in     : out   std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0);
            gpio_pin    : inout std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0)
        );
    end component;

    signal switch_tristate_pad_in_sig         : std_logic;
    signal buttons_tristate_array_gpio_in_sig : std_logic_vector(1 downto 0);
    signal leds_tristate_array_gpio_out_sig    : std_logic_vector(2 downto 0);

    signal rst_sync_0_q     : std_logic;
    signal rst_sync_1_q     : std_logic;
    signal switch_sync_0_q  : std_logic;
    signal switch_sync_1_q  : std_logic;
    signal buttons_sync_0_q : std_logic_vector(1 downto 0);
    signal buttons_sync_1_q : std_logic_vector(1 downto 0);

    type     integer_array is array (1 downto 0) of integer range 0 to 1_000_000;
    -- constant DEBOUNCE_THRESHOLD    : integer := 50_000;
    constant DEBOUNCE_THRESHOLD    : integer := 5;
    signal   debounce_cnts         : integer_array;
    signal   buttons_debounced_q   : std_logic_vector(1 downto 0);

begin

    rst_sync_proc : process (clk)
        begin
            if (rising_edge(clk)) then
                rst_sync_0_q <= rst_raw;
                rst_sync_1_q <= rst_sync_0_q;
            end if;
        end process;

    -- prevents synthesis optimisation of rst_sync_1_q logic
    leds_inout(3) <= rst_sync_1_q;

    u_ila_rst : ila_0
        port map
        (
            clk    => clk,
            probe0 => rst_sync_1_q
        );

    leds_tristate_array_inst : tristate_array
        generic map
        (
            TRISTATE_ARRAY_WIDTH => 3
        )
        port map
        (
            clock       => clk,
            reset       => rst_sync_1_q,
            gpio_out_en => "111", --write-only
            gpio_out    => leds_tristate_array_gpio_out_sig,
            gpio_in     => open,
            gpio_pin    => leds_inout(2 downto 0)
        );

    switch_tristate_pad_inst : tristate_pad
        port map
        (
            pad_out_en => '0', -- read-only
            pad_out    => '0',
            pad_in     => switch_tristate_pad_in_sig,
            pad_io     => switch_inout
        );

    switch_sync_proc : process (clk)
        begin
            if (rising_edge(clk)) then
                if (rst_sync_1_q = '1') then
                    switch_sync_0_q <= '0';
                    switch_sync_1_q <= '0';
                else
                    switch_sync_0_q <= switch_tristate_pad_in_sig;
                    switch_sync_1_q <= switch_sync_0_q;
                end if;
            end if;
        end process;

    leds_tristate_array_gpio_out_sig(2) <= switch_sync_1_q;

    buttons_tristate_array_inst : tristate_array
        generic map
        (
            TRISTATE_ARRAY_WIDTH => 2
        )
        port map
        (
            clock       => clk,
            reset       => rst_sync_1_q,
            gpio_out_en => (others => '0'), --read-only
            gpio_out    => (others => '0'),
            gpio_in     => buttons_tristate_array_gpio_in_sig,
            gpio_pin    => buttons_inout
        );

    buttons_sync_gen : for i in 0 to 1 generate
        buttons_sync_proc : process (clk)
        begin
            if (rising_edge(clk)) then
                if (rst_sync_1_q = '1') then
                    buttons_sync_0_q(i) <= '0';
                    buttons_sync_1_q(i) <= '0';
                else
                    buttons_sync_0_q(i) <= buttons_tristate_array_gpio_in_sig(i);
                    buttons_sync_1_q(i) <= buttons_sync_0_q(i);
                end if;
            end if;
        end process;
    end generate;

    buttons_debounce_gen : for i in 0 to 1 generate
        buttons_debounce_proc : process (clk)
        begin
            if (rising_edge(clk)) then
                if (rst_sync_1_q = '1') then
                    debounce_cnts(i)           <= 0;
                    buttons_debounced_q(i)     <= '0';
                elsif (buttons_sync_1_q(i) = buttons_debounced_q(i)) then
                    debounce_cnts(i)           <= 0;
                else
                    debounce_cnts(i)           <= debounce_cnts(i) + 1;
                    if (debounce_cnts(i) > DEBOUNCE_THRESHOLD) then
                        buttons_debounced_q(i) <= buttons_sync_1_q(i);
                    end if;
                end if;
            end if;
        end process;

        leds_tristate_array_gpio_out_sig(i) <= buttons_debounced_q(i);
    end generate;

end architecture;