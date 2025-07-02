library ieee;
use ieee.std_logic_1164.all;

entity my_gpio is
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
end entity;

architecture structural of my_gpio is

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

    signal gpio_in_sig : std_logic_vector(GPIO_WIDTH-1 downto 0);

begin

    tristate_array_inst : tristate_array
        generic map
        (
            TRISTATE_ARRAY_WIDTH => GPIO_WIDTH
        )
        port map
        (
            clock       => clock,
            reset       => reset,
            gpio_out_en => data_out_en_in,
            gpio_out    => data_in,
            gpio_in     => gpio_in_sig,
            gpio_pin    => gpio_pin_inout
        );

    data_out <= gpio_in_sig;

    irq_generation_gen : if (INT_ENABLED = true) generate
        irq_generation_inst : irq_generation
            generic map
            (
                GPIO_WIDTH => GPIO_WIDTH
            )
            port map
            (
                clock           => clock,
                reset           => reset,
                irq_source_in   => gpio_in_sig,
                irq_enable_in   => int_enable_in,
                irq_clear_in    => int_clear_in,
                irq_pending_out => int_pending_out,
                irq_out         => irq_out
            );
    end generate;

end architecture;