library ieee;
use ieee.std_logic_1164.all;

entity tristate_array is
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
        gpio_in     : out   std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0); -- synchronised with local clock
        gpio_pin    : inout std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0)
    );
end tristate_array;

architecture structural of tristate_array is

    component tristate_pad is
        port
        (
            pad_out_en : in    std_logic;
            pad_out    : in    std_logic;
            pad_in     : out   std_logic;
            pad_io     : inout std_logic
        );
    end component;

    signal gpio_en_q  : std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0);
    signal gpio_out_q : std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0);
    signal gpio_in_d  : std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0);
    signal gpio_in_sync_0_q  : std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0);
    signal gpio_in_sync_1_q  : std_logic_vector (TRISTATE_ARRAY_WIDTH-1 downto 0);

begin

    input_reg_proc : process (clock, reset)
    begin

        if (rising_edge(clock)) then

            if (reset = '1') then
                gpio_en_q  <= (others => '0');
                gpio_out_q <= (others => '0');
            else
                gpio_en_q  <= gpio_out_en;
                gpio_out_q <= gpio_out;
            end if;

        end if;

    end process;

    tristate_array_gen : for i in 0 to TRISTATE_ARRAY_WIDTH-1 generate

        gpio_inst : tristate_pad
            port map
            (
                pad_out_en => gpio_en_q(i),
                pad_out    => gpio_out_q(i),
                pad_in     => gpio_in_d(i),
                pad_io     => gpio_pin(i)
            );

    end generate;

    output_sync_proc : process (clock, reset)
    begin

        if (rising_edge(clock)) then
            if (reset = '1') then
                gpio_in_sync_0_q <= (others => '0');
                gpio_in_sync_1_q <= (others => '0');
            else
                gpio_in_sync_0_q <= gpio_in_d;
                gpio_in_sync_1_q <= gpio_in_sync_0_q;
            end if;
        end if;

    end process;

    gpio_in <= gpio_in_sync_1_q;

end structural;
